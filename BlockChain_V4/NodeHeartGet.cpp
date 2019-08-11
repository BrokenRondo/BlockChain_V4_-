#include "NodeHeartGet.h"
int NodeHeartGet::Routine()
{
	cout << "更新节点线程启动\n";
	while (true)
	{
		//每两分钟更新一次节点
		OS_Thread::Sleep(120);
		cout << "准备更新节点\n向列表中的节点发送心跳索取请求\n";
		//给节点列表中的所有节点发送心跳包，如果没有回复则认为已经离线
		//ifstream fin(NODELIST_PATH);
		ofstream fout("NodeListnew.txt",ios::binary);
		string temp = "";
		vector<string> ipvector=Nodes_vector;
		vector<string> ipvector_temp;
		vector<int> portvector;
		vector<int> disvector;
		//while (getline(fin, temp))
		//{
		//	if (temp == "")continue;;//读到空行直接跳过
		//	string IP = nodeIP(temp);
		//	unsigned short port = nodePort(temp);
		//	unsigned int dis = 0;// nodeDis(temp);
		//	ipvector.push_back(IP);
		//	portvector.push_back(port);
		//	disvector.push_back(dis);
		//}
		//fin.close();
		//fout.close();
		unsigned int disconnect_num = 0;
		for (unsigned int i = 0; i < ipvector.size(); i++)
		{
			OS_TcpSocket* Client=new OS_TcpSocket();
			Client->SetOpt_RecvTimeout(3000);
			Client->SetOpt_SendTimeout(3000);
			Client->Open();
			OS_SockAddr Serveraddr(ipvector[i].c_str(), PORT_NODELISTENING);
			if (Client->Connect(Serveraddr) < 0)
			{
				cout << "无法连接节点：" << ipvector[i] << endl;
				Client->Close();
				delete Client;
				continue;
			}
			char buf[BUF_SIZE];
			strcpy(buf, "hello");
			buf[0] = NODE_HEARTGET;
			int n = strlen(buf);
			Client->Send(buf,n);
			cout << "索取节点" << ipvector[i] << "的心跳回应\n";
			n = Client->Recv(buf, sizeof(buf));
			if (n == -1)
			{
				cout << "未收到来自" << ipvector[i] << "的心跳回应\n";
				disconnect_num++;
				delete Client;
				continue;
			}
			buf[n] = 0;
			if (REPLY_HEART == buf[0])
			{
				cout << "收到来自" << ipvector[i]<< "的心跳回应\n";
				ipvector_temp.push_back(ipvector[i]);
				string IP = ipvector[i] + ":";
				string Port_str = to_string(PORT_NODELISTENING);
				Port_str += ",";
				//ofstream fout("NodeListnew.txt",ios::app);
				int dis = 0;//目前貌似没有价值的距离参数
				fout << std::left << setw(20) << IP << setw(8) << Port_str << to_string(dis) << endl;
				//fout.close();
			}
			else
			{
				cout << "未收到来自" << ipvector[i] << "的心跳回应\n";
				disconnect_num++;
				delete Client;
				continue;
			}
			delete Client;
		}
		fout.close();
		if (disconnect_num== ipvector.size() + 1)
		{
			cout << "未获得所有节点心跳回应，判断自身与网络失去连接\n";
			net_connect = false;
		}
		if (remove(NODELIST_PATH) != -1)
		{
			if (std::rename("NodeListnew.txt", NODELIST_PATH) != -1)
			{
				cout << "更新节点完毕\n";
				Nodes_vector = ipvector_temp;
				Create_matrix();
			}
			else
				cout << "重命名临时节点列表失败\n";
		}
		else 
			cout << "删除原节点列表失败\n";
	}
	return 0;
}