#pragma once
#include "osapi/osapi.h"
#include <vector>
#include <string>
#include "ctime"

using namespace std;
extern void copy(string src, string dst);
//这个类供作为同步时间节点的节点使用
class Sync_time :public OS_Thread
{
private:
	virtual int Routine() override;
	std::vector<std::string> commit_nodes;//需要一个数组来存放回应一轮结束的节点
	int last_store_time;//上一次存储到哪个时间点了，也就是共识算法中的TIME_M
	int next_store_time;//下一次存储到哪个时间点，也就是共识算法中的TIME_N
	time_t time_index;//一轮共识启动的时间
	time_t time_index_roundEnd;//一轮共识结束的时间
	OS_UdpSocket socket;
	OS_SockAddr local;
	OS_SockAddr peer;
	std::vector<std::string >Nodes;
public:
	Sync_time();
	~Sync_time();
	std::vector<std::string > read_setNodelist();//用来从文件中读取节点表并存储到vector中


};