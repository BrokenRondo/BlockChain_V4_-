#pragma once
#ifndef NODEANALYSE_H_
#define NODEANALYSE_H_
#include<string>
#include<vector>
#include<fstream>
#include <io.h>
#include<iomanip>
#include "Global.h"
using namespace std;
//计算IP逻辑距离
unsigned int getIPCount(char ip1[], char ip2[]);
//分割IP地址存放到数组
void cutIPAddr(char[], int[]);
//判断IP地址是否合法
bool isValidIP(char[]);
//对接收的节点列表进行解析
void anaNodes(string buf, string IP);
//获取字符串中的IP部分
string nodeIP(string Nodestr);
//判断节点是否已经存在在列表中
bool isNewNode(string newNode);
//获取字符串中的端口部分
unsigned short nodePort(string Nodestr);
//获取字符串中的逻辑距离部分
string nodePublicKey(string Nodestr);
#endif // !NODEANALYSE_H_
