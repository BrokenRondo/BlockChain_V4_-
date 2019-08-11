#include "analyzeNodes.h"
class Nodes
{
public:
	string IP = "";
	unsigned short port = 0;
	unsigned int netDistance = 0;
};
vector<Nodes>nodeslist;
struct Public_Key
{
	string IP;
	string key;
};
extern list<Public_Key> public_keys;
int dir(string path)
{
	long long hFile = 0;
	struct _finddata_t fileInfo;
	string pathName, exdName;
	// \\* ����Ҫ�������е�����,��ĳ�\\*.jpg��ʾ����jpg�����ļ�
	if ((hFile = _findfirst(pathName.assign(path).append("\\*.json").c_str(), &fileInfo)) == -1) {
		return 0;
	}
	int i = 0;
	do
	{
		//�ж��ļ����������ļ��л����ļ�
		//cout << fileInfo.name << (fileInfo.attrib&_A_SUBDIR ? "[folder]" : "[file]") << endl;
		i++;
	} while (_findnext(hFile, &fileInfo) == 0);
	_findclose(hFile);
	return i;
}
//IP��ַ���Ϸ�
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
					printf("IP��ַ���Ϸ�(%d)\n", num);
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
	//����3����Ϊ���Ϸ�
	if (isDot > 3)
	{
		return false;
	}
	return true;
}
//��������ip֮���ж���ip���Դ˵����߼�����
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
//�ָ�IP��ַ��ŵ�����
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
//�жϽڵ��Ƿ��Ѿ��������б���
bool isNewNode(string newNode)
{
	ifstream fin("./NodesList/NodeList.txt");
	string temp = "";
	bool flag = true;
	string nodeIp = nodeIP(newNode);
	while (getline(fin, temp))
	{
		int locationIP = temp.find(":");
		string IP = temp.substr(0, locationIP);
		if (IP == nodeIp)flag = false;
	}
	return flag;
}
//��ȡ�ַ����е�IP����
string nodeIP(string Nodestr)
{
	int location_IP = 0;
	location_IP = Nodestr.find(':');
	string IP = Nodestr.substr(0, location_IP);
	return IP;
}
//��ȡ�ַ����еĶ˿ڲ���
unsigned short nodePort(string Nodestr)
{
	int location_Begin = 0, location_IP = 0, location_port = 0;
	location_IP = Nodestr.find(':');
	location_port = Nodestr.find(',');
	string Port = Nodestr.substr(location_IP + 1, location_port - location_IP - 1);
	return (short)stoi(Port);
}
//��ȡ�ַ����е��߼����벿��
string nodePublicKey(string Nodestr)
{
	int left = Nodestr.find(",") + 1;
	int right = Nodestr.length() - 1;
	string temp = Nodestr.substr(left, right);
	string key = temp.substr(temp.find_first_not_of(' '), temp.find_last_not_of(' '));
	return key;
}
//��ȡ�ַ����е��߼����벿�֣�ֱ�ӷ���0��
unsigned nodeDis(string Nodestr)
{
	return 0;
}
//�Խ��յĽڵ��б���н���
void anaNodes(string buf, string IP)
{
	string tobeAnalyze = buf;
	//�����ݽ��н������涨xxx..:xxx..,xxx...$Ϊһ��������Ϣ,0xffΪ�����Ǻ�
	int location_Begin = 0, location_IP = 0, location_port = 0;//��λ�ֽ��
	int pos = 0;//һ����������
	while (tobeAnalyze[pos] != '@')
	{
		while (tobeAnalyze[pos] != '$')
		{
			pos++;
		}//�ҵ�һ�ν����Ϣ�Ľ����Ǻ�
		string s = tobeAnalyze.substr(location_Begin, pos - location_Begin + 1);
		location_IP = s.find(':');
		location_port = s.find(',');
		string peerIP = s.substr(0, location_IP);
		string Port = s.substr(location_IP + 1, location_port - location_IP - 1);
		string publickey = nodePublicKey(s);
		Public_Key new_key = { peerIP,publickey };
		public_keys.push_back(new_key);
		unsigned int dis = getIPCount((char*)IP.c_str(), (char*)peerIP.c_str());
		Nodes newnode{ peerIP,std::stoi(Port),dis };
		nodeslist.push_back(newnode);
		location_Begin = pos + 1;
		pos++;
	}
	printf("�����ڵ��б�ɹ�,��ȡ��P2P�ڵ㣺\n");
	for (int i = 0; i < nodeslist.size(); i++)
	{
		printf("%s:%d,����%d\n", nodeslist[i].IP.c_str(), nodeslist[i].port, nodeslist[i].netDistance);
	}
	printf("������ӵ��ڵ��б�...\n");
	ofstream fout("./NodesList/NodeList.txt", ios::app);
	for (int i = 0; i < nodeslist.size(); i++)
	{
		string s = to_string(nodeslist[i].port) + ',';
		string ip = nodeslist[i].IP + ":";
		if (isNewNode(ip))
			fout << std::left << setw(20) << ip << setw(8) << s << nodeslist[i].netDistance << endl;
	}
	fout.close();
	printf("����ӵ��ڵ��б�\n");
}