#include "BlockPack.h"
int BlockPack::Routine()
{
	std::cout << "区块打包线程启动....\n";
	while (true)
	{
		//用来维持同步时钟
		while (true)
		{
			int time = gettime_minute() % 3;
			if (time == 0)//分钟是3的整数倍时跳出while进行打包
			{
				break;
			}
			else
			{
				OS_Thread::Msleep(100);
			}
		}
		//病历池为空，则无需打包
		if (Commonpool.empty())
		{
			OS_Thread::Sleep(60);
			continue;
		}
		Json::Value root;
		Json::Value value;
		Json::Reader reader;
		OS_Mutex mutex;
		mutex.Lock();
		if (!reader.parse(Commonpool, value))
		{
			cout << "打包区块时公共病历池Json格式解析失败！" << endl;
			continue;
		}
		Commonpool = "";
		//但是不置空公共病历池，因为不一定所有病历都能被共识
		mutex.Unlock();
		root["virsion"] = "1.00";
		int blocknum = CountBlock() - 1;
		const string preHashCalculated = sha256(ReadBlock("block" + to_string(blocknum)));
		root["prehash"] = preHashCalculated;
		root["timestamp"] = "2018-1-1 1:1:0";
		root["blockindex"] = to_string(blocknum + 1);
		root["trans"] = value;
		Json::StyledWriter swriter;
		string blockdata = swriter.write(root);
		cout << blockdata << endl;
		//获取转发IP
		ifstream  fin(NODELIST_PATH);
		std::string temp;
		while (getline(fin, temp))
		{
			if (temp.empty())continue;
			std::string IP = nodeIP(temp);
			if (IP == LocalIP)continue;
			OS_TcpSocket* Client = new OS_TcpSocket();
			Client->SetOpt_RecvTimeout(3000);
			Client->SetOpt_SendTimeout(3000);
			Client->Open();
			OS_SockAddr Serveraddr(IP.c_str(), PORT_BLOCKRECIEVE);
			if (Client->Connect(Serveraddr) < 0)
			{
				cout << "无法连接节点：" << IP << endl;
				Client->Close();
				delete Client;
				continue;
			}
			string blockhash = sha256(blockdata);
			char buf[BUF_SIZE];
			strcpy(buf, blockhash.c_str());
			int n = strlen(buf);
			Client->Send(buf, n);
			delete Client;
		}
		fin.close();
		WriteBlock(blockdata);
		Blocknum = CountBlock();
		OS_Thread::Sleep(60);
	}
	return 0;
}