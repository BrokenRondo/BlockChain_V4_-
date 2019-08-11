#include "BlockchainSyn.h"
int BlockchainSyn::Routine()
{
	OS_TcpSocket server_sock;
	OS_SockAddr sock_addr(Ip.c_str(), Port);
	server_sock.Open(sock_addr);
	if (server_sock.Ioctl_SetBlockedIo(false) < 0)
	{
		cout << "�˿�" << Port << "���÷�����ʧ��,���ܰ�IP���ʹ���\n";
	}
	server_sock.Listen();
	printf("ͬ����������������������˿�%d\n", Port);
	OS_Thread::Msleep(50);
	while (1)
	{
		sockaddr_in addr;
		SOCKET s;
		int len = sizeof(addr);
		s = accept(server_sock.hSock, (sockaddr*)& addr, &len);
		if (s == SOCKET_ERROR)
		{
			int r = WSAGetLastError();
			if (r == WSAEWOULDBLOCK)
			{
				OS_Thread::Msleep(500);
				continue;
			}
			else
			{
				cout << Port << "�˿ڰ󶨵ķ��������ɷ�������error code:" << r << endl;
				return false;
			}
		}
		else
		{
			BlockchainSynRun* pClient = new BlockchainSynRun(s, addr);
			if (!pClient->startRunning())cout << "����������\n";//�����ӽ��ܺͷ����߳̿�ʼִ�С�
		}
	}
	server_sock.Close();
	return 0;
}

BlockchainSynRun::BlockchainSynRun(SOCKET s, sockaddr_in addr)
{
	//��ʼ������Ա������
	m_socket = s;
	m_addr = addr;
	m_hRecvThread = NULL;
	m_hSendThread = NULL;
	m_IsConnected = true;
	m_IsSendData = true;
	m_hEvent = CreateEvent(NULL, true, false, NULL);
	m_pRecvData = new char[BUF_SIZE];
	m_pSendData = new char[BUF_SIZE];
	memset(m_pSendData, 0, BUF_SIZE);
	memset(m_pRecvData, 0, BUF_SIZE);
}

BlockchainSynRun::~BlockchainSynRun(void)
{
	delete[]m_pRecvData;
	delete[]m_pSendData;
}

bool BlockchainSynRun::IsConnected()
{
	return m_IsConnected;
}

bool BlockchainSynRun::set_senddata(std::string data)
{
	strcpy(m_pSendData, data.c_str());
	return true;
}

DWORD WINAPI BlockchainSynRun::sendThread(void* param)//�����߳���ں�����
{
	//std::cout << "���������߳̿�ʼ���У���" << std::endl;
	BlockchainSynRun* pClient = static_cast<BlockchainSynRun*>(param);//���BlockchainSynRun����ָ�롣�Ա���ݳ�Ա������
	WaitForSingleObject(pClient->m_hEvent, INFINITE);//�ȴ����������߳�֪ͨ��
	while (pClient->m_IsConnected)
	{
		while (pClient->m_IsSendData)//���Է������ݡ�
		{
			int ret = send(pClient->m_socket, pClient->m_pSendData, strlen(pClient->m_pSendData), 0);
			if (ret == SOCKET_ERROR)
			{
				int r = WSAGetLastError();
				if (r == WSAEWOULDBLOCK)
				{
					continue;
				}
				else
				{
					return 0;
				}
			}
			else
			{
				//std::cout << "������ͳɹ�����" << std::endl;
				pClient->m_IsSendData = false;
				break;
			}

		}
		Sleep(1000);//δ�յ�����֪ͨ��˯��1�롣
	}
	delete pClient;
	return 0;
}

