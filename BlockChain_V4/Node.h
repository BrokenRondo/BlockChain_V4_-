#pragma once
#ifndef NODE_H_
#define NODE_H_
//�ͻ��ˣ���Ϊ����ڵ����Ӵ����ڵ�ķ�����
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
	//������ʾ�����ڱ������߾���������������ȡ��IP��ַ��������ķ���
	unsigned int netDistance = 0;
};
//�ڵ�
class Node
{
	OS_TcpSocket Client;
	OS_SockAddr Serveraddr;
	char buf[BUF_SIZE];
	string CreationIP;
public:
	Node(string creationIP);
	//��ʼ�����磬��ȡ����IP��
	int setNetEnvironment();
	//����P2P����
	int LoginNet();
	//���б��нڵ㷢���������������������������
	int SendToList();
	//������������߳�
	int netlistening_start(NetListening& netlisteningThread);
	//������������ȡ�߳�
	int nodeheartget_start(NodeHeartGet& update);
	//�����ڸռ�������ʱ�Ӵ����ڵ㴦��ȡ�ڵ��
	int getP2PNodes();
};
#endif // !NODE_H_
