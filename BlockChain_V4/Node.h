#pragma once
#ifndef NODE_H_
#define NODE_H_
//客户端，作为网络节点连接创世节点的服务器
#include <winsock2.h>  
#include<cstdint>
#include "NetListening.h"
#include "NodeHeartGet.h"
#include "Global.h"
#pragma comment(lib, "ws2_32.lib")
using namespace std;
extern KeyPairHex keys;
class P2PNodes
{
public:
	string IP = "";
	unsigned short port = 10010;
	//由于演示环境在本机或者局域网，网络距离采取对IP地址进行运算的方法
	unsigned int netDistance = 0;
};
//节点
class Node
{
	OS_TcpSocket Client;
	OS_SockAddr Serveraddr;
	char buf[BUF_SIZE];
	string CreationIP;
public:
	Node(string creationIP);
	//初始化网络，获取本地IP等
	int setNetEnvironment();
	//加入P2P网络
	int LoginNet();
	//向列表中节点发送自身数据以宣告自身加入网络
	int SendToList();
	//启动网络监听线程
	int netlistening_start(NetListening& netlisteningThread);
	//启动心跳包索取线程
	int nodeheartget_start(NodeHeartGet& update);
	//用于在刚加入网络时从创世节点处获取节点表
	int getP2PNodes();
};
#endif // !NODE_H_
