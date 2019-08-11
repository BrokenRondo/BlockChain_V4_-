#pragma once
#ifndef BLOCKWRITEDB_H_
#define BLOCKWRITEDB_H_
#include "osapi/osapi.h"
#include "DB_operate.h"
#include <iostream>
#include <fstream>
//¿ØÖÆÏß³Ì
class BlockWriteDB :public OS_Thread
{
private:
	virtual int Routine() override;
};
#endif // !BLOCKWRITEDB_H_
