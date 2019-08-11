#include"NetListening.h"
int NetListening::Routine()
{
	OS_TcpSocket server_sock;
	OS_SockAddr sock_addr(Ip.c_str(), Port);
	server_sock.Open(sock_addr);
	if (server_sock.Ioctl_SetBlockedIo(false) < 0)
	{
		cout << "�˿�" << Port << "���÷�����ʧ��,���ܰ�IP���ʹ���\n";
	}
	server_sock.Listen();
	printf("�ڵ���������̷߳������������˿�%d\n", Port);
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
			NetListeningRun* pClient = new NetListeningRun(s, addr);
			if(!pClient->startRunning())cout<<"����������\n";//�����ӽ��ܺͷ����߳̿�ʼִ�С�
		}
	}
	server_sock.Close();
	return 0;
}

NetListeningRun::NetListeningRun(SOCKET s, sockaddr_in addr)
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

NetListeningRun::~NetListeningRun(void)
{
	delete[]m_pRecvData;
	delete[]m_pSendData;
}

bool NetListeningRun::IsConnected()
{
	return m_IsConnected;
}

bool NetListeningRun::set_senddata(std::string data)
{
	strcpy(m_pSendData, data.c_str());
	return true;
}

DWORD WINAPI NetListeningRun::sendThread(void* param)//�����߳���ں�����
{
	//std::cout << "���������߳̿�ʼ���У���" << std::endl;
	NetListeningRun* pClient = static_cast<NetListeningRun*>(param);//���NetListeningRun����ָ�롣�Ա���ݳ�Ա������
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

DWORD WINAPI NetListeningRun::recvThread(void* param)//���������߳���ں�����
{
	//std::cout << "���������߳̿�ʼ���У���" << std::endl;
	NetListeningRun* pClient = static_cast<NetListeningRun*>(param);
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
				case NODE_JOIN:
				{
					cout << "���յ��ڵ�������������:" + peerIP + "\n";
					temp = "this";
					temp[0] = REPLY_JOIN;
					pClient->set_senddata(temp.c_str());
					cout << "����Ӧ�ڵ�������������:" + peerIP+"\n";
					break;
				}
				case NODE_IN:
				{
					cout << "���ֽڵ��������"+peerIP+"\n";
					temp = temp.substr(1);
					string IP_temp = nodeIP(temp);
					//�ж��Ƿ����½ڵ�
					if (!isNewNode(IP_temp))
					{
						cout << "�ڵ�" << peerIP << "�Ѵ��ڣ��������\n";
					}
					else
					{
						std::ofstream fout;
						fout.open(NODELIST_PATH, ios::app);//׷�ӷ�ʽ��
						if (!fout)
						{
							printf("�ڵ��б��ʧ�ܣ������¼���Ľڵ�ʧ�ܣ�\n");
							abort();
						}
						else
						{
							Nodes_vector.push_back(IP_temp);
							string Port_temp = to_string(nodePort(temp));
							string nodekey = nodePublicKey(temp);
							Public_Key key_i = { IP_temp,nodekey };
							IP_temp += ":";
							Port_temp += ",";
							unsigned dis = getIPCount((char*)LocalIP.c_str(), (char*)IP_temp.c_str());//�����¼���Ľڵ���Լ��ľ���
							fout << std::left << setw(20) << IP_temp << setw(8) << Port_temp << dis << endl;
							public_keys.push_back(key_i);
							cout<<"�Ѹ����¼���ڵ�"+peerIP+"\n";
							fout.close();

							Create_matrix();
						}
					}
					break;
				}
				case NODE_HEARTGET:
				{
					cout << "���յ����Խڵ����������:" +peerIP +"\n";
					temp[0] = REPLY_HEART;
					pClient->set_senddata(temp.c_str());
					cout << "����Ӧ���Խڵ����������:" +peerIP + "\n";
					if (isNewNode(peerIP))
					{
						Nodes_vector.push_back(peerIP);
						peerIP += ":";
						std::ofstream fout(NODELIST_PATH, ios::app);
						fout << std::left << setw(20) << peerIP<< setw(8) << to_string(PORT_NODELISTENING)+"," << getIPCount((char*)peerIP.c_str(), (char*)LocalIP.c_str()) << endl;
						fout.close();
					}
					break;
				}
				case NODE_LIST:
				{
					cout << "���յ����Խڵ���б��ȡ����:" +peerIP + "\n";
					ifstream fin(NODELIST_PATH, ios::binary);
					string ss = "";
					while (!fin.eof())
					{
						string IP_temp, Port_temp, Distance_temp;
						string public_key;
						fin >> IP_temp >> Port_temp >> Distance_temp;
						for(unsigned int i=0;i<public_keys.size();i++)
						{
							if (public_keys[i].IP == IP_temp)
							{
								public_key = public_keys[i].key;
								break;
							}
						}
						if (IP_temp != "")
							ss = ss + IP_temp + Port_temp + public_key + '$';
					}
					fin.close();
					ss += '@';
					pClient->set_senddata(ss.c_str());
					cout << "����Ӧ���Խڵ���б��ȡ����:" + peerIP + "\n";
					break;
				}
				default:
				{
					cout << "���յ����Խڵ��޷�ʶ�������:" +peerIP + "\n";
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

bool NetListeningRun::startRunning()
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

bool NetListeningRun::DisConnect()
{
	m_IsConnected = false;
	return true;
}
