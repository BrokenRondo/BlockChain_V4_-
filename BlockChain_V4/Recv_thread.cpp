#include "Recv_thread.h"
#include "Global.h"
#include "jsoncpp/json.h"
#include <math.h>

int Recv_thread::Routine()
{
	//线程开启后打开socket监听发送给自己的数据，解析出来后去掉不符合的数据
	OS_SockAddr local, peer;
	char buf[2048] = { 0 };
	local.SetIp(localIP.c_str());
	local.SetPortA(20011);//接收共识算法传输数据的端口为20011

	socket_.SetOpt_RecvTimeout(2800);
	socket_.Open(local, true);
	while (!stopflag)
	{
		cout << "接收线程循环" << endl;
		memset(buf, 0, 2048);
		int n = socket_.RecvFrom(buf, 2048, peer);
		if (n < 0)
		{
			cout << "接收到错误的数据或超时，接收线程(20011)" << endl;
			continue;
		}
		cout << unitbuf;
		cout << buf << "来源：" << peer.GetIp_str() << endl;;
		Json::Value root;
		Json::Reader reader;
		if (!reader.parse(buf, root))
		{
			cout << "Json解析失败，来源：" << peer.GetIp_str() << ",接收线程（20011）" << endl;
			continue;
		}
		//正确解析json后先判断类型后取得json中的Merkle根，然后放入vector中。
		if (!(root["type"].asString() == "MERKLEROOT"))
		{
			cout << "类型不是Merkle根，来源：" << peer.GetIp_str() << ",接收线程(20011)" << endl;
		}
		std::string MerkleRoot = root["VALUE"].asString();
		auto *merkle_root_ip = new MerkleRoot_IP();
		merkle_root_ip->MerkleRoot = MerkleRoot;
		merkle_root_ip->src_ip = peer.GetIp_str();
		this->MerkleRoots.push_back(merkle_root_ip);//存入一个Merkle根：IP的结构体到vector

		//delete merkle_root_ip;
	}
	socket_.Close();
	return 0;

}
void Recv_thread::stop()
{
	this->socket_.Close();
	this->stopflag = true;
}
std::string Recv_thread::calculate_max_value()
{
	//首先把第一个放进vector，然后看第二个和第一个是否重复，如果不重复push_back
	if (MerkleRoots.empty())
	{
		return "0";//如果是空的，说明没人提议，直接返回
	}
	std::vector< MerkleRoot_IP*>::iterator iter = MerkleRoots.begin();
	calculate_max t{ (*iter)->MerkleRoot,0 };//初始化第一个
	calculate_max_vector.push_back(t);
	for (iter = MerkleRoots.begin(); iter != MerkleRoots.end(); iter++)
	{
		//找到某一个时看vector中有没有这个玩意，然后决定++还是pushback
		std::string MerkleRoot = (*iter)->MerkleRoot;
		int i = 0;
		bool findflag = false;//是否找到的标志位
		for (i = 0; i < calculate_max_vector.size(); i++)
		{
			if (calculate_max_vector[i].MerkleRoot == MerkleRoot)
			{
				//说明这个值已经记录过了
				calculate_max_vector[i].num++;
				findflag = true;
			}
		}
		int propose_z_times = 0;
		if (!findflag)
		{
			calculate_max calculate_max_{ MerkleRoot,1 };
			calculate_max_vector.push_back(calculate_max_);//pushback到底是放了地址进去还是拷贝进去了呢

		}
	}
	std::string max_str;//重复最多的hash是多少
	int max_num = 0;//重复最多的多少次
	for (int i = 0; i < calculate_max_vector.size(); i++)
	{

		if (max_num < calculate_max_vector[i].num)
		{
			max_num = calculate_max_vector[i].num;
			max_str = calculate_max_vector[i].MerkleRoot;
		}
	}
	int f = ((NodeNumber - 1) / 3);
	if (max_num >= NodeNumber - f)//至少n-f
	{
		return max_str;
	}
	else
	{
		//如果不满足n-f返回false
		return to_string(max_num);
	}
}

std::string Recv_thread::calculate_maxvalue_proposez()
{
	//首先把第一个放进vector，然后看第二个和第一个是否重复，如果不重复push_back
	if (MerkleRoots.empty())
	{
		return "0";//如果是空的，说明没人提议，直接返回
	}
	std::vector< MerkleRoot_IP*>::iterator iter = MerkleRoots.begin();
	calculate_max t{ (*iter)->MerkleRoot,0 };//初始化第一个
	calculate_max_vector.push_back(t);
	for (iter = MerkleRoots.begin(); iter != MerkleRoots.end(); iter++)
	{
		//找到某一个时看vector中有没有这个玩意，然后决定++还是pushback
		std::string MerkleRoot = (*iter)->MerkleRoot;
		int i = 0;
		bool findflag = false;//是否找到的标志位
		for (i = 0; i < calculate_max_vector.size(); i++)
		{
			if (calculate_max_vector[i].MerkleRoot == MerkleRoot)
			{
				//说明这个值已经记录过了
				calculate_max_vector[i].num++;
				findflag = true;
			}
		}
		int propose_z_times = 0;
		if (!findflag)
		{
			calculate_max calculate_max_{ MerkleRoot,1 };
			calculate_max_vector.push_back(calculate_max_);//pushback到底是放了地址进去还是拷贝进去了呢

		}
	}
	std::string max_str;//重复最多的hash是多少
	int max_num = 0;//重复最多的多少次
	for (int i = 0; i < calculate_max_vector.size(); i++)
	{

		if (max_num < calculate_max_vector[i].num)
		{
			max_num = calculate_max_vector[i].num;
			max_str = calculate_max_vector[i].MerkleRoot;
		}
	}
	int f = ((NodeNumber - 1) / 3);
	this->propose_z_times = max_num;
	if (max_num >= f)//至少f次
	{
		return max_str;
	}
	else
	{
		//如果不满足f返回false
		return to_string(max_num);
	}
}
int Recv_thread::get_propose_z_times()
{
	return this->propose_z_times;
}
