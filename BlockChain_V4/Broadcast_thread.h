#pragma once
#include "osapi\osapi.h"
#include<string>

class Broadcast_thread :public OS_Thread
{
private:
	virtual int Routine() override;
	char* message;
	int port;
	int msleep = 0;
public:
	Broadcast_thread(std::string message);
	Broadcast_thread(std::string message, int port, int msleep);
	Broadcast_thread(std::string message, int port);
	~Broadcast_thread() {};
	int broadcast(char* message);
	void setMessage(char* message);
	void setMsleep(int Msleep);
	void setPort(int port);
};