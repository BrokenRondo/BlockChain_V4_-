#include "BlockPack.h"
int BlockPack::Routine()
{
	std::cout << "�������߳�����....\n";
	while (true)
	{
		//����ά��ͬ��ʱ��
		while (true)
		{
			int time = gettime_minute() % 3;
			if (time == 0)//������3��������ʱ����while���д��
			{
				break;
			}
			else
			{
				OS_Thread::Msleep(100);
			}
		}
		//������Ϊ�գ���������
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
			cout << "�������ʱ����������Json��ʽ����ʧ�ܣ�" << endl;
			continue;
		}
		Commonpool = "";
		//���ǲ��ÿչ��������أ���Ϊ��һ�����в������ܱ���ʶ
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
		//��ȡת��IP
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
				cout << "�޷����ӽڵ㣺" << IP << endl;
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