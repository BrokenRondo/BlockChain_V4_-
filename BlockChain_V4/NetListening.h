#pragma once
#ifndef NETLISTENING_H_
#define NETLISTENING_H_
//�����������ڼ��������ڵ㷢�͸���������󲢽�����Ӧ����
#include <fstream>
#include <iostream>
#include <iomanip>
#include "quorum.h"
#include "osapi/osapi.h"
#include "NodeAnalyse.h"
#include "Global.h"
using namespace std;
//����������
class NetListening :public OS_Thread
{
private:
	std::string Ip;
	int Port;
	virtual int Routine() override;
public:
	NetListening(std::string Ip, int Port) { this->Ip = Ip; this->Port = Port; }
};
//��������֧
class NetListeningRun
{
public:
	NetListeningRun(SOCKET s, sockaddr_in addr);
	~NetListeningRun(void);
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
#endif // !NETLISTENING_H_

