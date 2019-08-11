#pragma once
#ifndef DB_OPERATE_H_
#define DB_OPERATE_H_
#include <WinSock2.h>
#include <vector>
#include <iomanip>
#include <chrono>
#include <iostream>
#include<algorithm>
#include <regex>
#include<ctime>
#include <sstream>
#include "md5.h"
#include "mysql/include/mysql.h"
#include "jsoncpp/json.h"

//���ݿ�ϵͳIP
#define DB_IP "127.0.0.1"
//���ݿ�ϵͳ�û���
#define DB_USERNAME "root"
//���ݿ�ϵͳ����
#define DB_PASSWORD "521520"
//���ݿ�����
#define DB_NAME "blocksdata"
//�ж��ܷ��������ݿ�
bool ConnectDB();
//��ȡ������Ŀ
int CountBlock();
//������ӣ���ӱ��
int AddBlock(std::string hash, std::string time);
//��ȡĳһ�����ϣֵ
std::string PreHash(std::string blockname);
//��ĳһ��������Ӳ���
int AddCase(std::string blockname, std::string CaseJson);
//��ȡ�������ݣ�����Json��ʽ�ַ���
std::string ReadBlock(std::string blockname);
//���������
int DeleteBlock();
//��BlockJson�ַ���ת�����ݿ�
int WriteBlock(std::string Blockjson);
//�ҳ���������������ʱ��εĲ���
std::vector<std::string> FindCase(int m, int n);
//�ҳ�������������ʱ��εĲ���
std::vector<std::string> FindBlockCase(int m, int n, int index);
//��ʱ���תʱ���ʽ
std::string num2time(int n);
//�ҳ���������ָ��ʱ��Ĳ�����ϣ����
std::vector<std::string> FindTraspoolCase(int m, int n);
std::vector<char*> FindTraspoolCase_char(int m, int n);
int standard_to_stamp(char *str_time);//��׼ʱ��ת��Ϊunixʱ���
#endif // !DB_OPERATE_H_
