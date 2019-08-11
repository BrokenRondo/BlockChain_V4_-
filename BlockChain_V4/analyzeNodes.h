#pragma once 
#ifndef ANALYZENODES_H
#define ANALYZENODES_H
#include<string>
#include<vector>
#include<fstream>
#include <io.h>
#include<list>
#include<iomanip>
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
//��ȡ�ַ����е��߼����벿�֣�ֱ�ӷ���0
unsigned int nodeDis(string Nodestr);
//��ȡ�ַ����е��߼����벿��
string nodePublicKey(string Nodestr);
//��ȡ�ļ������ļ���Ŀ
int dir(string path);
#endif
