#include "Respond_MerkleTree.h"

extern std::string localIP;
extern merkletree* merkletree_global;

int Respond_MerkleTree::recv_check()
{

	//���յ���Ϣ��ʱ�����Ҫ��ʲô
	char buf_waiting[4096] = { 0 };

	local.SetIp(localIP.c_str());
	local.SetPortA(20080);
	sock.Open(local, true);


	//��������˵��socket��Ҫ���ó�ʱʱ�䣬������ʱû����
	while (true)
	{
		memset(buf_waiting, 0, 4096);
		int n = sock.RecvFrom(buf_waiting, 4096, peer);
		if (n < 0) {
			std::cout << "socket���ִ���,��֤��Ӧ�̣߳�20040��" << endl;
			continue;
		}


		//����ͬ��ʱ�ӷ���������
		if (!reader.parse(buf_waiting, root))
		{
			std::cout << "json����������֤��Ӧ�̣߳�20080��" << endl;
			continue;
		}
		const std::string type = root["type"].asString();//��ȡ���json�����ͣ�����Ӧ����MN����ʾ֪ͨM��N��ʱ���
		if (type != "PROOF")
		{
			std::cout << "��֤�����쳣������Ϊ" << type << ",��֤��Ӧ�̣߳�20080��" << endl;
			continue;
		}
		else
		{
			std::cout << "���յ����ڵ��ͬ����Ϣ����Ϣ����Ϊ" << type << endl;
		}
		break;
	}
	return 0;
}
int Respond_MerkleTree::Routine()
{
	std::cout << "��֤�߳̿���" << endl;
	while (stopflag)
	{

		//�����յ��İ�������ǲ��Ϸ��İ�����������ǺϷ��İ�����֤
		recv_check();
		//������json����֤�����ؽ��
		int i = root["leaves"].size();
		//������typeΪcommit������в���commit�������
		respond["type"] = "COMMIT";
		for (int j = 0; j < i; j++)
		{
			//��ÿһ��Ҷ�ӣ���֤�Ƿ���Ĭ�˶����У�������ھ���ӵ�������Json
			//�Ȱ�proofҪ�õĽڵ������
			std::string leaf = root["leaves"][j].asString();
			// get proof
			vector<ProofNode> proof = merkletree_global->proof(const_cast<char*>(leaf.c_str()));

			// verify proof
			bool verified = verifyProof(const_cast<char*>(leaf.c_str()), merkletree_global->root(), proof);
			cout << "Ҷ��" << leaf << "��֤״̬Ϊ��" << verified << endl;
			if (!verified)
			{
				//�����֤ʧ�������Ҷ�ӷŽ�NOCOMMIT��
				Json::Value leaves;
				leaves = leaf;
				respond["NOTCOMMIT_LEAVES"].append(leaves);
				respond["type"] = "NOTCOMMIT";
			}
		}
		std::string respond_string = fwriter.write(respond);
		OS_UdpSocket sock;
		OS_SockAddr local(localIP.c_str(), 20041);
		sock.Open(local, true);
		sock.SendTo(respond_string.c_str(), 2048, peer);
		cout << "������֤�����" << endl;
		sock.Close();
	}
	return 0;
}
