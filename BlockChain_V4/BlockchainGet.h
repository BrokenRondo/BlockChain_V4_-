#pragma once
#ifndef BLOCKCHAINGET_H_
#define BLOCKCHAINGET_H_
#include "Global.h"
#include "DB_operate.h"
#include "osapi/osapi.h"
#include <fstream>
#include "BlockWriteDB.h"
using namespace std;
//客户端用于发送区块链同步请求并接收同步的区块链
class SynClient
{
	OS_TcpSocket Client;
	OS_SockAddr Serveraddr;
	char buf[BUF_SIZE];
	string CreationIP;
	string command;
public:
	SynClient(string creationIP,string command);
	int startsyn();
};
#endif // !BLOCKCHAINGET_H_
