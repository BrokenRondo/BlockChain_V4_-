#include "BlockchainGet.h"
SynClient::SynClient(string creationIP,string command)
{
	this->CreationIP = creationIP;
	this->Client.Open();
	this->Client.SetOpt_RecvTimeout(3000);
	this->Client.SetOpt_SendTimeout(3000);
	this->Serveraddr.SetIp(this->CreationIP.c_str());
	this->Serveraddr.SetPortA(PORT_BLOCKCHAINSYN);
	this->command = command;
}
int SynClient::startsyn()
{
	if (this->Client.Connect(this->Serveraddr) < 0)
	{
		cout << "无法连接同步对象...\n";
		this->Client.Close();
		return -1;
	}
	strcpy(this->buf, this->command.c_str());
	int n = strlen(this->buf);
	this->Client.Send(this->buf, n);
	int blocknum = CountBlock();
	if (blocknum != 0)//如果区块链不是空的话
	{
		blocknum -= 1;
	}
	//同步开始
	ofstream oqf("block.txt");
	oqf.close();
	ofstream of("block.txt", ios::app);
	for (int i = 0;; i++)
	{
		string blockdata = "";
		while (true)
		{
			// 接收数据
			n = Client.Recv(buf, sizeof(buf));
			if (-1 == n)
			{
				cout << "接收区块数据失败...\n区块链同步时失败...\n";
				return -1;
			}
			buf[n] = 0;
			int recvcommand = buf[0];
			if (recvcommand == LAST_DATA)
			{
				string bufstr = buf;
				string temp(bufstr, 1);
				blockdata+= temp;
				break;
			}
			if (recvcommand == ALL_END)
			{
				of.close();
				goto loop;
			}
			string bufstr = buf;
			string temp(bufstr, 1);
			blockdata += temp;
		}
		//接收到一个完整的区块后
		//cout << "写入到同步数据：\n" << blockdata << endl;
		of << blockdata+"###";
		//WriteBlock(blockdata);
		//blocknum = CountBlock();
		//cout << "已同步第" << blocknum << "个区块\n";
	}
loop:
	cout << "区块链接收完成\n";
	BlockWriteDB* db = new BlockWriteDB();
	db->Run();
	//Blocknum = CountBlock();
	return 0;
}