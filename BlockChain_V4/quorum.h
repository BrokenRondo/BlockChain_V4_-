#pragma once
#ifndef QUORUM_H_
#define QUORUM_H_
#include "Global.h"
#include <fstream>
#include "osapi/osapi.h"
using namespace std;
extern string nodeIP(string Nodestr);
extern int CountBlock();
//ÿ���ڵ㶼��Ҫһ���ٲþ����Ҹ��ٲþ���ֻ�е��ڵ����������仯ʱ�Ż���������
//ÿ���ڵ���ٲþ�����ܲ�һ��
//��һ�ι�ʶ��Ϻ󣬽ڵ��㲥��ǰ�߶Ⱥ�ǩ��
bool Create_matrix();//�����ٲþ����㷨
vector<unsigned int> factorization(unsigned int size);//�����ֽ�
vector<unsigned int> Get_hrd(vector<string> ipvector);//��ȡhrd,d=[0],h=[1],r=[2]
vector<string> Get_quorum();//��ȡ�ٲýڵ�
void BroadCastHeight(int H);//�㲥�߶�
vector<string> Get_MaxHeight_IP();//��ȡ��Ч�߶ȣ�������ͬ��IP
string Get_synIP(string IP);//��ȡͬ������IP
#endif // !QUORUM_H_
