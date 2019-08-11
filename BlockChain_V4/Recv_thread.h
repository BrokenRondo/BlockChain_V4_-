#pragma once
#include "osapi\osapi.h"
#include  <vector>
#include "Global.h"
class Recv_thread :public OS_Thread
{
private:
	virtual int Routine()override;
	bool stopflag = false;

	struct calculate_max
	{
		std::string MerkleRoot;
		unsigned int num = 1;//重复了多少次
	};
	std::vector<calculate_max>calculate_max_vector;
	int propose_z_times = 1;
	OS_UdpSocket socket_;
public:
	std::vector<MerkleRoot_IP*>MerkleRoots;//存储了Merkle根和IP的结构体数组
	void stop();
	std::string calculate_max_value();//计算出重复最多的hash值的string，如果重复最多的次数都不满足至少n-f，返回null
	std::string calculate_maxvalue_proposez();
	int get_propose_z_times();
};