#include "NodeHeartGet.h"
int NodeHeartGet::Routine()
{
	cout << "���½ڵ��߳�����\n";
	while (true)
	{
		//ÿ�����Ӹ���һ�νڵ�
		OS_Thread::Sleep(120);
		cout << "׼�����½ڵ�\n���б��еĽڵ㷢��������ȡ����\n";
		//���ڵ��б��е����нڵ㷢�������������û�лظ�����Ϊ�Ѿ�����
		//ifstream fin(NODELIST_PATH);
		ofstream fout("NodeListnew.txt",ios::binary);
		string temp = "";
		vector<string> ipvector=Nodes_vector;
		vector<string> ipvector_temp;
		vector<int> portvector;
		vector<int> disvector;
		//while (getline(fin, temp))
		//{
		//	if (temp == "")continue;;//��������ֱ������
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
				cout << "�޷����ӽڵ㣺" << ipvector[i] << endl;
				Client->Close();
				delete Client;
				continue;
			}
			char buf[BUF_SIZE];
			strcpy(buf, "hello");
			buf[0] = NODE_HEARTGET;
			int n = strlen(buf);
			Client->Send(buf,n);
			cout << "��ȡ�ڵ�" << ipvector[i] << "��������Ӧ\n";
			n = Client->Recv(buf, sizeof(buf));
			if (n == -1)
			{
				cout << "δ�յ�����" << ipvector[i] << "��������Ӧ\n";
				disconnect_num++;
				delete Client;
				continue;
			}
			buf[n] = 0;
			if (REPLY_HEART == buf[0])
			{
				cout << "�յ�����" << ipvector[i]<< "��������Ӧ\n";
				ipvector_temp.push_back(ipvector[i]);
				string IP = ipvector[i] + ":";
				string Port_str = to_string(PORT_NODELISTENING);
				Port_str += ",";
				//ofstream fout("NodeListnew.txt",ios::app);
				int dis = 0;//Ŀǰò��û�м�ֵ�ľ������
				fout << std::left << setw(20) << IP << setw(8) << Port_str << to_string(dis) << endl;
				//fout.close();
			}
			else
			{
				cout << "δ�յ�����" << ipvector[i] << "��������Ӧ\n";
				disconnect_num++;
				delete Client;
				continue;
			}
			delete Client;
		}
		fout.close();
		if (disconnect_num== ipvector.size() + 1)
		{
			cout << "δ������нڵ�������Ӧ���ж�����������ʧȥ����\n";
			net_connect = false;
		}
		if (remove(NODELIST_PATH) != -1)
		{
			if (std::rename("NodeListnew.txt", NODELIST_PATH) != -1)
			{
				cout << "���½ڵ����\n";
				Nodes_vector = ipvector_temp;
				Create_matrix();
			}
			else
				cout << "��������ʱ�ڵ��б�ʧ��\n";
		}
		else 
			cout << "ɾ��ԭ�ڵ��б�ʧ��\n";
	}
	return 0;
}