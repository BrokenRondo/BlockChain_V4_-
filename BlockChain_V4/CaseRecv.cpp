#include "CaseRecv.h"
int CaseRecv::Routine()
{
	OS_TcpSocket server_sock;
	OS_SockAddr sock_addr(Ip.c_str(), Port);
	server_sock.Open(sock_addr);
	if (server_sock.Ioctl_SetBlockedIo(false) < 0)
	{
		cout << "�˿�" << Port << "���÷�����ʧ��,���ܰ�IP���ʹ���\n";
	}
	server_sock.Listen();
	printf("���������̷߳������������˿�%d\n", Port);
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
			CaseRecvRun* pClient = new CaseRecvRun(s, addr);
			if (!pClient->startRunning())cout << "����������\n";//�����ӽ��ܺͷ����߳̿�ʼִ�С�
		}
	}
	server_sock.Close();
	return 0;
}

CaseRecvRun::CaseRecvRun(SOCKET s, sockaddr_in addr)
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

DWORD WINAPI CaseRecvRun::sendThread(void* param)//�����߳���ں�����
{
	//std::cout << "���������߳̿�ʼ���У���" << std::endl;
	CaseRecvRun* pClient = static_cast<CaseRecvRun*>(param);//���CaseRecvRun����ָ�롣�Ա���ݳ�Ա������
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

DWORD WINAPI CaseRecvRun::recvThread(void* param)//���������߳���ں�����
{
	//std::cout << "���������߳̿�ʼ���У���" << std::endl;
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
			CaseAnalysis* caseanalysis = new CaseAnalysis(temp);
			caseanalysis->Run();
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

bool CaseRecvRun::startRunning()
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

bool CaseRecvRun::DisConnect()
{
	m_IsConnected = false;
	return true;
}
