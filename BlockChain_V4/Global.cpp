#include "Global.h"
string LocalIP;
string MainIP;
unsigned int Blocknum;
string Commonpool;
string Privatepool;
vector<string> Commonhashpool;
vector<string> Privatehashpool;
bool net_connect = false;
vector<Public_Key>public_keys;
KeyPairHex keys;
string signatures;
unsigned int NodeNumber;
string localIP;
string localhospital = "anhuihospital";
string trapoolstr;//������Ų������ַ�����json��ʽ������һ�����黹δ����֮ǰ��
vector<vector<vector<string>>> quorum_matrix;//���ɵ��ٲþ���
vector<vector<string>> height_set;//ÿ���߶Ⱥ���ڵ�Ըø߶ȵ�ǩ��
string SynIPSig;//�ٲýڵ㽨���ͬ������
vector<string> Nodes_vector;//�ڵ��б�
wchar_t* Utf8_2_Unicode(string row_i)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, row_i.c_str(), strlen(row_i.c_str()), NULL, 0);
	wchar_t *wszStr = new wchar_t[len + 1];
	MultiByteToWideChar(CP_UTF8, 0, row_i.c_str(), strlen(row_i.c_str()), wszStr, len);
	wszStr[len] = '\0';
	return wszStr;
}