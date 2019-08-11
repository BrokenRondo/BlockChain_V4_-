
#include "Broadcast_thread.h"
#include "Sync_time.h"
#include "Global.h"
#include <fstream>
#include "jsoncpp/json.h"
#include <Windows.h>
#include "NodeAnalyse.h"


Sync_time::Sync_time()
{
	this->time_index = std::time(0);
	this->last_store_time = 0;
	this->next_store_time = time_index - 2 * 60;//延后2分钟打包

}
Sync_time::~Sync_time()
{

}
std::vector<std::string> Sync_time::read_setNodelist()
{
	copy("./NodeList.txt", "./NodeListTemp.txt");
	ifstream fin("./NodeListTemp.txt");
	string temp = "";
	std::vector<std::string>Nodes;
	while (getline(fin, temp))
	{
		if (temp == "")continue;
		if (nodeIP(temp) == LocalIP)continue;
		int locationIP = temp.find(":");
		string IP = temp.substr(0, locationIP);
		Nodes.push_back(IP);
	}
	fin.close();
	DeleteFile("./NodeListTemp.txt");//删除临时文件
	return Nodes;
}


int Sync_time::Routine()
{
	//首先设置IP地址等
	local.SetIp(LocalIP.c_str());
	local.SetPortA(20050);//应该随便设置哪个端口都可以
	socket.Open(local, true);
	//一段时间后开始通知时间点M和N
	OS_Thread::Sleep(30);
	NodeNumber = Nodes_vector.size();
	NodeNumber -= 1;//去掉主节点
	//要先打包好通知的json
	Json::Value root;
	Json::FastWriter fwriter;
	Json::Reader reader;
	std::string json_str;
	while (true)//这个循环用来控制第多少次共识
	{
		time_index = std::time(0);
		next_store_time = time_index - 10 * 60;
		root["type"] = "MN";
		root["TIME_M"] = last_store_time;
		root["TIME_N"] = next_store_time;
		json_str = fwriter.write(root);
		//准备广播到所有节点
		Broadcast_thread* broadcast_thread = new  Broadcast_thread(json_str, 20000);
		broadcast_thread->Run();
		//通知完所有结点MN后要准备通知开始第几轮共识
		OS_Thread::Sleep(1);//等待五秒，让所有节点都计算完自己的默克尔树
		//然后开始通知开始共识
		int round_time = NodeNumber / 3 + 1;//重复的次数要为f+1次
		cout << "需要重复的次数" << round_time << endl;
		int this_time = 0;//表示到了第几轮共识(共f+1次）
		//从文件中读取节点列表
		//Nodes = this->read_setNodelist();
		int node_i = 1;//用来标记通知第几个节点作为主节点,跳过自己
		while (this_time < round_time)
		{
			root.clear();
			root["type"] = "ROUND";
			root["State"] = "START";
			json_str = fwriter.write(root);
			cout << this_time << endl;
			Broadcast_thread* broadcast_thread_round = new Broadcast_thread(json_str, 20000);
			broadcast_thread_round->Run();
			//每一次广播后进行共识算法的节点需要经过三轮阶段，所以等一段时间
			OS_Thread::Msleep(1500);
			delete broadcast_thread_round;
			//然后控制Propose 的时间点
			root.clear();
			root["type"] = "PROPOSE";
			json_str = fwriter.write(root);
			Broadcast_thread* broadcast_thread_propose = new Broadcast_thread(json_str, 20000);
			broadcast_thread_propose->Run();
			//接下来要通知主节点是谁，本来这个工作应该由各个节点自己分布式完成，但是目前暂时这么简化///////////////////////////////////
			//然后随机选取f+1个主节点，不过目前先直接选取前f+1个作为主节点
			std::string thistime_mainNode = Nodes_vector[node_i];
			OS_Thread::Msleep(1200);
			root.clear();
			root["type"] = "MAINNODE";
			root["MAINIP"] = thistime_mainNode;
			json_str = fwriter.write(root);
			//然后广播给所有节点
			Broadcast_thread* broadcast_thread_mainnode = new Broadcast_thread(json_str, 20000);
			cout << "广播" << json_str << endl;
			broadcast_thread_mainnode->Run();
			//然后休眠一段时间，等节点完成主节点的共识
			OS_Thread::Sleep(2);
			//round_time++;
			node_i++;
			this_time++;
			node_i = node_i % (NodeNumber - 1);
			delete broadcast_thread_mainnode;
			delete broadcast_thread_propose;
		}
		//如果已经到结束的时候了则通知结束
		root.clear();
		root["type"] = "ROUND";
		root["State"] = "STOP";
		json_str = fwriter.write(root);
		delete broadcast_thread;
		Broadcast_thread* broadcast_thread_round = new Broadcast_thread(json_str, 20000);
		broadcast_thread_round->Run();
		//每一次广播后进行共识算法的节点需要经过三轮阶段，所以等一段时间
		cout << "STOP" << endl;
		//设置下一次要共识的病历的时间
		// 将上次时间设置为这次结束时间
		last_store_time = next_store_time;
		OS_Thread::Sleep(5 * 60);
		delete broadcast_thread_round;
		this_time++;
	}//这个循环用来控制第多少次共识



}
