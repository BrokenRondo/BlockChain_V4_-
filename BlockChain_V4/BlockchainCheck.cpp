#include "BlockchainCheck.h"
int BlockChainCheck::Routine()
{
	std::cout << "检查区块链线程启动\n";
	while (true)
	{
		OS_Thread::Sleep(30);
		//先判断区块链有没有被更改过
		cout << "开始检查区块链\n";
		int result = verifyBlockChain();
		//如果是-1就说明必须全部同步
		if (result == -1)
		{
			cout << "区块链发生篡改，开始同步所有数据\n";
			syncdata(0);
		}
		else if (result == 0)
		{
			cout << "区块链数据为空，开始同步所有数据\n";
			syncdata(0);
		}
		else
		{
			//如果不需要同步所有数据，就只需要添加尾部少了的数据就可以
			//因为区块链preHash指向前一个区块，所以如果没有被篡改就不可能出现中间分叉的情况
			cout << "区块链未被篡改，试图寻找更长的区块链\n";
			syncdata(CountBlock());
		}
	}
	return 0;
}
int BlockChainCheck::verifyBlockChain()
{
	//区块链为空，返回0；区块链不正确，返回-1；区块链正确，返回1
	const int blocknum = CountBlock();
	//如果区块链文件是空的，说明还没有同步数据，返回0
	if (blocknum == 0)
	{
		return 0;
	}
	//i从1开始因为初始区块并没有前区块哈希值
	for (int i = 1; i < blocknum; i++)
	{
		std::string CalculateHash = sha256(ReadBlock("block" + to_string(i - 1)));
		std::string PreHash_str =PreHash("block" + to_string(i));
		if (CalculateHash != PreHash_str)
		{
			cout << "检测到block"+to_string(i-1)+"或者block"+to_string(i)+"之间哈希值不一致\n";
			return -1;
		}
	}
	//未检测错误
	return 1;
}

int BlockChainCheck::syncdata(int index)
{
	if (index == 0)
	{
		//删除原有区块链，从头开始同步区块链
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
	//现在已经获得了除自身外拥有最长区块链的节点列表
	if (Max_Block_length <= CountBlock())
	{
		cout << "没有更长区块链，寻找更长的区块链结束\n";
		return 0;
	}
	//存在比自身还长的区块链
	int Random = rand() % Max_Block_length_IP.size();
	string synnodeIP = Max_Block_length_IP[Random];
	string command = "index";
	command += to_string(index);
	SynClient* synclient = new SynClient(synnodeIP,command);
	if (synclient->startsyn() == -1)
	{
		while (true)
		{
			cout << "切换同步节点...\n";
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
					cout << "暂时无法切换节点，稍候尝试\n";
					return -2;
				}
			}
		}
	}
	delete synclient;
	Blocknum = CountBlock();
	return 0;
}
