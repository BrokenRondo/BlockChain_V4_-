
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
	this->next_store_time = time_index - 2 * 60;//�Ӻ�2���Ӵ��

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
	DeleteFile("./NodeListTemp.txt");//ɾ����ʱ�ļ�
	return Nodes;
}


int Sync_time::Routine()
{
	//��������IP��ַ��
	local.SetIp(LocalIP.c_str());
	local.SetPortA(20050);//Ӧ����������ĸ��˿ڶ�����
	socket.Open(local, true);
	//һ��ʱ���ʼ֪ͨʱ���M��N
	OS_Thread::Sleep(30);
	NodeNumber = Nodes_vector.size();
	NodeNumber -= 1;//ȥ�����ڵ�
	//Ҫ�ȴ����֪ͨ��json
	Json::Value root;
	Json::FastWriter fwriter;
	Json::Reader reader;
	std::string json_str;
	while (true)//���ѭ���������Ƶڶ��ٴι�ʶ
	{
		time_index = std::time(0);
		next_store_time = time_index - 10 * 60;
		root["type"] = "MN";
		root["TIME_M"] = last_store_time;
		root["TIME_N"] = next_store_time;
		json_str = fwriter.write(root);
		//׼���㲥�����нڵ�
		Broadcast_thread* broadcast_thread = new  Broadcast_thread(json_str, 20000);
		broadcast_thread->Run();
		//֪ͨ�����н��MN��Ҫ׼��֪ͨ��ʼ�ڼ��ֹ�ʶ
		OS_Thread::Sleep(1);//�ȴ����룬�����нڵ㶼�������Լ���Ĭ�˶���
		//Ȼ��ʼ֪ͨ��ʼ��ʶ
		int round_time = NodeNumber / 3 + 1;//�ظ��Ĵ���ҪΪf+1��
		cout << "��Ҫ�ظ��Ĵ���" << round_time << endl;
		int this_time = 0;//��ʾ���˵ڼ��ֹ�ʶ(��f+1�Σ�
		//���ļ��ж�ȡ�ڵ��б�
		//Nodes = this->read_setNodelist();
		int node_i = 1;//�������֪ͨ�ڼ����ڵ���Ϊ���ڵ�,�����Լ�
		while (this_time < round_time)
		{
			root.clear();
			root["type"] = "ROUND";
			root["State"] = "START";
			json_str = fwriter.write(root);
			cout << this_time << endl;
			Broadcast_thread* broadcast_thread_round = new Broadcast_thread(json_str, 20000);
			broadcast_thread_round->Run();
			//ÿһ�ι㲥����й�ʶ�㷨�Ľڵ���Ҫ�������ֽ׶Σ����Ե�һ��ʱ��
			OS_Thread::Msleep(1500);
			delete broadcast_thread_round;
			//Ȼ�����Propose ��ʱ���
			root.clear();
			root["type"] = "PROPOSE";
			json_str = fwriter.write(root);
			Broadcast_thread* broadcast_thread_propose = new Broadcast_thread(json_str, 20000);
			broadcast_thread_propose->Run();
			//������Ҫ֪ͨ���ڵ���˭�������������Ӧ���ɸ����ڵ��Լ��ֲ�ʽ��ɣ�����Ŀǰ��ʱ��ô��///////////////////////////////////
			//Ȼ�����ѡȡf+1�����ڵ㣬����Ŀǰ��ֱ��ѡȡǰf+1����Ϊ���ڵ�
			std::string thistime_mainNode = Nodes_vector[node_i];
			OS_Thread::Msleep(1200);
			root.clear();
			root["type"] = "MAINNODE";
			root["MAINIP"] = thistime_mainNode;
			json_str = fwriter.write(root);
			//Ȼ��㲥�����нڵ�
			Broadcast_thread* broadcast_thread_mainnode = new Broadcast_thread(json_str, 20000);
			cout << "�㲥" << json_str << endl;
			broadcast_thread_mainnode->Run();
			//Ȼ������һ��ʱ�䣬�Ƚڵ�������ڵ�Ĺ�ʶ
			OS_Thread::Sleep(2);
			//round_time++;
			node_i++;
			this_time++;
			node_i = node_i % (NodeNumber - 1);
			delete broadcast_thread_mainnode;
			delete broadcast_thread_propose;
		}
		//����Ѿ���������ʱ������֪ͨ����
		root.clear();
		root["type"] = "ROUND";
		root["State"] = "STOP";
		json_str = fwriter.write(root);
		delete broadcast_thread;
		Broadcast_thread* broadcast_thread_round = new Broadcast_thread(json_str, 20000);
		broadcast_thread_round->Run();
		//ÿһ�ι㲥����й�ʶ�㷨�Ľڵ���Ҫ�������ֽ׶Σ����Ե�һ��ʱ��
		cout << "STOP" << endl;
		//������һ��Ҫ��ʶ�Ĳ�����ʱ��
		// ���ϴ�ʱ������Ϊ��ν���ʱ��
		last_store_time = next_store_time;
		OS_Thread::Sleep(5 * 60);
		delete broadcast_thread_round;
		this_time++;
	}//���ѭ���������Ƶڶ��ٴι�ʶ



}
