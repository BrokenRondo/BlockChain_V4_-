#pragma once
#include "osapi/osapi.h"
#include "jsoncpp/json.h"
#include "Global.h"
#include "Broadcast_thread.h"
#include "Recv_thread.h"
#include "MerkleTree/merkletree.h"
#include "CaseBroadCast.h"
#include "DB_operate.h"
#include <ctime>
#include <sstream>
#include <random>
#include "md5.h"
#include <string>
#include<iostream>
using namespace std;
/*0：所有节点开始运行，接收的病历上写上时间戳
1：x = 本节点在n到n + m时间段内接收到的病历信息
2：在一段时间内，order节点向所有发送消息，通知打包n到n + m时间段内的病历并广播
3：for从第1到第f + 1个阶段
第一轮
4：  广播value(x)
第二轮
5：	 if接收到的value(y)至少n - f次，then
6：	   广播propose(y)
7：	 end if
8:	 if接收到的propose(z)至少f次then
9 :	 x = z
10 : end if
第三轮
11： 节点vi是确定的第i阶段的主节点
12： 主节点广播它的值w
13： if接收到的propose(x)的次数严格少于n - f then
14：   x = w
15： end if
16：end for
*/
/*在进行共识的过程中，为了节约网络通信成本只发送默克尔树，但是可能存在不符合的情况，所以
 * 当正常运行的时候速度快，但是出现错误时要处理。例如一个节点发现决策值和自己的不同，并
 * 开始改变决策值，应该验证自己的病历是否已在默克尔树中，如果都在，并且自己不需要存储具体
 * 数据，那么只保留决策值，如果都在，同时发现自己确实是需要存储这个区块具体数据的节点时，这个时候他本身
 * 没有具体的数据，需要向本次共识中有正确决策值的节点同步具体数据，如果有部分不在，自己需要
 * 存储具体数据，则同步，同时将病历放入下一次共识，如果有部分不在，自己不需要存储具体数据，则
 * 不同步，只把病历放入下一次共识
 */

 /*在进行共识的时候所有数据包都用json传输，接收病历的接口已经写好了，只要写发送默克尔树和共识这一
  * 块的json。所有的TCP都需要用分界符分开
  */

class Consensus :public OS_Thread
{
private:
	enum State { waiting, recv_mn, ready_to_broadcast, recv_1, propose_1, no_propose, propose_2, change_propose, mainNode_broad, listening_mainNode };
	unsigned int time_m, time_n;
	bool mainNode = false;
	unsigned int recv_num = 0;
	virtual int Routine() override;
	OS_UdpSocket sock;
	OS_SockAddr local;
	OS_SockAddr peer;
	std::string buf;
	char buf_waiting[2048] = { 0 };
	State state = waiting;
	Json::Value root;
	Json::Reader reader;
	std::vector<char* >cases_time_mn;


public:
	std::vector<MerkleRoot_IP*>MerkleRoots;
	std::vector<MerkleRoot_IP*>MerkleRoots1;//用来保存第一次广播值的IP，方便请求默克尔树时查找
	int broadcast_value();
	int recv_and_change_state(char* state_str);//用来接收以及解析
	int request_MerkleTree_proof(std::string IP);
	bool check_store();
	int sync_stored_thistime();
	int commit_value();//在实现同步后通知时间同步服务器
	bool verify_include_record();//通过默克尔树检查病历是否在里面
};