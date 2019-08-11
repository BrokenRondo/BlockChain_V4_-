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

//数据库系统IP
#define DB_IP "127.0.0.1"
//数据库系统用户名
#define DB_USERNAME "root"
//数据库系统密码
#define DB_PASSWORD "521520"
//数据库名称
#define DB_NAME "blocksdata"
//判断能否连接数据库
bool ConnectDB();
//获取区块数目
int CountBlock();
//区块添加（添加表格）
int AddBlock(std::string hash, std::string time);
//获取某一区块哈希值
std::string PreHash(std::string blockname);
//向某一区块中添加病历
int AddCase(std::string blockname, std::string CaseJson);
//获取区块内容，返回Json形式字符串
std::string ReadBlock(std::string blockname);
//清空区块链
int DeleteBlock();
//将BlockJson字符串转入数据库
int WriteBlock(std::string Blockjson);
//找出所有区块内满足时间段的病例
std::vector<std::string> FindCase(int m, int n);
//找出单个区块满足时间段的病例
std::vector<std::string> FindBlockCase(int m, int n, int index);
//将时间戳转时间格式
std::string num2time(int n);
//找出病历池中指定时间的病历哈希数组
std::vector<std::string> FindTraspoolCase(int m, int n);
std::vector<char*> FindTraspoolCase_char(int m, int n);
int standard_to_stamp(char *str_time);//标准时间转化为unix时间戳
#endif // !DB_OPERATE_H_
