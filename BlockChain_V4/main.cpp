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
	//测试用，把固定的病历读入tranpoolstr
	ifstream fin("./trapoolstr.txt");
	stringstream buffer;
	buffer << fin.rdbuf();
	std::string content(buffer.str());
	trapoolstr = content;
	cout << "临时病历：" << trapoolstr << endl;
	cout << "正在识别本机IP....";
	LocalIP="192.168.100.133";
	localIP = LocalIP;
	MainIP ="192.168.100.140";
	std::cout << "本机IP为" << LocalIP << endl;
	Nodes_vector.push_back(LocalIP);
	ofstream fout(NODELIST_PATH);
	fout << std::left << setw(20) << LocalIP+":" << setw(8) <<PORT_NODELISTENING<< to_string(0) << endl;
	fout.close();
	/*if (!ConnectDB())
	{
		cout << "无法连接区块链数据库\n";
		return 0;
	}*/
	//——————节点——————
	Node node = Node(MainIP);
	node.setNetEnvironment();
	//非创世节点向创世节点连接
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
	//节点网络监听线程
	NetListening* netlistening_server = new NetListening(LocalIP,PORT_NODELISTENING);
	node.netlistening_start(*netlistening_server);
	//心跳节点索取线程
	/////NodeHeartGet* nodeheartget_server = new NodeHeartGet();
	//开启心跳包索取线程
	////node.nodeheartget_start(*nodeheartget_server);

	////OS_Thread::Sleep(2);
	//——————病历————————
	CaseRecv* caserecv_server = new CaseRecv(LocalIP,PORT_CASERECIEVE);
	caserecv_server->Run();

	////OS_Thread::Sleep(2);
	//——————区块链———————
	////BlockchainSyn *blockchainsyn_server = new BlockchainSyn(LocalIP,PORT_BLOCKCHAINSYN);
	////blockchainsyn_server->Run();
	//BlockRecv *blockrecv_server = new BlockRecv(LocalIP,PORT_BLOCKRECIEVE);
	//blockrecv_server->Run();
	//BlockPack *blockpack_thread = new BlockPack();
	//blockpack_thread->Run();
	//——————共识————————
	Consensus * consensus = new Consensus();
	consensus->Run();
	Respond_MerkleTree *respond_merkle = new Respond_MerkleTree();
	respond_merkle->Run();
	//Sync_time* sync_time = new Sync_time();
	//sync_time->Run();


	//————————仲裁服务器————————
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