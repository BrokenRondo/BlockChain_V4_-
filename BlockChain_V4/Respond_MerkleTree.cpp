#include "Respond_MerkleTree.h"

extern std::string localIP;
extern merkletree* merkletree_global;

int Respond_MerkleTree::recv_check()
{

	//在收到信息的时候决定要做什么
	char buf_waiting[4096] = { 0 };

	local.SetIp(localIP.c_str());
	local.SetPortA(20080);
	sock.Open(local, true);


	//理论上来说，socket需要设置超时时间，不过暂时没设置
	while (true)
	{
		memset(buf_waiting, 0, 4096);
		int n = sock.RecvFrom(buf_waiting, 4096, peer);
		if (n < 0) {
			std::cout << "socket出现错误,验证响应线程（20040）" << endl;
			continue;
		}


		//解析同步时钟发来的数据
		if (!reader.parse(buf_waiting, root))
		{
			std::cout << "json解析错误！验证响应线程（20080）" << endl;
			continue;
		}
		const std::string type = root["type"].asString();//获取这个json的类型，类型应该是MN，表示通知M和N的时间点
		if (type != "PROOF")
		{
			std::cout << "验证请求异常，类型为" << type << ",验证响应线程（20080）" << endl;
			continue;
		}
		else
		{
			std::cout << "接收到主节点的同步消息，消息类型为" << type << endl;
		}
		break;
	}
	return 0;
}
int Respond_MerkleTree::Routine()
{
	std::cout << "验证线程开启" << endl;
	while (stopflag)
	{

		//监听收到的包，如果是不合法的包丢掉，如果是合法的包则验证
		recv_check();
		//解析出json后验证并返回结果
		int i = root["leaves"].size();
		//先设置type为commit，如果有不是commit的则更改
		respond["type"] = "COMMIT";
		for (int j = 0; j < i; j++)
		{
			//对每一个叶子，验证是否在默克尔树中，如果不在就添加到反馈的Json
			//先把proof要用的节点求出来
			std::string leaf = root["leaves"][j].asString();
			// get proof
			vector<ProofNode> proof = merkletree_global->proof(const_cast<char*>(leaf.c_str()));

			// verify proof
			bool verified = verifyProof(const_cast<char*>(leaf.c_str()), merkletree_global->root(), proof);
			cout << "叶子" << leaf << "验证状态为：" << verified << endl;
			if (!verified)
			{
				//如果验证失败则将这个叶子放进NOCOMMIT里
				Json::Value leaves;
				leaves = leaf;
				respond["NOTCOMMIT_LEAVES"].append(leaves);
				respond["type"] = "NOTCOMMIT";
			}
		}
		std::string respond_string = fwriter.write(respond);
		OS_UdpSocket sock;
		OS_SockAddr local(localIP.c_str(), 20041);
		sock.Open(local, true);
		sock.SendTo(respond_string.c_str(), 2048, peer);
		cout << "反馈验证已完成" << endl;
		sock.Close();
	}
	return 0;
}