DWORD WINAPI BlockchainSynRun::recvThread(void* param)//���������߳���ں�����
{
	//std::cout << "���������߳̿�ʼ���У���" << std::endl;
	BlockchainSynRun* pClient = static_cast<BlockchainSynRun*>(param);
	while (pClient->m_IsConnected)
	{
		memset(pClient->m_pRecvData, 0, BUF_SIZE);
		int ret = recv(pClient->m_socket, pClient->m_pRecvData, BUF_SIZE, 0);
		if (ret == SOCKET_ERROR)
		{
			int r = WSAGetLastError();
			if (r == WSAEWOULDBLOCK)
			{
				Sleep(20);
				continue;
			}
			else if (r == WSAENETDOWN)
			{
				cout << "���������̳߳��ִ���,�����жϣ�" << endl;
				break;
			}
			else if (r == WSAECONNRESET || r == WSAECONNABORTED)
			{
				//std::cout << "�ͻ���ǿ���ж�" << std::endl;
				break;
			}
			else
			{
				//std::cout << "���������̳߳��ִ���error code:" <<r<< std::endl;
				break;
			}
		}
		else if (ret == 0)
		{
			//std::cout << "�û��뿪���ر����ӣ�" << std::endl;
			pClient->DisConnect();
			break;
		}
		else
		{
			//���յ��������ݣ�������Ӧ����
			string result = pClient->m_pRecvData;
			//�Է������ȡ����������
			if (result[0] == GET_LEN)
			{
				string length = to_string(CountBlock());
				pClient->set_senddata(length);
			}
			//�Է�����ͬ��������
			else
			{
				//�Ƚ�resultǰ5λ�Ƿ�Ϊindex��������ر�����
				char indexstr[6] = { 0 };
				memcpy(indexstr, result.c_str(), 5);
				if (strcmp(indexstr, "index") != 0)
				{
					cout << "ͬ������ָ����󣬷�indexָ��\n";
					pClient->DisConnect();
					ExitThread((DWORD)pClient->sendThread);
					delete pClient;
					return -1;
				}
				//�����index�����ȡҪͬ����λ��
				unsigned int location = atoi(result.substr(5).c_str());
				cout << "���յ�������ͬ��ָ��,ͬ����ʼ�������"+to_string(location)+"\n";
				Blocknum = CountBlock();
				for (int i = location; i < Blocknum; i++)
				{
					//��ȡ�������ݲ��ְ�
					string Blockdata = ReadBlock("block" + std::to_string(i));	
					vector<string> vector_blockdata = pClient->blockcut(Blockdata);
					for (unsigned int j=0;j<vector_blockdata.size();j++)
					{
						int ret = send(pClient->m_socket, vector_blockdata[j].c_str(), strlen(vector_blockdata[j].c_str()), 0);
						Sleep(200);
					}
				}
				char newbuf[2];
				newbuf[0] = ALL_END; newbuf[1] = '\0';
				pClient->set_senddata(newbuf);
			}
			//�߳�δ�ͷ�
			//�������
			SetEvent(pClient->m_hEvent);
			pClient->m_IsSendData = true;
		}
	}
	//��ֹ�����̣߳�������deletepClient�����̻߳�����쳣
	ExitThread((DWORD)pClient->sendThread);
	delete pClient;
	return 0;
}

bool BlockchainSynRun::startRunning()
{
	m_hRecvThread = CreateThread(NULL, 0, recvThread, (void*)this, 0, NULL);//����static��Ա�������޷��������Ա����˴���thisָ�롣
	if (m_hRecvThread == NULL)
	{
		return false;
	}
	m_hSendThread = CreateThread(NULL, 0, sendThread, (void*)this, 0, NULL);
	if (m_hSendThread == NULL)
	{
		return false;
	}
	return true;
}

bool BlockchainSynRun::DisConnect()
{
	m_IsConnected = false;
	return true;
}

vector<string> BlockchainSynRun::blockcut(string blockdata)
{
	vector<string> result;
	int packsize = BUF_SIZE - 4;
	char pchar[BUF_SIZE];
	for (int j = 0; j < blockdata.size(); j++)
	{
		if (j % packsize == 0 && j != 0)
		{
			//һ����װ����
			pchar[0] = CONTINUE_DATA;
			pchar[packsize+1] = '\0';
			result.push_back(pchar);
		}
		//��װ����
		pchar[j % packsize + 1] = blockdata[j];
		if (j == blockdata.size() - 1)
		{
			//������ûװ�����Ǳ��ָ�������Ѿ�װ����
			pchar[0] = LAST_DATA;
			pchar[j % packsize + 2] = '\0';
			result.push_back(pchar);
		}
	}
	return result;
}
