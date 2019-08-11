#include "CaseBroadCast.h"
//转发病历的线程部分
int CaseBroadcast::Routine()
{
	//获取连接对象Ip和Port
	//ifstream fin(NODELIST_PATH);
	//if (!fin)
	//{
	//	cout << "节点列表文件丢失...\n";
	//	return -1;
	//}
	//string temp = "";
	//vector<string> ipvector;
	//while (getline(fin, temp))
	//{
	//	if (temp == "")
	//		continue;//读到空行直接跳过
	//	string IP = nodeIP(temp);
	//	ipvector.push_back(IP);
	//}
	//fin.close();
	vector<string> ipvector=Nodes_vector;
	//转发完毕，创建客户端发送
	for (int i = 0; i < ipvector.size(); i++)
	{
		OS_TcpSocket* newClient = new OS_TcpSocket;
		newClient->Open();
		newClient->SetOpt_RecvTimeout(3000);
		newClient->SetOpt_SendTimeout(3000);
		OS_SockAddr *newServeraddr = new OS_SockAddr(ipvector[i].c_str(), PORT_CASERECIEVE);
		if (newClient->Connect(*newServeraddr) < 0)
		{
			cout << "无法向节点发送公共病历信息" + ipvector[i] + "\n";
			continue;
		}
		char* newbuf = new char[BUF_SIZE];
		//若发送HTML文件则需分包
		strcpy(newbuf, this->case_str.c_str());
		int n = strlen(newbuf);
		newClient->Send(newbuf, n);
		newClient->Close();
		delete newClient;
		delete newServeraddr;
		delete[] newbuf;
	}
	return 0;
}