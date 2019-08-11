#pragma once
#ifndef GLOBAL_H_
#define GLOBAL_H_
#include<string>
#include <vector>
#include "RSASIG.h"
using  namespace std;
//可能会用到的全局变量和全局函数
//监听节点请求服务器绑定端口
#define PORT_NODELISTENING 10010
//接收病历服务器绑定端口
#define PORT_CASERECIEVE 10020
//接收区块服务器绑定端口
#define PORT_BLOCKRECIEVE 10030
//同步区块链服务器绑定端口
#define PORT_BLOCKCHAINSYN 10040
//总裁服务器绑定端口 
#define PORT_QUORUM 10050
//数据库系统IP
#define DB_IP "127.0.0.1"
//数据库系统用户名
#define DB_USERNAME "root"
//数据库系统密码
#define DB_PASSWORD ""
//数据库名称
#define DB_NAME "blocksdata"
//缓冲区大小
#define BUF_SIZE 2048
//节点请求：加入网络声明
#define NODE_JOIN 0x01
//节点请求：请求获取节点列表
#define NODE_LIST 0x02
//节点请求：索取心跳包
#define NODE_HEARTGET 0x03
//节点请求：加入网络行为
#define NODE_IN 0x04
//节点请求响应：回复加入网络声明
#define REPLY_JOIN 0x66
//节点请求响应：心跳包检测回复
#define REPLY_HEART 0x67
//节点列表路径
#define NODELIST_PATH "NodeList.txt"
//单次数据接收结束指令
#define LAST_DATA 0x11
//多次数据接收结束指令
#define ALL_END 0x12
//多次数据接收未结束指令
#define CONTINUE_DATA 0x10
//获取区块链长度指令
#define GET_LEN 0x0A
//自身无线网卡的IP地址
extern string LocalIP;
extern string localIP;
//主节点的IP地址
extern string MainIP;
//自身区块链的个数，从1开始计算
extern unsigned int Blocknum;
//公共病历池
extern string Commonpool;
//自身病历池
extern string Privatepool;
//已接收过的广播病历的公共哈希值池
extern vector<string> Commonhashpool;
//已接收过的桌面端的私有哈希值池
extern vector<string> Privatehashpool;
//网络连接状态
extern bool net_connect;
//密钥池?
struct Public_Key
{
	string IP;
	string key;
};
extern vector<Public_Key>public_keys;
//节点自身密钥
extern KeyPairHex keys;
//签名
extern string signatures;
//本地医院名称
extern string localhospital;
struct MerkleRoot_IP
{
	std::string src_ip;
	std::string MerkleRoot;
};
extern unsigned int NodeNumber;
extern string trapoolstr;//用来存放病历的字符串，json格式，（在一个区块还未生成之前）
extern vector<vector<vector<string>>> quorum_matrix;//生成的仲裁矩阵
extern vector<vector<string>> height_set;//每个高度和其节点对该高度的签名
extern string SynIPSig;//仲裁节点建议的同步对象
wchar_t* Utf8_2_Unicode(string row_i);
extern vector<string> Nodes_vector;//节点列表
#endif // !GLOBAL_H_
