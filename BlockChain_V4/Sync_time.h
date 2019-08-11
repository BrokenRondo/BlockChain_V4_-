#pragma once
#include "osapi/osapi.h"
#include <vector>
#include <string>
#include "ctime"

using namespace std;
extern void copy(string src, string dst);
//����๩��Ϊͬ��ʱ��ڵ�Ľڵ�ʹ��
class Sync_time :public OS_Thread
{
private:
	virtual int Routine() override;
	std::vector<std::string> commit_nodes;//��Ҫһ����������Ż�Ӧһ�ֽ����Ľڵ�
	int last_store_time;//��һ�δ洢���ĸ�ʱ����ˣ�Ҳ���ǹ�ʶ�㷨�е�TIME_M
	int next_store_time;//��һ�δ洢���ĸ�ʱ��㣬Ҳ���ǹ�ʶ�㷨�е�TIME_N
	time_t time_index;//һ�ֹ�ʶ������ʱ��
	time_t time_index_roundEnd;//һ�ֹ�ʶ������ʱ��
	OS_UdpSocket socket;
	OS_SockAddr local;
	OS_SockAddr peer;
	std::vector<std::string >Nodes;
public:
	Sync_time();
	~Sync_time();
	std::vector<std::string > read_setNodelist();//�������ļ��ж�ȡ�ڵ���洢��vector��


};