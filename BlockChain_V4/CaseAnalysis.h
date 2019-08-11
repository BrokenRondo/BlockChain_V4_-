#pragma once
#include "jsoncpp/json.h"
#include "osapi/osapi.h"
#include "Global.h"
#include "CaseBroadCast.h"
using namespace std;
//解析病例的线程
class CaseAnalysis :public OS_Thread
{
public:
	CaseAnalysis(string str);
private:
	//传入待解析的病历
	std::string case_str;
	virtual int Routine() override;
};