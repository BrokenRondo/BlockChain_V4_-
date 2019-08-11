#include "NodeAnalyse.h"
class Nodes
{
public:
	string IP = "";
	unsigned int port = 0;
	unsigned int netDistance = 0;
};
//判断IP地址是否合法
bool isValidIP(char IP[])
{
	int num;
	int isDot = 0;
	bool flag = true;
	for (int i = 0; i < (int)strlen(IP); i++)
	{
		if (IP[i] != '\0')
		{
			if (IP[i] != '.')
			{
				if (flag == true)
				{
					num = atoi(IP + i);
					flag = false;
				}

				if (num < 0 || num>255)
				{
					//printf("IP地址不合法(%d)\n", num);
					return false;
				}
			}
			else
			{
				isDot++;
				flag = true;
			}
		}
	}
	//超出3个点为不合法
	if (isDot > 3)
	{
		return false;
	}
	return true;
}
//计算两个ip之间有多少ip，以此当做逻辑距离
unsigned int getIPCount(char ip1[], char ip2[])
{
	if (!isValidIP(ip2) || !isValidIP(ip2))
	{
		return -1;
	}
	int arr1[4];
	int arr2[4];
	int res = 1;
	cutIPAddr(ip1, arr1);
	cutIPAddr(ip2, arr2);
	for (int i = 0; i < 4; i++)
	{
		res *= (abs(arr1[i] - arr2[i]) + 1);
	}
	return res;
}
//分割IP地址存放到数组
void cutIPAddr(char ip[], int arr[])
{
	bool flag = true;
	int times = 0;
	for (int i = 0; i < (int)strlen(ip); i++)
	{
		if (ip[i] != '\0')
		{
			if (ip[i] != '.')
			{
				if (flag == true)
				{
					arr[times] = atoi(ip + i);
					times++;
					flag = false;
				}
			}
			else
			{
				flag = true;
			}
		}
	}
}
//判断节点是否已经存在在列表中
bool isNewNode(string newNode)
{
	//ifstream fin(NODELIST_PATH);
	//string temp = "";
	//bool flag = true;
	//string nodeIp = nodeIP(newNode);
	//while (getline(fin, temp))
	//{
	//	int locationIP = temp.find(":");
	//	string IP = temp.substr(0, locationIP);
	//	if (IP == nodeIp)flag = false;
	//}
	bool flag = true;
	for (unsigned int i = 0; i < Nodes_vector.size(); i++)
	{
		if (Nodes_vector[i] == newNode)
		{
			flag = false;
			return flag;
		}
	}
	return flag;
}
//获取字符串中的IP部分
string nodeIP(string Nodestr)
{
	int location_IP = 0;
	location_IP = Nodestr.find(':');
	string IP = Nodestr.substr(0, location_IP);
	return IP;
}
//获取字符串中的端口部分
unsigned short nodePort(string Nodestr)
{
	int location_Begin = 0, location_IP = 0, location_port = 0;
	location_IP = Nodestr.find(':');
	location_port = Nodestr.find(',');
	string Port = Nodestr.substr(location_IP + 1, location_port - location_IP - 1);
	return (short)stoi(Port);
}
//获取字符串中的逻辑距离部分
string nodePublicKey(string Nodestr)
{
	int left = Nodestr.find(",") + 1;
	int right = Nodestr.length() - 1;
	string temp = Nodestr.substr(left, right);
	string key = temp.substr(temp.find_first_not_of(' '), temp.find_last_not_of(' '));
	return key;
}
//对接收的节点列表进行解析
void anaNodes(string buf, string IP)
{
	vector<Nodes>nodeslist;
	string tobeAnalyze = buf;
	//对数据进行解析，规定xxx..:xxx..,xxx...$为一个结点的信息,0xff为结束记号
	int location_Begin = 0, location_IP = 0, location_port = 0;//定位分界点
	int pos = 0;//一个个向后查找
	while (tobeAnalyze[pos] != '@')
	{
		while (tobeAnalyze[pos] != '$')
		{
			pos++;
		}//找到一段结点信息的结束记号
		string s = tobeAnalyze.substr(location_Begin, pos - location_Begin + 1);
		location_IP = s.find(':');
		location_port = s.find(',');
		string peerIP = s.substr(0, location_IP);
		string Port = s.substr(location_IP + 1, location_port - location_IP - 1);
		string publickey = nodePublicKey(s);
		Public_Key new_key = { peerIP,publickey };
		public_keys.push_back(new_key);
		unsigned int dis = getIPCount((char*)IP.c_str(), (char*)peerIP.c_str());
		Nodes newnode{ peerIP,(unsigned int)atoi(Port.c_str()),dis };
		nodeslist.push_back(newnode);
		location_Begin = pos + 1;
		pos++;
	}
	printf("解析节点列表成功,获取的P2P节点如下：\n");
	for (unsigned int i = 0; i < nodeslist.size(); i++)
	{
		Nodes_vector.push_back(nodeslist[i].IP);
		printf("%s:%d,距离%d\n", nodeslist[i].IP.c_str(), nodeslist[i].port, nodeslist[i].netDistance);
	}
	printf("开始添加到节点列表...\n");
	ofstream fout(NODELIST_PATH, ios::app);
	for (unsigned int i = 0; i < nodeslist.size(); i++)
	{
		string s = to_string(nodeslist[i].port) + ',';
		string ip = nodeslist[i].IP + ":";
		if (isNewNode(nodeslist[i].IP))
			fout << std::left << setw(20) << ip << setw(8) << s << nodeslist[i].netDistance << endl;
	}
	fout.close();
	printf("已添加到节点列表\n");
}