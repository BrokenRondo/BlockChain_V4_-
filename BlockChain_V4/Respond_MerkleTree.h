#pragma once
#include "osapi/osapi.h"
#include "MerkleTree/merkletree.h"
#include "jsoncpp/json.h"

class Respond_MerkleTree :public OS_Thread
{
	//����������ṩ��֤�Ĺ���
private:
	virtual int Routine() override;
	bool stopflag = true;

	Json::Value root;
	Json::Reader reader;
	Json::Value respond;
	Json::FastWriter fwriter;
	OS_UdpSocket sock;
	OS_SockAddr peer;
	OS_SockAddr local;
public:
	int recv_check();
};