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
//区块链校检线程，用于检查区块链是否被篡改，周期为3分钟
class BlockChainCheck :public OS_Thread
{
private:
	virtual int Routine() override;
	//检验区块链完整性
	int verifyBlockChain();
	//申请从指定高度区块同步区块链
	int syncdata(int index);
};
#endif // !BLOCKCHAINCHECK_H_
