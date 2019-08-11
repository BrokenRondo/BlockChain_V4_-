#include "Broadcast_thread.h"
#include "Global.h"
#include<string>
#include<iostream>
#include<fstream>
using namespace std;

extern string nodeIP(string Nodestr);
Broadcast_thread::Broadcast_thread(std::string message)
{
	//����Ҫ��message���ڴ濽����char*����
	this->port = 20010;
	this->message = new char[2048];
	memset(this->message, 0, 2048);
	memcpy(this->message, message.c_str(), message.length());
}
Broadcast_thread::Broadcast_thread(std::string message, int port, int msleep)
{
	//����Ҫ��message���ڴ濽����char*����
	this->msleep = msleep;
	this->message = new char[2048];
	memset(this->message, 0, 2048);
	memcpy(this->message, message.c_str(), message.length());
	this->port = port;
}
Broadcast_thread::Broadcast_thread(std::string message, int port)
{
	//����Ҫ��message���ڴ濽����char*����
	this->message = new char[2048];
	memset(this->message, 0, 2048);
	memcpy(this->message, message.c_str(), message.length());
	this->port = port;
}
int Broadcast_thread::Routine()
{
	//����߳������㲥�Լ���Ĭ�˶�����
	broadcast(this->message);
	return 0;
}
int Broadcast_thread::broadcast(char* message)
{
	OS_Thread::Msleep(msleep);
	OS_UdpSocket sock;
	OS_SockAddr local(localIP.c_str(), port);
	OS_SockAddr peer;
	//ifstream fin("./NodeList.txt");
	//string temp = "";
	sock.Open(local, true);
	for(int i=0;i<Nodes_vector.size();i++)
	{
		//if (Nodes_vector[i]==localIP)continue;
		//if (nodeIP(temp) == localIP)continue;
		//int locationIP = temp.find(":");
		//string IP = temp.substr(0, locationIP);
		peer.SetIp(Nodes_vector[i].c_str());
		peer.SetPortA(port);
		cout << "�㲥��" << Nodes_vector[i] << endl;
		sock.SendTo(message, 2048, peer);
	}
	sock.Close();
	return 0;
}
void Broadcast_thread::setMessage(char* message)
{
	memset(this->message, 0, 2048);
	memcpy(this->message, message, strlen(message));
}
void Broadcast_thread::setMsleep(int Msleep)
{
	this->msleep = Msleep;
}
void Broadcast_thread::setPort(int port)
{
	this->port = port;
}

