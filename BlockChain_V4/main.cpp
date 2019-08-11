#include <iostream>
#include "Node.h"
#include "DB_operate.h"
#include "Tools.h"
#include "CaseRecv.h"
#include "Blockchain.h"
#include "Consensus.h"
#include "QuorumServer.h"
#include "quorum.h"
#include "Sync_time.h"
#include "Respond_MerkleTree.h"
extern string MainIP;
extern string LocalIP;
using namespace std;
int main()
{
	//DeleteBlock();
	//NodeNumber = 16;
	//�����ã��ѹ̶��Ĳ�������tranpoolstr
	ifstream fin("./trapoolstr.txt");
	stringstream buffer;
	buffer << fin.rdbuf();
	std::string content(buffer.str());
	trapoolstr = content;
	cout << "��ʱ������" << trapoolstr << endl;
	cout << "����ʶ�𱾻�IP....";
	LocalIP="192.168.100.133";
	localIP = LocalIP;
	MainIP ="192.168.100.140";
	std::cout << "����IPΪ" << LocalIP << endl;
	Nodes_vector.push_back(LocalIP);
	ofstream fout(NODELIST_PATH);
	fout << std::left << setw(20) << LocalIP+":" << setw(8) <<PORT_NODELISTENING<< to_string(0) << endl;
	fout.close();
	/*if (!ConnectDB())
	{
		cout << "�޷��������������ݿ�\n";
		return 0;
	}*/
	//�������������ڵ㡪����������
	Node node = Node(MainIP);
	node.setNetEnvironment();
	//�Ǵ����ڵ������ڵ�����
	if (LocalIP != MainIP)
	{
		if (-1 == node.LoginNet())
			return -1;
		node.getP2PNodes();
		node.SendToList();
	}
	string command = "index";
	command += to_string(0);
	//SynClient* synclient = new SynClient("192.168.1.102", command);
	//synclient->startsyn();
	//�ڵ���������߳�
	NetListening* netlistening_server = new NetListening(LocalIP,PORT_NODELISTENING);
	node.netlistening_start(*netlistening_server);
	//�����ڵ���ȡ�߳�
	/////NodeHeartGet* nodeheartget_server = new NodeHeartGet();
	//������������ȡ�߳�
	////node.nodeheartget_start(*nodeheartget_server);

	////OS_Thread::Sleep(2);
	//��������������������������������
	CaseRecv* caserecv_server = new CaseRecv(LocalIP,PORT_CASERECIEVE);
	caserecv_server->Run();

	////OS_Thread::Sleep(2);
	//��������������������������������
	////BlockchainSyn *blockchainsyn_server = new BlockchainSyn(LocalIP,PORT_BLOCKCHAINSYN);
	////blockchainsyn_server->Run();
	//BlockRecv *blockrecv_server = new BlockRecv(LocalIP,PORT_BLOCKRECIEVE);
	//blockrecv_server->Run();
	//BlockPack *blockpack_thread = new BlockPack();
	//blockpack_thread->Run();
	//��������������ʶ����������������
	Consensus * consensus = new Consensus();
	consensus->Run();
	Respond_MerkleTree *respond_merkle = new Respond_MerkleTree();
	respond_merkle->Run();
	//Sync_time* sync_time = new Sync_time();
	//sync_time->Run();


	//�����������������ٲ÷���������������������
	////OS_Thread::Sleep(15);
	////Create_matrix();
	////BlockChainCheck *blockchaincheck_thread = new BlockChainCheck();
	////blockchaincheck_thread->Run();
	////vector<string> quorunnode = Get_quorum();
	////QuorumServer* quorumserver = new QuorumServer(LocalIP,PORT_QUORUM);
	////quorumserver->Run();
	////OS_Thread::Sleep(30);
	////int H = 12;// CountBlock();
	////BroadCastHeight(H);
	getchar();
	return 0;
}