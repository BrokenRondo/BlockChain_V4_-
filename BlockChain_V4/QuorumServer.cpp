#include "QuorumServer.h"
#define HEIGHT_DATA 0x01
#define HEIGHT_GET 0x02
#define SIG_GET 0x03
int QuorumServer::Routine()
{
	OS_TcpSocket server_sock;
	OS_SockAddr sock_addr(Ip.c_str(), Port);
	server_sock.Open(sock_addr);
	if (server_sock.Ioctl_SetBlockedIo(false) < 0)
	{
		cout << "�˿�" << Port << "���÷�����ʧ��,���ܰ�IP���ʹ���\n";
	}
	server_sock.Listen();
	printf("�ٲ÷������������˿�%d\n", Port);
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
			QuorumServerRun* pClient = new QuorumServerRun(s, addr);
			if (!pClient->startRunning())cout << "����������\n";//�����ӽ��ܺͷ����߳̿�ʼִ�С�
		}
	}
	server_sock.Close();
	return 0;
}

QuorumServerRun::QuorumServerRun(SOCKET s, sockaddr_in addr)
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

QuorumServerRun::~QuorumServerRun(void)
{
	delete[]m_pRecvData;
	delete[]m_pSendData;
}

bool QuorumServerRun::IsConnected()
{
	return m_IsConnected;
}

bool QuorumServerRun::set_senddata(std::string data)
{
	strcpy(m_pSendData, data.c_str());
	return true;
}

DWORD WINAPI QuorumServerRun::sendThread(void* param)//�����߳���ں�����
{
	//std::cout << "���������߳̿�ʼ���У���" << std::endl;
	QuorumServerRun* pClient = static_cast<QuorumServerRun*>(param);//���QuorumServerRun����ָ�롣�Ա���ݳ�Ա������
	WaitForSingleObject(pClient->m_hEvent, INFINITE);//�ȴ����������߳�֪ͨ��
	while (pClient->m_IsConnected)
	{
		while (pClient->m_IsSendData)//���Է������ݡ�
		{
			//std::cout << "�ȴ����������߳�֪ͨ����" << std::endl;
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

DWORD WINAPI QuorumServerRun::recvThread(void* param)//���������߳���ں�����
{
	//std::cout << "���������߳̿�ʼ���У���" << std::endl;
	QuorumServerRun* pClient = static_cast<QuorumServerRun*>(param);
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
			string temp = pClient->m_pRecvData;
			char request = temp[0];
			string peerIP = std::string(inet_ntoa(pClient->m_addr.sin_addr));//�ͻ���IP
			unsigned short peerPort = ntohs(pClient->m_addr.sin_port);//�ͻ���Port
			switch (request)
			{
				case HEIGHT_DATA:
				{
					//�յ������ڵ㷢�͵ĸ߶���Ϣ�����뼯����
					temp = temp.substr(1);
					cout << "���յ��߶���ǩ��" << temp << endl;
					//����#�ָ�߶Ⱥ�ǩ��
					int pos = temp.find('#');
					string height = temp.substr(0, pos);//�߶�
					string sig = temp.substr(pos + 1);//ǩ����Ŀǰ��IP���
					bool value = false;
					//*********************************************************
					//�ж�ǩ���Ƿ���Ч
					if (sig == sig)
						value = true;
					//**********************************************************
					if (value)//ǩ����Ч�����뼯��
					{
						bool no_exit = true;//Ĭ�ϲ����ڸø߶�
						for (unsigned int i = 0; i < height_set.size(); i++)
						{
							if (height_set[i][0] == height)
							{
								//�Ѵ��ڴ˸߶ȣ�ֱ�����ǩ��
								no_exit = false;
								height_set[i][1] +=sig+'~';
								break;
							}
						}
						if (no_exit)
						{
							//�����ڴ˸߶ȣ��������
							vector<string> height_temp;
							height_temp.push_back(height);
							height_temp.push_back(sig+'~');
							height_set.push_back(height_temp);
						}
					}
					break;
				}
				case HEIGHT_GET:
				{
					//�����ȡ����
					string senddata;
					//��ȡ��ǰ�ڵ�����
					//ifstream fin(NODELIST_PATH);
					//int node_num = 0;
					//while (getline(fin, temp))
					//{
					//	if (temp == "")continue;//��������ֱ������
					//	node_num++;
					//}
					//fin.close();
					int node_num = Nodes_vector.size();
					//ɸѡ���ϣ���ȡ��Ч��߸߶�
					int max_height = 0;
					for (unsigned int i = 0; i < height_set.size(); i++)
					{
						temp = height_set[i][1];//ǩ������
						int num = count(temp.begin(), temp.end(), '~');
						if (num > node_num / 3)
						{
							//ǩ�������㹻
							if (atoi(height_set[i][0].c_str()) > max_height)
							{
								max_height = atoi(height_set[i][0].c_str());
								SynIPSig = temp;//IPsig
							}
						}
					}
					senddata = to_string(max_height)+"#"+LocalIP;
					pClient->set_senddata(senddata.c_str());
					break;
				}
				case SIG_GET:
				{
					//�����ȡͬ������IP
					string senddata = SynIPSig;
					pClient->set_senddata(senddata.c_str());
					break;
				}
				default:
				{
					break;
				}
			}
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

bool QuorumServerRun::startRunning()
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

bool QuorumServerRun::DisConnect()
{
	m_IsConnected = false;
	return true;
}
