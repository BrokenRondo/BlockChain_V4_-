#pragma once
#ifndef CASEBROADCAST_H_
#define CASEBROADCAST_H_
#include <iostream>
#include <vector>
#include <fstream>
#include "osapi/osapi.h"
#include "Global.h"
using namespace std;
extern std::string nodeIP(std::string Nodestr);
//转发病例的线程
class CaseBroadcast :public OS_Thread
{
public:
	CaseBroadcast(string case_str) { this->case_str = case_str; }
private:
	//待转发的病历
	std::string case_str;
	virtual int Routine() override;
};
#endif // !CASEBROADCAST_H_
