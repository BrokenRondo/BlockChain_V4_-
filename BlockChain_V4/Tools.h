#pragma once
#ifndef TOOLS_H_
#define TOOLS_H_
#include<string>
#include<iostream>
#include<fstream>
#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#include <IPHlpApi.h>
#include "Global.h"
#pragma comment(lib,"Iphlpapi.lib")
using namespace std;
extern string LocalIP;
//��ȡ��������IP
string getIP();
//��ȡʱ��
string gettime();
int gettime_minute();
int gettime_second();
int gettime_msecond();
#endif // !TOOLS_H_
