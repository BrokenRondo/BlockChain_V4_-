#include"NetListening.h"
int NetListening::Routine()
{
	OS_TcpSocket server_sock;
	OS_SockAddr sock_addr(Ip.c_str(), Port);
	server_sock.Open(sock_addr);
	if (server_sock.Ioctl_SetBlockedIo(false) < 0)
	{
		cout << "端口" << Port << "设置非阻塞失败,可能绑定IP发送错误\n";
	}
	server_sock.Listen();
	printf("节点请求监听线程服务器启动，端口%d\n", Port);
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
				cout << Port << "端口绑定的服务器主干发生错误：error code:" << r << endl;
				return false;
			}
		}
		else
		{
			NetListeningRun* pClient = new NetListeningRun(s, addr);
			if(!pClient->startRunning())cout<<"服务发生错误\n";//该链接接受和发送线程开始执行。
		}
	}
	server_sock.Close();
	return 0;
}

NetListeningRun::NetListeningRun(SOCKET s, sockaddr_in addr)
{
	//初始化各成员变量。
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

DWORD WINAPI NetListeningRun::sendThread(void* param)//发送线程入口函数。
{
	//std::cout << "发送数据线程开始运行！！" << std::endl;
	NetListeningRun* pClient = static_cast<NetListeningRun*>(param);//获得NetListeningRun对象指针。以便操纵成员变量。
	WaitForSingleObject(pClient->m_hEvent, INFINITE);//等待接收数据线程通知。
	while (pClient->m_IsConnected)
	{
		while (pClient->m_IsSendData)//可以发送数据。
		{
			//std::cout << "等待接收数据线程通知！！" << std::endl;
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
				//std::cout << "结果发送成功！！" << std::endl;
				pClient->m_IsSendData = false;
				break;
			}

		}
		Sleep(1000);//未收到发送通知，睡眠1秒。
	}
	delete pClient;
	return 0;
}

DWORD WINAPI NetListeningRun::recvThread(void* param)//接收数据线程入口函数。
{
	//std::cout << "接收数据线程开始运行！！" << std::endl;
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
				cout << "接收数据线程出现错误,连接中断！" << endl;
				break;
			}
			else if (r == WSAECONNRESET || r == WSAECONNABORTED)
			{
				//std::cout << "客户端强制中断" << std::endl;
				break;
			}
			else
			{
				//std::cout << "接收数据线程出现错误！error code:" <<r<< std::endl;
				break;
			}
		}
		else if (ret == 0)
		{
			//std::cout << "用户离开，关闭连接！" << std::endl;
			pClient->DisConnect();
			break;
		}
		else
		{
			//接收到请求数据，进行相应处理
			string temp = pClient->m_pRecvData;
			char request = temp[0];
			string peerIP = std::string(inet_ntoa(pClient->m_addr.sin_addr));//客户端IP
			unsigned short peerPort = ntohs(pClient->m_addr.sin_port);//客户端Port
			switch (request)
			{
				case NODE_JOIN:
				{
					cout << "接收到节点加入网络的请求:" + peerIP + "\n";
					temp = "this";
					temp[0] = REPLY_JOIN;
					pClient->set_senddata(temp.c_str());
					cout << "已响应节点加入网络的请求:" + peerIP+"\n";
					break;
				}
				case NODE_IN:
				{
					cout << "发现节点加入网络"+peerIP+"\n";
					temp = temp.substr(1);
					string IP_temp = nodeIP(temp);
					//判断是否是新节点
					if (!isNewNode(IP_temp))
					{
						cout << "节点" << peerIP << "已存在，无需更新\n";
					}
					else
					{
						std::ofstream fout;
						fout.open(NODELIST_PATH, ios::app);//追加方式打开
						if (!fout)
						{
							printf("节点列表打开失败，更新新加入的节点失败！\n");
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
							unsigned dis = getIPCount((char*)LocalIP.c_str(), (char*)IP_temp.c_str());//计算新加入的节点和自己的距离
							fout << std::left << setw(20) << IP_temp << setw(8) << Port_temp << dis << endl;
							public_keys.push_back(key_i);
							cout<<"已更新新加入节点"+peerIP+"\n";
							fout.close();

							Create_matrix();
						}
					}
					break;
				}
				case NODE_HEARTGET:
				{
					cout << "接收到来自节点的心跳检测包:" +peerIP +"\n";
					temp[0] = REPLY_HEART;
					pClient->set_senddata(temp.c_str());
					cout << "已响应来自节点的心跳检测包:" +peerIP + "\n";
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
					cout << "接收到来自节点的列表获取请求:" +peerIP + "\n";
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
					cout << "已响应来自节点的列表获取请求:" + peerIP + "\n";
					break;
				}
				default:
				{
					cout << "接收到来自节点无法识别的请求:" +peerIP + "\n";
					break;
				}
			}
			//处理完毕
			SetEvent(pClient->m_hEvent);
			pClient->m_IsSendData = true;
		}
	}
	//终止发送线程，否则在deletepClient后发送线程会产生异常
	ExitThread((DWORD)pClient->sendThread);
	delete pClient;
	return 0;
}

bool NetListeningRun::startRunning()
{
	m_hRecvThread = CreateThread(NULL, 0, recvThread, (void*)this, 0, NULL);//由于static成员函数，无法访问类成员。因此传入this指针。
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
