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
/*0�����нڵ㿪ʼ���У����յĲ�����д��ʱ���
1��x = ���ڵ���n��n + mʱ����ڽ��յ��Ĳ�����Ϣ
2����һ��ʱ���ڣ�order�ڵ������з�����Ϣ��֪ͨ���n��n + mʱ����ڵĲ������㲥
3��for�ӵ�1����f + 1���׶�
��һ��
4��  �㲥value(x)
�ڶ���
5��	 if���յ���value(y)����n - f�Σ�then
6��	   �㲥propose(y)
7��	 end if
8:	 if���յ���propose(z)����f��then
9 :	 x = z
10 : end if
������
11�� �ڵ�vi��ȷ���ĵ�i�׶ε����ڵ�
12�� ���ڵ�㲥����ֵw
13�� if���յ���propose(x)�Ĵ����ϸ�����n - f then
14��   x = w
15�� end if
16��end for
*/
/*�ڽ��й�ʶ�Ĺ����У�Ϊ�˽�Լ����ͨ�ųɱ�ֻ����Ĭ�˶��������ǿ��ܴ��ڲ����ϵ����������
 * ���������е�ʱ���ٶȿ죬���ǳ��ִ���ʱҪ��������һ���ڵ㷢�־���ֵ���Լ��Ĳ�ͬ����
 * ��ʼ�ı����ֵ��Ӧ����֤�Լ��Ĳ����Ƿ�����Ĭ�˶����У�������ڣ������Լ�����Ҫ�洢����
 * ���ݣ���ôֻ��������ֵ��������ڣ�ͬʱ�����Լ�ȷʵ����Ҫ�洢�������������ݵĽڵ�ʱ�����ʱ��������
 * û�о�������ݣ���Ҫ�򱾴ι�ʶ������ȷ����ֵ�Ľڵ�ͬ���������ݣ�����в��ֲ��ڣ��Լ���Ҫ
 * �洢�������ݣ���ͬ����ͬʱ������������һ�ι�ʶ������в��ֲ��ڣ��Լ�����Ҫ�洢�������ݣ���
 * ��ͬ����ֻ�Ѳ���������һ�ι�ʶ
 */

 /*�ڽ��й�ʶ��ʱ���������ݰ�����json���䣬���ղ����Ľӿ��Ѿ�д���ˣ�ֻҪд����Ĭ�˶����͹�ʶ��һ
  * ���json�����е�TCP����Ҫ�÷ֽ���ֿ�
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
	std::vector<MerkleRoot_IP*>MerkleRoots1;//���������һ�ι㲥ֵ��IP����������Ĭ�˶���ʱ����
	int broadcast_value();
	int recv_and_change_state(char* state_str);//���������Լ�����
	int request_MerkleTree_proof(std::string IP);
	bool check_store();
	int sync_stored_thistime();
	int commit_value();//��ʵ��ͬ����֪ͨʱ��ͬ��������
	bool verify_include_record();//ͨ��Ĭ�˶�����鲡���Ƿ�������
};