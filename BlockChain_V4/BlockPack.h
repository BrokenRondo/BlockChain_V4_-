#pragma once
#ifndef BLOCKPACK_H_
#define BLOCKPACK_H_
//�̣߳����ڽ��������еĲ������������
#include <sstream>
#include <fstream>
#include "osapi/osapi.h"
#include "DB_operate.h"
#include "Tools.h"
#include "sha256.h"
using namespace std;
extern string nodeIP(string Nodestr);
class BlockPack :public OS_Thread
{
private:
	virtual int Routine() override;
};
#endif // !BLOCKPACK_H_
