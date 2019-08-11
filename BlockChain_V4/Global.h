#pragma once
#ifndef GLOBAL_H_
#define GLOBAL_H_
#include<string>
#include <vector>
#include "RSASIG.h"
using  namespace std;
//���ܻ��õ���ȫ�ֱ�����ȫ�ֺ���
//�����ڵ�����������󶨶˿�
#define PORT_NODELISTENING 10010
//���ղ����������󶨶˿�
#define PORT_CASERECIEVE 10020
//��������������󶨶˿�
#define PORT_BLOCKRECIEVE 10030
//ͬ���������������󶨶˿�
#define PORT_BLOCKCHAINSYN 10040
//�ܲ÷������󶨶˿� 
#define PORT_QUORUM 10050
//���ݿ�ϵͳIP
#define DB_IP "127.0.0.1"
//���ݿ�ϵͳ�û���
#define DB_USERNAME "root"
//���ݿ�ϵͳ����
#define DB_PASSWORD ""
//���ݿ�����
#define DB_NAME "blocksdata"
//��������С
#define BUF_SIZE 2048
//�ڵ����󣺼�����������
#define NODE_JOIN 0x01
//�ڵ����������ȡ�ڵ��б�
#define NODE_LIST 0x02
//�ڵ�������ȡ������
#define NODE_HEARTGET 0x03
//�ڵ����󣺼���������Ϊ
#define NODE_IN 0x04
//�ڵ�������Ӧ���ظ�������������
#define REPLY_JOIN 0x66
//�ڵ�������Ӧ�����������ظ�
#define REPLY_HEART 0x67
//�ڵ��б�·��
#define NODELIST_PATH "NodeList.txt"
//�������ݽ��ս���ָ��
#define LAST_DATA 0x11
//������ݽ��ս���ָ��
#define ALL_END 0x12
//������ݽ���δ����ָ��
#define CONTINUE_DATA 0x10
//��ȡ����������ָ��
#define GET_LEN 0x0A
//��������������IP��ַ
extern string LocalIP;
extern string localIP;
//���ڵ��IP��ַ
extern string MainIP;
//�����������ĸ�������1��ʼ����
extern unsigned int Blocknum;
//����������
extern string Commonpool;
//��������
extern string Privatepool;
//�ѽ��չ��Ĺ㲥�����Ĺ�����ϣֵ��
extern vector<string> Commonhashpool;
//�ѽ��չ�������˵�˽�й�ϣֵ��
extern vector<string> Privatehashpool;
//��������״̬
extern bool net_connect;
//��Կ��?
struct Public_Key
{
	string IP;
	string key;
};
extern vector<Public_Key>public_keys;
//�ڵ�������Կ
extern KeyPairHex keys;
//ǩ��
extern string signatures;
//����ҽԺ����
extern string localhospital;
struct MerkleRoot_IP
{
	std::string src_ip;
	std::string MerkleRoot;
};
extern unsigned int NodeNumber;
extern string trapoolstr;//������Ų������ַ�����json��ʽ������һ�����黹δ����֮ǰ��
extern vector<vector<vector<string>>> quorum_matrix;//���ɵ��ٲþ���
extern vector<vector<string>> height_set;//ÿ���߶Ⱥ���ڵ�Ըø߶ȵ�ǩ��
extern string SynIPSig;//�ٲýڵ㽨���ͬ������
wchar_t* Utf8_2_Unicode(string row_i);
extern vector<string> Nodes_vector;//�ڵ��б�
#endif // !GLOBAL_H_
