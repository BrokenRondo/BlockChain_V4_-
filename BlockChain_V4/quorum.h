#pragma once
#ifndef QUORUM_H_
#define QUORUM_H_
#include "Global.h"
#include <fstream>
#include "osapi/osapi.h"
using namespace std;
extern string nodeIP(string Nodestr);
extern int CountBlock();
//每个节点都需要一个仲裁矩阵，且该仲裁矩阵只有当节点数量发生变化时才会重新生成
//每个节点的仲裁矩阵可能不一样
//当一次共识完毕后，节点会广播当前高度和签名
bool Create_matrix();//生成仲裁矩阵算法
vector<unsigned int> factorization(unsigned int size);//因数分解
vector<unsigned int> Get_hrd(vector<string> ipvector);//获取hrd,d=[0],h=[1],r=[2]
vector<string> Get_quorum();//获取仲裁节点
void BroadCastHeight(int H);//广播高度
vector<string> Get_MaxHeight_IP();//获取有效高度，并返回同步IP
string Get_synIP(string IP);//获取同步对象IP
#endif // !QUORUM_H_
