#include "CaseRecv.h"
int CaseRecv::Routine()
{
	OS_TcpSocket server_sock;
	OS_SockAddr sock_addr(Ip.c_str(), Port);
	server_sock.Open(sock_addr);
	if (server_sock.Ioctl_SetBlockedIo(false) < 0)
	{
		cout << "端口" << Port << "设置非阻塞失败,可能绑定IP发送错误\n";
	}
	server_sock.Listen();
	printf("病历接收线程服务器启动，端口%d\n", Port);
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
			CaseRecvRun* pClient = new CaseRecvRun(s, addr);
			if (!pClient->startRunning())cout << "服务发生错误\n";//该链接接受和发送线程开始执行。
		}
	}
	server_sock.Close();
	return 0;
}

CaseRecvRun::CaseRecvRun(SOCKET s, sockaddr_in addr)
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

CaseRecvRun::~CaseRecvRun(void)
{
	delete[]m_pRecvData;
	delete[]m_pSendData;
}

bool CaseRecvRun::IsConnected()
{
	return m_IsConnected;
}

bool CaseRecvRun::set_senddata(std::string data)
{
	strcpy(m_pSendData, data.c_str());
	return true;
}

DWORD WINAPI CaseRecvRun::sendThread(void* param)//发送线程入口函数。
{
	//std::cout << "发送数据线程开始运行！！" << std::endl;
	CaseRecvRun* pClient = static_cast<CaseRecvRun*>(param);//获得CaseRecvRun对象指针。以便操纵成员变量。
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

DWORD WINAPI CaseRecvRun::recvThread(void* param)//接收数据线程入口函数。
{
	//std::cout << "接收数据线程开始运行！！" << std::endl;
	CaseRecvRun* pClient = static_cast<CaseRecvRun*>(param);
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
			CaseAnalysis* caseanalysis = new CaseAnalysis(temp);
			caseanalysis->Run();
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

bool CaseRecvRun::startRunning()
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

bool CaseRecvRun::DisConnect()
{
	m_IsConnected = false;
	return true;
}
