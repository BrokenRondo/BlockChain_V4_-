#pragma once
#ifndef NODEHEARTGET_H_
#define NODEHEARTGET_H_
//客户端，用于请求节点心跳包
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>
#include "quorum.h"
#include "osapi/osapi.h"
#include "Global.h"
using namespace std;
extern unsigned short nodePort(std::string Nodestr);
extern std::string nodeIP(std::string Nodestr);
//控制线程
class NodeHeartGet :public OS_Thread
{
private:
	virtual int Routine() override;
};
#endif // !NODEHEARTGET_H_
