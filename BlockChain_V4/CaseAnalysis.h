#pragma once
#include "jsoncpp/json.h"
#include "osapi/osapi.h"
#include "Global.h"
#include "CaseBroadCast.h"
using namespace std;
//�����������߳�
class CaseAnalysis :public OS_Thread
{
public:
	CaseAnalysis(string str);
private:
	//����������Ĳ���
	std::string case_str;
	virtual int Routine() override;
};