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
		cout << "无法连接创世节点...\n";
		this->Client.Close();
		return -1;
	}
	else
	{
		cout << "成功连接创世节点...\n";
	}
	//发送信息以申请加入网络
	for (int i = 0; i < 3; i++)
	{
		string temp = "hello";
		temp[0] = NODE_JOIN;
		cout << "第" << i << "次申请加入网络...\n";
		strcpy(this->buf, temp.c_str());
		int n = strlen(this->buf);
		this->Client.Send(this->buf, n);
		n = Client.Recv(buf, sizeof(buf));
		if (-1 == n)
		{
			cout << "接收创世节点信息失败...\n";
			break;
		}
		buf[n] = 0;
		if (REPLY_JOIN == buf[0])
		{
			net_connect = true;
			cout << "成功加入区块链节点网络...\n";
			return 0;
		}
	}
	cout << "加入区块链节点网络失败...\n";
	this->Client.Close();
	return -1;
}

int Node::netlistening_start(NetListening& netlisteningThread)
{
	if (0 == netlisteningThread.Run())
		return 0;
	else
	{
		cout << "节点请求监听线程启动失败\n";
		return -1;
	}
}

int Node::nodeheartget_start(NodeHeartGet& update)
{
	if (0 == update.Run())
		return 0;
	else 
	{
		cout << "心跳包获取线程启动失败\n";
		return -1;
	}
}

int Node::getP2PNodes()
{
	cout << "企图申请节点列表...\n";
	string temp = "getnode";
	temp[0] = NODE_LIST;
	strcpy(this->buf, temp.c_str());
	int n = strlen(this->buf);
	this->Client.Send(buf, n);
	n = this->Client.Recv(buf, sizeof(buf));
	if ( -1==n)
	{
		cout << "获取节点列表失败...\n";
		return -1;
	}
	buf[n] = 0;
	cout << "获取列表即将分析...\n";
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
	//	if (temp == "")continue;;//读到空行直接跳过
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
			cout << "无法向节点"+ipvector[i]+"发送自身节点信息\n";
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

