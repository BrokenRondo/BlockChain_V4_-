#pragma once
#ifndef QUORUMSERVER_H_
#define QUORUMSERVER_H_
#include "osapi/osapi.h"
//�����������ڴ������ٲ÷�����ص����񣬰������ո߶Ⱥͷ��͸߶ȼ���
#include "Global.h"
#include <fstream>
using namespace std;
//����������
class QuorumServer :public OS_Thread
{
private:
	std::string Ip;
	int Port;
	virtual int Routine() override;
public:
	QuorumServer(std::string Ip, int Port) { this->Ip = Ip; this->Port = Port; }
};
//��������֧
class QuorumServerRun
{
public:
	QuorumServerRun(SOCKET s, sockaddr_in addr);
	~QuorumServerRun(void);
public:
	bool IsConnected();//�ж������Ƿ��жϡ�
	bool DisConnect();//�ж�������������ӡ�
	bool set_senddata(std::string data);//���÷������ݣ�������д
	bool startRunning();//��ʼ���з��ͺͽ����̡߳�
	static DWORD WINAPI sendThread(void* param);//�����߳���ں�����
	static DWORD WINAPI recvThread(void* param);//�����߳���ں�����
	char* m_pSendData;//���ͻ�������
	char* m_pRecvData;//���ջ�������
private:
	HANDLE m_hSendThread;//�����߳̾����
	HANDLE m_hRecvThread;//�����߳̾����
	HANDLE m_hEvent;//�����̺߳ͽ����߳�ͬ���¼����󡣽��տͻ��������֪ͨ�����̷߳��͵�ǰʱ�䡣
	SOCKET m_socket;//��ͻ��������׽��֡�
	sockaddr_in m_addr;//�ͻ��˵�ַ��
	bool m_IsConnected;
	bool m_IsSendData;//����ֻ�н��յ��ͻ�����������Ҫ���ͣ��ñ��������Ƿ������ݡ�
};
#endif // !QUORUMSERVER_H_
