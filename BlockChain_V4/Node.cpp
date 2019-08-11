#include "Node.h"
Node::Node(string creationIP)
{
	this->CreationIP = creationIP;
	this->Client.Open();
	this->Client.SetOpt_RecvTimeout(10000);
	this->Client.SetOpt_SendTimeout(10000);
	this->Serveraddr.SetIp(this->CreationIP.c_str());
	this->Serveraddr.SetPortA(PORT_NODELISTENING);
}

int Node::setNetEnvironment()
{
	Public_Key key_i{ LocalIP,keys.publicKey };
	public_keys.push_back(key_i);
	return 0;
}

int Node::LoginNet()
{
	if (this->Client.Connect(this->Serveraddr) < 0)
	{
		cout << "�޷����Ӵ����ڵ�...\n";
		this->Client.Close();
		return -1;
	}
	else
	{
		cout << "�ɹ����Ӵ����ڵ�...\n";
	}
	//������Ϣ�������������
	for (int i = 0; i < 3; i++)
	{
		string temp = "hello";
		temp[0] = NODE_JOIN;
		cout << "��" << i << "�������������...\n";
		strcpy(this->buf, temp.c_str());
		int n = strlen(this->buf);
		this->Client.Send(this->buf, n);
		n = Client.Recv(buf, sizeof(buf));
		if (-1 == n)
		{
			cout << "���մ����ڵ���Ϣʧ��...\n";
			break;
		}
		buf[n] = 0;
		if (REPLY_JOIN == buf[0])
		{
			net_connect = true;
			cout << "�ɹ������������ڵ�����...\n";
			return 0;
		}
	}
	cout << "�����������ڵ�����ʧ��...\n";
	this->Client.Close();
	return -1;
}

int Node::netlistening_start(NetListening& netlisteningThread)
{
	if (0 == netlisteningThread.Run())
		return 0;
	else
	{
		cout << "�ڵ���������߳�����ʧ��\n";
		return -1;
	}
}

int Node::nodeheartget_start(NodeHeartGet& update)
{
	if (0 == update.Run())
		return 0;
	else 
	{
		cout << "��������ȡ�߳�����ʧ��\n";
		return -1;
	}
}

int Node::getP2PNodes()
{
	cout << "��ͼ����ڵ��б�...\n";
	string temp = "getnode";
	temp[0] = NODE_LIST;
	strcpy(this->buf, temp.c_str());
	int n = strlen(this->buf);
	this->Client.Send(buf, n);
	n = this->Client.Recv(buf, sizeof(buf));
	if ( -1==n)
	{
		cout << "��ȡ�ڵ��б�ʧ��...\n";
		return -1;
	}
	buf[n] = 0;
	cout << "��ȡ�б�������...\n";
	anaNodes(buf, LocalIP);
	return 0;
}

int Node::SendToList()
{
	//std::ifstream fin(NODELIST_PATH);
	std::string temp = "";
	std::vector<std::string> ipvector=Nodes_vector;
	//while (getline(fin, temp))
	//{
	//	if (temp == "")continue;;//��������ֱ������
	//	string IP = nodeIP(temp);
	//	unsigned short port = nodePort(temp);
	//	unsigned int dis = 0;// nodeDis(temp);
	//	if (IP == LocalIP)
	//		continue;
	//	else
	//	{
	//		ipvector.push_back(IP);
	//	}
	//}
	//fin.close();
	for (unsigned int i = 0; i < ipvector.size(); i++)
	{
		if (ipvector[i] == LocalIP)continue;
		OS_TcpSocket* newClient=new OS_TcpSocket;
		newClient->Open();
		newClient->SetOpt_RecvTimeout(3000);
		newClient->SetOpt_SendTimeout(3000);
		OS_SockAddr *newServeraddr=new OS_SockAddr(ipvector[i].c_str(), PORT_NODELISTENING);
		if (newClient->Connect(*newServeraddr) < 0)
		{
			cout << "�޷���ڵ�"+ipvector[i]+"��������ڵ���Ϣ\n";
			continue;
		}
		temp = "";
		temp += NODE_IN;
		temp+= LocalIP + ":" + to_string(PORT_NODELISTENING) + ","+ keys.publicKey + "$";
		char* newbuf = new char[BUF_SIZE];
		strcpy(newbuf, temp.c_str());
		int n = strlen(newbuf);
		newClient->Send(newbuf, n);
		newClient->Close();
		delete newClient;
		delete newServeraddr;
		delete[] newbuf;
	}
	return 0;
}

