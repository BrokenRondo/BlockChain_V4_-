#include "BlockchainSyn.h"
int BlockchainSyn::Routine()
{
	OS_TcpSocket server_sock;
	OS_SockAddr sock_addr(Ip.c_str(), Port);
	server_sock.Open(sock_addr);
	if (server_sock.Ioctl_SetBlockedIo(false) < 0)
	{
		cout << "端口" << Port << "设置非阻塞失败,可能绑定IP发送错误\n";
	}
	server_sock.Listen();
	printf("同步请求监听服务器启动，端口%d\n", Port);
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
			BlockchainSynRun* pClient = new BlockchainSynRun(s, addr);
			if (!pClient->startRunning())cout << "服务发生错误\n";//该链接接受和发送线程开始执行。
		}
	}
	server_sock.Close();
	return 0;
}

BlockchainSynRun::BlockchainSynRun(SOCKET s, sockaddr_in addr)
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

DWORD WINAPI BlockchainSynRun::sendThread(void* param)//发送线程入口函数。
{
	//std::cout << "发送数据线程开始运行！！" << std::endl;
	BlockchainSynRun* pClient = static_cast<BlockchainSynRun*>(param);//获得BlockchainSynRun对象指针。以便操纵成员变量。
	WaitForSingleObject(pClient->m_hEvent, INFINITE);//等待接收数据线程通知。
	while (pClient->m_IsConnected)
	{
		while (pClient->m_IsSendData)//可以发送数据。
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

DWORD WINAPI BlockchainSynRun::recvThread(void* param)//接收数据线程入口函数。
{
	//std::cout << "接收数据线程开始运行！！" << std::endl;
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
			string result = pClient->m_pRecvData;
			//对方请求获取区块链长度
			if (result[0] == GET_LEN)
			{
				string length = to_string(CountBlock());
				pClient->set_senddata(length);
			}
			//对方请求同步区块链
			else
			{
				//比较result前5位是否为index。不是则关闭连接
				char indexstr[6] = { 0 };
				memcpy(indexstr, result.c_str(), 5);
				if (strcmp(indexstr, "index") != 0)
				{
					cout << "同步请求指令错误，非index指令\n";
					pClient->DisConnect();
					ExitThread((DWORD)pClient->sendThread);
					delete pClient;
					return -1;
				}
				//如果是index，则获取要同步的位置
				unsigned int location = atoi(result.substr(5).c_str());
				cout << "接收到区块链同步指令,同步起始区块序号"+to_string(location)+"\n";
				Blocknum = CountBlock();
				for (int i = location; i < Blocknum; i++)
				{
					//获取区块数据并分包
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
			//线程未释放
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

bool BlockchainSynRun::startRunning()
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
			//一个包装满了
			pchar[0] = CONTINUE_DATA;
			pchar[packsize+1] = '\0';
			result.push_back(pchar);
		}
		//填装数据
		pchar[j % packsize + 1] = blockdata[j];
		if (j == blockdata.size() - 1)
		{
			//包可能没装满但是被分割的数据已经装完了
			pchar[0] = LAST_DATA;
			pchar[j % packsize + 2] = '\0';
			result.push_back(pchar);
		}
	}
	return result;
}
