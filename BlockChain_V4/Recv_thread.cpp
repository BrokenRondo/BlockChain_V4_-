#include "Recv_thread.h"
#include "Global.h"
#include "jsoncpp/json.h"
#include <math.h>

int Recv_thread::Routine()
{
	//�߳̿������socket�������͸��Լ������ݣ�����������ȥ�������ϵ�����
	OS_SockAddr local, peer;
	char buf[2048] = { 0 };
	local.SetIp(localIP.c_str());
	local.SetPortA(20011);//���չ�ʶ�㷨�������ݵĶ˿�Ϊ20011

	socket_.SetOpt_RecvTimeout(2800);
	socket_.Open(local, true);
	while (!stopflag)
	{
		cout << "�����߳�ѭ��" << endl;
		memset(buf, 0, 2048);
		int n = socket_.RecvFrom(buf, 2048, peer);
		if (n < 0)
		{
			cout << "���յ���������ݻ�ʱ�������߳�(20011)" << endl;
			continue;
		}
		cout << unitbuf;
		cout << buf << "��Դ��" << peer.GetIp_str() << endl;;
		Json::Value root;
		Json::Reader reader;
		if (!reader.parse(buf, root))
		{
			cout << "Json����ʧ�ܣ���Դ��" << peer.GetIp_str() << ",�����̣߳�20011��" << endl;
			continue;
		}
		//��ȷ����json�����ж����ͺ�ȡ��json�е�Merkle����Ȼ�����vector�С�
		if (!(root["type"].asString() == "MERKLEROOT"))
		{
			cout << "���Ͳ���Merkle������Դ��" << peer.GetIp_str() << ",�����߳�(20011)" << endl;
		}
		std::string MerkleRoot = root["VALUE"].asString();
		auto *merkle_root_ip = new MerkleRoot_IP();
		merkle_root_ip->MerkleRoot = MerkleRoot;
		merkle_root_ip->src_ip = peer.GetIp_str();
		this->MerkleRoots.push_back(merkle_root_ip);//����һ��Merkle����IP�Ľṹ�嵽vector

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
	//���Ȱѵ�һ���Ž�vector��Ȼ�󿴵ڶ����͵�һ���Ƿ��ظ���������ظ�push_back
	if (MerkleRoots.empty())
	{
		return "0";//����ǿյģ�˵��û�����飬ֱ�ӷ���
	}
	std::vector< MerkleRoot_IP*>::iterator iter = MerkleRoots.begin();
	calculate_max t{ (*iter)->MerkleRoot,0 };//��ʼ����һ��
	calculate_max_vector.push_back(t);
	for (iter = MerkleRoots.begin(); iter != MerkleRoots.end(); iter++)
	{
		//�ҵ�ĳһ��ʱ��vector����û��������⣬Ȼ�����++����pushback
		std::string MerkleRoot = (*iter)->MerkleRoot;
		int i = 0;
		bool findflag = false;//�Ƿ��ҵ��ı�־λ
		for (i = 0; i < calculate_max_vector.size(); i++)
		{
			if (calculate_max_vector[i].MerkleRoot == MerkleRoot)
			{
				//˵�����ֵ�Ѿ���¼����
				calculate_max_vector[i].num++;
				findflag = true;
			}
		}
		int propose_z_times = 0;
		if (!findflag)
		{
			calculate_max calculate_max_{ MerkleRoot,1 };
			calculate_max_vector.push_back(calculate_max_);//pushback�����Ƿ��˵�ַ��ȥ���ǿ�����ȥ����

		}
	}
	std::string max_str;//�ظ�����hash�Ƕ���
	int max_num = 0;//�ظ����Ķ��ٴ�
	for (int i = 0; i < calculate_max_vector.size(); i++)
	{

		if (max_num < calculate_max_vector[i].num)
		{
			max_num = calculate_max_vector[i].num;
			max_str = calculate_max_vector[i].MerkleRoot;
		}
	}
	int f = ((NodeNumber - 1) / 3);
	if (max_num >= NodeNumber - f)//����n-f
	{
		return max_str;
	}
	else
	{
		//���������n-f����false
		return to_string(max_num);
	}
}

std::string Recv_thread::calculate_maxvalue_proposez()
{
	//���Ȱѵ�һ���Ž�vector��Ȼ�󿴵ڶ����͵�һ���Ƿ��ظ���������ظ�push_back
	if (MerkleRoots.empty())
	{
		return "0";//����ǿյģ�˵��û�����飬ֱ�ӷ���
	}
	std::vector< MerkleRoot_IP*>::iterator iter = MerkleRoots.begin();
	calculate_max t{ (*iter)->MerkleRoot,0 };//��ʼ����һ��
	calculate_max_vector.push_back(t);
	for (iter = MerkleRoots.begin(); iter != MerkleRoots.end(); iter++)
	{
		//�ҵ�ĳһ��ʱ��vector����û��������⣬Ȼ�����++����pushback
		std::string MerkleRoot = (*iter)->MerkleRoot;
		int i = 0;
		bool findflag = false;//�Ƿ��ҵ��ı�־λ
		for (i = 0; i < calculate_max_vector.size(); i++)
		{
			if (calculate_max_vector[i].MerkleRoot == MerkleRoot)
			{
				//˵�����ֵ�Ѿ���¼����
				calculate_max_vector[i].num++;
				findflag = true;
			}
		}
		int propose_z_times = 0;
		if (!findflag)
		{
			calculate_max calculate_max_{ MerkleRoot,1 };
			calculate_max_vector.push_back(calculate_max_);//pushback�����Ƿ��˵�ַ��ȥ���ǿ�����ȥ����

		}
	}
	std::string max_str;//�ظ�����hash�Ƕ���
	int max_num = 0;//�ظ����Ķ��ٴ�
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
	if (max_num >= f)//����f��
	{
		return max_str;
	}
	else
	{
		//���������f����false
		return to_string(max_num);
	}
}
int Recv_thread::get_propose_z_times()
{
	return this->propose_z_times;
}
