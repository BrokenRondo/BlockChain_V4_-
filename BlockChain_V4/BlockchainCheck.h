#pragma once
#ifndef BLOCKCHAINCHECK_H_
#define BLOCKCHAINCHECK_H_
#include <sstream>
#include <fstream>
#include "quorum.h"
#include "sha256.h"
#include "DB_operate.h"
#include "BlockchainGet.h"
using namespace std;
extern string nodeIP(string Nodestr);
//������У���̣߳����ڼ���������Ƿ񱻴۸ģ�����Ϊ3����
class BlockChainCheck :public OS_Thread
{
private:
	virtual int Routine() override;
	//����������������
	int verifyBlockChain();
	//�����ָ���߶�����ͬ��������
	int syncdata(int index);
};
#endif // !BLOCKCHAINCHECK_H_
