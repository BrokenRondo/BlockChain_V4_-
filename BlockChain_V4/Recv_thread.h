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
		unsigned int num = 1;//�ظ��˶��ٴ�
	};
	std::vector<calculate_max>calculate_max_vector;
	int propose_z_times = 1;
	OS_UdpSocket socket_;
public:
	std::vector<MerkleRoot_IP*>MerkleRoots;//�洢��Merkle����IP�Ľṹ������
	void stop();
	std::string calculate_max_value();//������ظ�����hashֵ��string������ظ����Ĵ���������������n-f������null
	std::string calculate_maxvalue_proposez();
	int get_propose_z_times();
};