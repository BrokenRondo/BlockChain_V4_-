#include "BlockchainCheck.h"
int BlockChainCheck::Routine()
{
	std::cout << "����������߳�����\n";
	while (true)
	{
		OS_Thread::Sleep(30);
		//���ж���������û�б����Ĺ�
		cout << "��ʼ���������\n";
		int result = verifyBlockChain();
		//�����-1��˵������ȫ��ͬ��
		if (result == -1)
		{
			cout << "�����������۸ģ���ʼͬ����������\n";
			syncdata(0);
		}
		else if (result == 0)
		{
			cout << "����������Ϊ�գ���ʼͬ����������\n";
			syncdata(0);
		}
		else
		{
			//�������Ҫͬ���������ݣ���ֻ��Ҫ���β�����˵����ݾͿ���
			//��Ϊ������preHashָ��ǰһ�����飬�������û�б��۸ľͲ����ܳ����м�ֲ�����
			cout << "������δ���۸ģ���ͼѰ�Ҹ�����������\n";
			syncdata(CountBlock());
		}
	}
	return 0;
}
int BlockChainCheck::verifyBlockChain()
{
	//������Ϊ�գ�����0������������ȷ������-1����������ȷ������1
	const int blocknum = CountBlock();
	//����������ļ��ǿյģ�˵����û��ͬ�����ݣ�����0
	if (blocknum == 0)
	{
		return 0;
	}
	//i��1��ʼ��Ϊ��ʼ���鲢û��ǰ�����ϣֵ
	for (int i = 1; i < blocknum; i++)
	{
		std::string CalculateHash = sha256(ReadBlock("block" + to_string(i - 1)));
		std::string PreHash_str =PreHash("block" + to_string(i));
		if (CalculateHash != PreHash_str)
		{
			cout << "��⵽block"+to_string(i-1)+"����block"+to_string(i)+"֮���ϣֵ��һ��\n";
			return -1;
		}
	}
	//δ������
	return 1;
}

int BlockChainCheck::syncdata(int index)
{
	if (index == 0)
	{
		//ɾ��ԭ������������ͷ��ʼͬ��������
		DeleteBlock();
	}
	vector<string> Max_H_IP=Get_MaxHeight_IP();
	int Max_Block_length =atoi(Max_H_IP[0].c_str());
	vector<string>  Max_Block_length_IP;
	string temp = Max_H_IP[1];
	string IPtemp="";
	for (int i = 0; i < temp.size(); i++)
	{
		if (temp[i] == '~')
		{
			Max_Block_length_IP.push_back(IPtemp);
			IPtemp = "";
		}
		else
		{
			IPtemp += temp[i];
		}
	}
	//�����Ѿ�����˳�������ӵ����������Ľڵ��б�
	if (Max_Block_length <= CountBlock())
	{
		cout << "û�и�����������Ѱ�Ҹ���������������\n";
		return 0;
	}
	//���ڱ���������������
	int Random = rand() % Max_Block_length_IP.size();
	string synnodeIP = Max_Block_length_IP[Random];
	string command = "index";
	command += to_string(index);
	SynClient* synclient = new SynClient(synnodeIP,command);
	if (synclient->startsyn() == -1)
	{
		while (true)
		{
			cout << "�л�ͬ���ڵ�...\n";
			int connectTimes = 0;
			for (int i = 0; i < Max_Block_length_IP.size(); i++)
			{
				synnodeIP = Max_Block_length_IP[i];
				SynClient* other_synclient = new SynClient(synnodeIP, command);
				if (other_synclient->startsyn()!= -1)
				{
					delete other_synclient;
					break;
				}
				delete other_synclient;
				if (i + 1 == Max_Block_length_IP.size())
				{
					cout << "��ʱ�޷��л��ڵ㣬�Ժ���\n";
					return -2;
				}
			}
		}
	}
	delete synclient;
	Blocknum = CountBlock();
	return 0;
}
