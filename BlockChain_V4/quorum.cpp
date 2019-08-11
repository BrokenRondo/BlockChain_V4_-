#include "quorum.h"
#define HEIGHT_DATA 0x01
#define HEIGHT_GET 0x02
#define SIG_GET 0x03
//�����ٲþ���
bool Create_matrix()
{
	//��ȡ��ǰ���нڵ�IP��ַ
	//ifstream fin(NODELIST_PATH);
	//string temp = "";
	//vector<string> ipvector;
	//while (getline(fin, temp))
	//{
	//	if (temp == "")continue;//��������ֱ������
	//	string IP = nodeIP(temp);
	//	ipvector.push_back(IP);
	//}
	//fin.close();
	vector<string> ipvector=Nodes_vector;
	sort(ipvector.begin(), ipvector.end());
	//��ȡ��ǰ�ڵ���������Ѱ�Һ��ʵ�h��d��r������������ǰ�ڵ���������(number<4)��������
	if (ipvector.size() < 4)
		return false;
	vector<unsigned int> vec_temp = Get_hrd(ipvector);
	unsigned int d = vec_temp[0]; cout << "d:" + to_string(d) + "\n";
	unsigned int h = vec_temp[1]; cout << "h:" + to_string(h) + "\n";
	unsigned int r = vec_temp[2]; cout << "r:" + to_string(r) + "\n";
	//�����node
	random_shuffle(ipvector.begin(), ipvector.end());
	//���ݽڵ��б����������
	quorum_matrix.clear();//������ٲþ���
	for (unsigned int i = 0; i < d; i++)
	{
		vector<vector<string>> d_temp;
		quorum_matrix.push_back(d_temp);
		for (unsigned int j = 0; j < h; j++)
		{
			vector<string> h_temp;
			quorum_matrix[i].push_back(h_temp);
			for (unsigned int z = 0; z < r; z++)
			{
				if (ipvector.empty())
					continue;
				string r_temp = ipvector[0];
				quorum_matrix[i][j].push_back(r_temp);
				ipvector.erase(ipvector.begin());
			}
		}
	}
	cout<<"��ǰ�����ٲþ���\n";
	for (int i = 0; i < quorum_matrix.size(); i++)
	{
		for (int j = 0; j < quorum_matrix[i].size(); j++)
			cout << quorum_matrix[i][j][0]<<"	";
		cout << endl;
	}
	//������
	return true;
}
//�����ֽ�
vector<unsigned int> factorization(unsigned int size)
{
	vector<unsigned int> result;
	if (size <= 0)
		return result;
	int i = 2;
	//�����ֽ�
	while (true)
	{
		if (size % i == 0)
		{
			size /= i;
			result.push_back(i);
			if (size == 1)
				break;
		}
		else
			i += 1;
		if (i == size)
		{
			result.push_back(i);
			break;
		}
	}
	return result;
}
//��ȡhrd,d=[0],h=[1],r=[2]
vector<unsigned int> Get_hrd(vector<string> ipvector)
{
	vector<unsigned int> result;
	unsigned int size = ipvector.size();
	//�����ֽ�
	vector<unsigned int> vec_temp = factorization(size);
	while (true)
	{
		if (vec_temp.size() != 1)
			break;
		//���Ŀǰ�ڵ���Ŀ�����������޷����ɾ��󣬿������ȥ��1���ڵ�������h��d��r
		unsigned int randnode = (rand() % size);//���ȥ���ڵ����ţ���0����
		ipvector.erase(ipvector.begin() + randnode);
		size = ipvector.size();
		vec_temp.clear();
		vec_temp = factorization(size);
	}
	//Ѱ�Һ��ʵ�d��h*r
	int s = sqrt(size);
	unsigned int t = 1;
	unsigned int i = 1;
	for (unsigned int j = 0; j < vec_temp.size(); j++)
	{
		if (i*vec_temp[j] > s)
			break;
		i *= vec_temp[j];
	}
	for (unsigned int j = 0; j < vec_temp.size(); j++)
	{
		t *= vec_temp[j];
		if (t > s)
			break;
	}
	//ȷ��d
	if ((t - size / t)*(t - size / t) > (i - size / i)*(i - size / i))
		result.push_back(i);
	else
		result.push_back(t);
	//Ѱ���ʺϵ�h��r
	unsigned int hr = (unsigned int)size / result[0];
	vec_temp.clear();
	vec_temp = factorization(hr);
	//�������������ֱ��r=1��h=size / result[0]
	if (vec_temp.size() == 1)
	{
		result.push_back(hr);//ȷ��h
		result.push_back(1);//ȷ��r
		return result;
	}
	//����Ѱ�Һ��ʵ�h��r
	size = vec_temp.size();
	s = sqrt(size);
	t = 1;
	i = 1;
	for (unsigned int j = 0; j < vec_temp.size(); j++)
	{
		if (i*vec_temp[j] > s)
			break;
		i *= vec_temp[j];
	}
	for (unsigned int j = 0; j < vec_temp.size(); j++)
	{
		t *= vec_temp[j];
		if (t > s)
			break;
	}
	//ȷ��h
	if (fabs(t - hr / t) < fabs(i - hr / i))
		result.push_back(i > (hr / i) ? i : (hr / i));
	else
		result.push_back(t > (hr / t) ? t : (hr / t));
	//ȷ��r
	result.push_back((unsigned int)hr / result[1]);
	return result;
}
//��ȡ�ٲýڵ�
vector<string> Get_quorum()
{
	vector<string> quorum_node;
	//����ÿһ��hѡȡ��һ��r�����нڵ�Ϊ�ٲýڵ�
	//���һ��h��2������е�r���ѡȡһ���ڵ���Ϊ�ٲýڵ�
	if (quorum_matrix.empty())
		return quorum_node;
	for (unsigned int h = 0; h < quorum_matrix[0].size(); h++)
	{
		quorum_node.insert(quorum_node.end(), quorum_matrix[0][h].begin(), quorum_matrix[0][h].end());
		if (h == quorum_matrix[0].size() - 1)
		{
			for (unsigned int d = 1; d < quorum_matrix.size(); d++)
			{
				int size = quorum_matrix[d][h].size();
				if (size == 0)
					continue;
				unsigned int randnum = rand() % size;
				quorum_node.push_back(quorum_matrix[d][h][randnum]);
			}
		}
	}
	//ȥ���ظ�Ԫ��
	quorum_node.erase(unique(quorum_node.begin(),quorum_node.end()),quorum_node.end());
	cout << "��ȡ�ٲýڵ�" << endl;
	for (int i = 0; i <quorum_node.size(); i++)
		cout << quorum_node[i] << endl;
	return quorum_node;
}
//�㲥�߶���Ϣ
void BroadCastHeight(int H)
{
	//��ȡ��ǰ���нڵ�IP��ַ
	//ifstream fin(NODELIST_PATH);
	//string temp = "";
	//vector<string> ipvector;
	//while (getline(fin, temp))
	//{
	//	if (temp == "")continue;//��������ֱ������
	//	string IP = nodeIP(temp);
	//	ipvector.push_back(IP);
	//}
	//fin.close();
	vector<string> ipvector=Nodes_vector;
	//��ȡ��ǰ�߶���Ϣ
	string height = to_string(H);//�������߶�
	//*********************************************************************
	string senddata = 'T'+height+"#"+LocalIP;
	senddata[0]=HEIGHT_DATA;
	//*********************************************************************
	//ѭ�����͸߶�
	for (unsigned int i = 0; i < ipvector.size(); i++)
	{
		OS_TcpSocket* Client = new OS_TcpSocket();
		Client->SetOpt_RecvTimeout(3000);
		Client->SetOpt_SendTimeout(3000);
		Client->Open();
		OS_SockAddr Serveraddr(ipvector[i].c_str(), PORT_QUORUM);
		if (Client->Connect(Serveraddr) < 0)
		{
			cout << "�޷����ӽڵ㣺" << ipvector[i] << endl;
			Client->Close();
			delete Client;
			continue;
		}
		char buf[BUF_SIZE];
		strcpy(buf, senddata.c_str());
		int n = strlen(buf);
		Client->Send(buf, n);
		cout << "����" << ipvector[i] << "���͸߶���Ϣ��ǩ��\n";
		delete Client;
	}
}
//��ȡ��Ч�߶ȣ�������ͬ������IP
vector<string> Get_MaxHeight_IP()
{
	int Height = 0;
	string Height_IP = "";
	//��ȡ�ٲýڵ�
	vector<string> ipvector = Get_quorum();
	vector<vector<string>> height_num;
	//ѭ������������Ϣ
	for (unsigned int i=0;i<ipvector.size();i++)
	{
		OS_TcpSocket* Client = new OS_TcpSocket();
		Client->SetOpt_RecvTimeout(3000);
		Client->SetOpt_SendTimeout(3000);
		Client->Open();
		OS_SockAddr Serveraddr(ipvector[i].c_str(), PORT_QUORUM);
		if (Client->Connect(Serveraddr) < 0)
		{
			cout << "�޷����ӽڵ㣺" << ipvector[i] << endl;
			Client->Close();
			delete Client;
			continue;
		}
		char buf[BUF_SIZE];
		strcpy(buf, "hello");
		buf[0] = HEIGHT_GET;
		int n = strlen(buf);
		Client->Send(buf, n);
		cout << "����" << ipvector[i] << "��ȡ�߶ȼ���\n";
		n = Client->Recv(buf, sizeof(buf));
		if (n == -1)
		{
			delete Client;
			continue;
		}
		buf[n] = 0;
		string recvdata = buf;
		int pos =recvdata.find('#');
		string height_temp = recvdata.substr(0, pos).c_str();
		string client_IP= recvdata.substr(pos + 1);
		bool exit = false;
		for (unsigned int j = 0; j < height_num.size(); j++)
		{
			if (height_num[j][0] == height_temp)
			{
				exit = true;
				height_num[j][1] += client_IP+"~";
				break;
			}
		}
		if (!exit)
		{
			vector<string> height_num_temp;
			height_num_temp.push_back(height_temp);
			height_num_temp.push_back(client_IP+"~");
			height_num.push_back(height_num_temp);
		}
		delete Client;
	}
	for (unsigned int i = 0; i < height_num.size(); i++)
	{
		string temp = height_num[i][1];//ǩ������
		int num = count(temp.begin(), temp.end(), '~');
		if (num > ipvector.size()/2)
		{
			//������������
			if (atoi(height_num[i][0].c_str()) > Height)
			{
				Height = atoi(height_num[i][0].c_str());
				Height_IP = height_num[i][1];
			}
		}
	}
	cout << "�����Ч�߶�Ϊ" << Height << endl;
	int pos1 = Height_IP.find('~');
	Height_IP = Height_IP.substr(0, pos1);//�߶�
	cout << "���ٲýڵ�:" << Height_IP << "����ǩ������\n";
	string IPsig=Get_synIP(Height_IP);//��ȡͬ������IP
	vector<string> result;
	result.push_back(to_string(Height));
	result.push_back(IPsig);
	return result;
}
string Get_synIP(string IP)
{

	OS_TcpSocket* Client = new OS_TcpSocket();
	Client->SetOpt_RecvTimeout(3000);
	Client->SetOpt_SendTimeout(3000);
	Client->Open();
	OS_SockAddr Serveraddr(IP.c_str(), PORT_QUORUM);
	if (Client->Connect(Serveraddr) < 0)
	{
		cout << "�޷����ӽڵ㣺" << IP << endl;
		Client->Close();
		delete Client;
		return "error";
	}
	char buf[BUF_SIZE];
	strcpy(buf, "hello");
	buf[0] = SIG_GET;
	int n = strlen(buf);
	Client->Send(buf, n);
	n = Client->Recv(buf, sizeof(buf));
	if (n == -1)
	{
		delete Client;
		return "error";
	}
	buf[n] = 0;
	string recvdata = buf;
	return recvdata;
}
