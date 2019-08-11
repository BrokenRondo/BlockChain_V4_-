#pragma once
#ifndef NETLISTENING_H_
#define NETLISTENING_H_
//服务器，用于监听其他节点发送给自身的请求并进行相应处理
#include <fstream>
#include <iostream>
#include <iomanip>
#include "quorum.h"
#include "osapi/osapi.h"
#include "NodeAnalyse.h"
#include "Global.h"
using namespace std;
//服务器主干
class NetListening :public OS_Thread
{
private:
	std::string Ip;
	int Port;
	virtual int Routine() override;
public:
	NetListening(std::string Ip, int Port) { this->Ip = Ip; this->Port = Port; }
};
//服务器分支
class NetListeningRun
{
public:
	NetListeningRun(SOCKET s, sockaddr_in addr);
	~NetListeningRun(void);
public:
	bool IsConnected();//判断连接是否中断。
	bool DisConnect();//中断与服务器的连接。
	bool set_senddata(std::string data);//设置发送数据，用于重写
	bool startRunning();//开始运行发送和接收线程。
	static DWORD WINAPI sendThread(void* param);//发送线程入口函数。
	static DWORD WINAPI recvThread(void* param);//接收线程入口函数。
	char* m_pSendData;//发送缓冲区。
	char* m_pRecvData;//接收缓冲区。
private:
	HANDLE m_hSendThread;//发送线程句柄。
	HANDLE m_hRecvThread;//接受线程句柄。
	HANDLE m_hEvent;//发送线程和接收线程同步事件对象。接收客户端请求后通知发送线程发送当前时间。
	SOCKET m_socket;//与客户端连接套接字。
	sockaddr_in m_addr;//客户端地址。
	bool m_IsConnected;
	bool m_IsSendData;//由于只有接收到客户端请求后才需要发送，该变量控制是否发送数据。
};
#endif // !NETLISTENING_H_

