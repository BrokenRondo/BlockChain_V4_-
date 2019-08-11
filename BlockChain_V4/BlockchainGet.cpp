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
		cout << "�޷�����ͬ������...\n";
		this->Client.Close();
		return -1;
	}
	strcpy(this->buf, this->command.c_str());
	int n = strlen(this->buf);
	this->Client.Send(this->buf, n);
	int blocknum = CountBlock();
	if (blocknum != 0)//������������ǿյĻ�
	{
		blocknum -= 1;
	}
	//ͬ����ʼ
	ofstream oqf("block.txt");
	oqf.close();
	ofstream of("block.txt", ios::app);
	for (int i = 0;; i++)
	{
		string blockdata = "";
		while (true)
		{
			// ��������
			n = Client.Recv(buf, sizeof(buf));
			if (-1 == n)
			{
				cout << "������������ʧ��...\n������ͬ��ʱʧ��...\n";
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
		//���յ�һ�������������
		//cout << "д�뵽ͬ�����ݣ�\n" << blockdata << endl;
		of << blockdata+"###";
		//WriteBlock(blockdata);
		//blocknum = CountBlock();
		//cout << "��ͬ����" << blocknum << "������\n";
	}
loop:
	cout << "�������������\n";
	BlockWriteDB* db = new BlockWriteDB();
	db->Run();
	//Blocknum = CountBlock();
	return 0;
}