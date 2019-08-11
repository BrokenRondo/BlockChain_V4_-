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
string trapoolstr;//用来存放病历的字符串，json格式，（在一个区块还未生成之前）
vector<vector<vector<string>>> quorum_matrix;//生成的仲裁矩阵
vector<vector<string>> height_set;//每个高度和其节点对该高度的签名
string SynIPSig;//仲裁节点建议的同步对象
vector<string> Nodes_vector;//节点列表
wchar_t* Utf8_2_Unicode(string row_i)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, row_i.c_str(), strlen(row_i.c_str()), NULL, 0);
	wchar_t *wszStr = new wchar_t[len + 1];
	MultiByteToWideChar(CP_UTF8, 0, row_i.c_str(), strlen(row_i.c_str()), wszStr, len);
	wszStr[len] = '\0';
	return wszStr;
}