#include "CaseBroadCast.h"
//ת���������̲߳���
int CaseBroadcast::Routine()
{
	//��ȡ���Ӷ���Ip��Port
	//ifstream fin(NODELIST_PATH);
	//if (!fin)
	//{
	//	cout << "�ڵ��б��ļ���ʧ...\n";
	//	return -1;
	//}
	//string temp = "";
	//vector<string> ipvector;
	//while (getline(fin, temp))
	//{
	//	if (temp == "")
	//		continue;//��������ֱ������
	//	string IP = nodeIP(temp);
	//	ipvector.push_back(IP);
	//}
	//fin.close();
	vector<string> ipvector=Nodes_vector;
	//ת����ϣ������ͻ��˷���
	for (int i = 0; i < ipvector.size(); i++)
	{
		OS_TcpSocket* newClient = new OS_TcpSocket;
		newClient->Open();
		newClient->SetOpt_RecvTimeout(3000);
		newClient->SetOpt_SendTimeout(3000);
		OS_SockAddr *newServeraddr = new OS_SockAddr(ipvector[i].c_str(), PORT_CASERECIEVE);
		if (newClient->Connect(*newServeraddr) < 0)
		{
			cout << "�޷���ڵ㷢�͹���������Ϣ" + ipvector[i] + "\n";
			continue;
		}
		char* newbuf = new char[BUF_SIZE];
		//������HTML�ļ�����ְ�
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