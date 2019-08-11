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
//����IP�߼�����
unsigned int getIPCount(char ip1[], char ip2[]);
//�ָ�IP��ַ��ŵ�����
void cutIPAddr(char[], int[]);
//�ж�IP��ַ�Ƿ�Ϸ�
bool isValidIP(char[]);
//�Խ��յĽڵ��б���н���
void anaNodes(string buf, string IP);
//��ȡ�ַ����е�IP����
string nodeIP(string Nodestr);
//�жϽڵ��Ƿ��Ѿ��������б���
bool isNewNode(string newNode);
//��ȡ�ַ����еĶ˿ڲ���
unsigned short nodePort(string Nodestr);
//��ȡ�ַ����е��߼����벿��
string nodePublicKey(string Nodestr);
#endif // !NODEANALYSE_H_
