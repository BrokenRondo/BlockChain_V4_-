#include "Consensus.h"
#include <ctime>

#include <iomanip>
extern std::string localIP;
extern unsigned int NodeNumber;
extern std::string trapoolstr;
merkletree* merkletree_global;



int Consensus::broadcast_value()
{
	return 0;
}

int Consensus::recv_and_change_state(char* state_str)
{

	//在收到信息的时候决定要做什么
	while (true)
	{
		memset(buf_waiting, 0, 2048);
		int n = sock.RecvFrom(buf_waiting, 2048, peer);
		if (n < 0) {
			std::cout << "socket出现错误,共识算法线程（20000）" << state_str << "propose阶段" << endl;
			continue;
		}
		//state = recv_mn;
		//if (!(peer.GetIp_str() == MainIP))//如果不是同步时钟服务器发过来的
		//{
		//	std::cout << "未知来源，丢弃，共识算法线程（20000）" << state_str << "propose阶段" << endl;
		//}

		//解析同步时钟发来的数据
		if (!reader.parse(buf_waiting, root))
		{
			std::cout << "来自主节点的json解析错误！共识算法线程（20000）" << state_str << "propose阶段" << endl;
			continue;
		}
		const std::string type = root["type"].asString();//获取这个json的类型，类型应该是MN，表示通知M和N的时间点
		if (type != state_str)
		{
			if(type=="ROUND")
			{
				cout << "判定主节点出错" << endl;
				return 1;
			}
			std::cout << "来自主节点的通知异常，期望类型为" << state_str << "，实际类型为" << type << ",共识算法线程（20000）" << state_str << "propose阶段" << endl;
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

int Consensus::request_MerkleTree_proof(std::string IP)
{
	cout << "向节点：" << IP << "请求验证" << endl;
	Json::Value root;
	Json::FastWriter fwriter;
	Json::Reader reader;
	std::string Merkle_leaves_json;
	root["type"] = "PROOF";
	Json::Value leaves;
	std::vector<std::string>cases;//可以直接用case_time_mn代替
	//TODO 要初始化cases，cases为本轮原本要上传的公共缓冲区中的病历(暂时不需要) 

	for (int i = 0; i < cases_time_mn.size(); i++)
	{
		leaves = cases_time_mn[i];
		root["leaves"].append(leaves);
	}
	// ReSharper disable CppJoinDeclarationAndAssignment
	Merkle_leaves_json = fwriter.write(root);
	OS_UdpSocket socket;
	OS_SockAddr local(localIP.c_str(), 20020);
	OS_SockAddr peer(IP.c_str(), 20080);
	socket.Open(local);
	socket.SendTo(Merkle_leaves_json.c_str(), Merkle_leaves_json.length(), peer);
	char buf[4096] = { 0 };
	socket.RecvFrom(buf, 4096, peer);
	//TODO  解析是否都已经上传，如果没有则解析没上传的病历（已完成）

	cout << "收到验证的回复：" << buf << endl;
	if (!reader.parse(buf, root))
	{
		cout << "json解析失败，无法验证病历是否上传，按照未上传处理" << endl;
		return -1;
	}
	if (root["type"].asString() == "COMMIT")
	{
		cout << "病历全部上传，验证是否需要存储本区块" << endl;//如果病历都上传了，说明是自己少存了病历，然后判断需不需要存储，如果不需要就直接保存root，不再同步
		if (check_store())
		{
			cout << "需要存储，开始同步" << endl;
			//TODO 同步具体病历，需要等待10s左右再同步，防止有的节点没有更新到最新值，同步时需要用仲裁系统来判断最新值是多少


		}
		else
		{
			cout << "不需要存储病历，跳过同步过程" << endl;
		}
	}
	else
	{
		//解析出没有成功上传的病历重新上传,在上传时记得修改病历的上传时间
		//从json中解析出未上传的数组
		vector<int>index_to_delete;
		Json::Value value;
		Json::Value delete_value;
		Json::Reader reader_add;
		Json::StyledWriter swriter;
		Json::Value trapoolstr_json;
		int i = root["NOTCOMMIT_LEAVES"].size();
		for (int j = 0; j < i; j++)
		{
			std::string starttime = num2time(time_m);
			std::string endtime = num2time(time_n);
			//每找到一个MD5就去检查病历池中有没有病历的MD5值对应

			if (!reader_add.parse(trapoolstr, value) && !trapoolstr.empty())
				printf("病历池Json解析错误\n");
			//遍历病历池获取md5值
			int size = value.size();
			vector<CaseBroadcast*>BroadCast_threads;
			bool deleteflag = true;
			for (int i = 0; i < size; i++)
			{
				Json::Value jsontemp;
				jsontemp = value[i];
				//判断是否满足时间段
				if (standard_to_stamp( (char*)jsontemp["time"].asString().c_str()) > time_n ||standard_to_stamp( (char*)jsontemp["time"].asString().c_str()) < time_m)
					continue;
				Json::StreamWriterBuilder writer;
				std::unique_ptr<Json::StreamWriter> write(writer.newStreamWriter());
				std::ostringstream os;
				write->write(jsontemp, &os);
				write->~StreamWriter();
				std::string Casejson = os.str();
				//已经获取遍历，取md5值
				MD5* md5 = new  MD5(Casejson);
				std::string md5str = md5->hexdigest();

				if (root["NOTCOMMIT_LEAVES"][j].asString() == md5str)
				{
					//如果在里面就修改时间为目前时间,可以用两个时间一个标记上传时间一个标记书写时间
					Json::Value casejson;
					Json::Reader casejson_reader;
					if (!casejson_reader.parse(Casejson,casejson))
					{
						cerr << "casejson解析失败" << endl;
						continue;
					}
					deleteflag = false;
					std::time_t t = std::time(0);//获取系统当前时间
					value[i]["time"] = num2time( static_cast<int>(t));
					//然后重新广播病历，只有是原来自己缓冲区中的病历才重新广播
					if (casejson["hospital"].asString() != localhospital)
					{
						continue;
					}
					Json::Value broad_again;
					broad_again = value[i];
					std::string broad_again_str = swriter.write(value[i]);
					CaseBroadcast *c_broadcast = new CaseBroadcast(broad_again_str);
					cout << "重新广播的病历"<<broad_again_str << endl;
					//把线程放进vector中，方便后续释放内存
					BroadCast_threads.push_back(c_broadcast);
					c_broadcast->Run();
					trapoolstr_json.append(broad_again);

				}
				else
				{
					index_to_delete.push_back(i);
				}
			}
			//如果不在没确认的里面就标记这个病历，在后面删除这些病历
			if (deleteflag)
			{
				
			}
			//在for循环后要删除广播的线程

			Msleep(300);
			for (int i = 0; i < BroadCast_threads.size(); i++)
			{
				delete BroadCast_threads[i];
			}
			deleteflag = true;
		}
		//结束循环后没确认的所有病例都找到了，没在没确认表里的病历全部删掉
		//删又删不掉，只能重新再弄一个json
		//for (int i = index_to_delete.size() - 1; i >= 0; i--)//从最后一个删起
		//{
		//	//value.removeIndex(index_to_delete[i],NULL);
		//	//value[index_to_delete[i]] = Json::Value::null;
		//}
		//最后把修改完成的数据写回去
		trapoolstr = swriter.write(trapoolstr_json);
		cout << "病历缓冲区：" << trapoolstr << endl;
		//TODO 写回去的未上传的数据还要继续广播一遍////////////(已完成，164~174行）


	}
	return 0;
}

bool Consensus::check_store()
{
	return true;
}






int Consensus::Routine()
{
	char buf_waiting[1024] = { 0 };//这个buf用来接收同步时钟发来的m和n时间点mn.以及之后的同步消息
	local.SetIp(localIP.c_str());
	local.SetPortA(20000);
	sock.Open(local, true);
	int mainNodeFailure = 0;
	std::string value_x;
	std::string merkleTree_root;
	state = waiting;
	std::cout << "共识算法线程开启（20000）" << endl;
	//共识算法线程开启后需要监听网络中的数据，并且需要依据同步时钟的数据转变状态
	//最开始的状态是waiting状态
	while (true)
	{
		int n = sock.RecvFrom(buf_waiting, 1024, peer);
		NodeNumber = Nodes_vector.size()-1;
		if (n < 0) {
			std::cout << "接收到超过长度的数据,共识算法线程（20000）" << endl;
			continue;
		}
		//sock.Close();
		state = recv_mn;
		if (!(peer.GetIp_str() == MainIP))//如果不是同步时钟服务器发过来的
		{
			std::cout << "未知来源，来源为" << peer.GetIp_str() << "，丢弃，共识算法线程（20000）" << endl;
			continue;
		}

		//解析同步时钟发来的数据
		if (!reader.parse(buf_waiting, root))
		{
			std::cout << "来自主节点的json解析错误！共识算法线程（20000）";
			continue;
		}
		const std::string type = root["type"].asString();//获取这个json的类型，类型应该是MN，表示通知M和N的时间点
		if (type != "MN")
		{
			std::cout << "来自主节点的通知无效，类型为" << type << ",共识算法线程（20000）" << endl;
			continue;
		}
		//如果类型正确，则获取M和N分别是多少
		//M和N应该是整形的
		std::time_t t = std::time(0);//获取系统当前时间
		std::cout << "当前unix时间戳为" << t << endl;

		time_m = root["TIME_M"].asUInt();
		time_n = root["TIME_N"].asUInt();
		//获取到M和N后准备进入第一阶段
		//首先要把这个时间段内的病历打包好，然后计算默克尔树

		//TODO  查找符合时间段内的病历，然后按照时间戳把病历排序(下一行已完成）
		cases_time_mn = FindTraspoolCase_char(time_m, time_n);
		//TODO  计算默克尔树(已完成）
		//如果是空的，也就是这个时间段内没病历，就要直接给出空字符串的hash值，而不要计算默克尔树
		if (!cases_time_mn.empty())
		{
			//如果不空就计算默克尔树
			/*vector<char*>cases_t;
			for (int i=0;i<cases_time_mn.size();i++)
			{
				char *buf = new char[256];
				buf = (char*)cases_time_mn[i].c_str();
				cases_t.push_back(buf);
			}*/
			merkletree* mtree = new merkletree(cases_time_mn);
			merkletree_global = mtree;
			char* MerkleTreeRoot = mtree->root();
			merkleTree_root = MerkleTreeRoot;
		}
		else
		{
			merkleTree_root = "00000000000000000000000000000000";
		}
		//TODO 计算默克尔树之前要把每个病历以及它对应的MD5放入vector中（已完成) 
		cout << "计算出的merkleroot:" << merkleTree_root << endl;
		value_x = merkleTree_root;
		int Consensus_times = 0;
		while (true) //这个循环控制f+1阶段的共识
		{
			//首先要等待同步时钟的通知,ROUND表示,另一个类型Start或者Stop控制是否跳出
			if (mainNodeFailure==0)
			{
				recv_and_change_state((char*)"ROUND");//上一轮leader没出错才等待ROUND的同步，否则直接跳过去
			}
			mainNodeFailure = 0;
			if (root["State"].asString() == "STOP")
			{
				cout << "本阶段共识算法结束，共" << Consensus_times << "轮" << endl;
				cout << "最终决策值为" << value_x << endl;;
				break;
			}
			else
			{
				cout << "本次共识算法第" << Consensus_times << "次开始" << endl;
			}
			Consensus_times++;
			
			//然后广播自己计算出的默克尔树根（在发现错误的时候再请求默克尔树）
			state = ready_to_broadcast;

			//TODO 不能直接广播merkleTree_root,需要整合成json（已完成） 
			Json::Value merkletree_root_json;
			Json::FastWriter f_Writer;
			merkletree_root_json["type"] = "MERKLEROOT";
			merkletree_root_json["VALUE"] = value_x;
			std::string merkleTree_root_1 = f_Writer.write(merkletree_root_json);


			//进入接收第一轮广播值的状态
			state = recv_1;
			//首先要开启接收线程，然后才能开启广播线程，而且广播线程开启时间要稍微晚一点
			Recv_thread *recv_thread = new Recv_thread();
			recv_thread->Run();

			//OS_Thread::Sleep(2);//2秒以后开启广播，为了避免加大网络负载，随机退避0~5秒
			srand((int)time(nullptr));
			int random_time = rand() % 1000 + 1000;//1~3s///////////////////////////////////////////////////////////////////////////////注意这个休眠应该放在广播的线程里，要改
			//OS_Thread::Msleep(random_time);
			cout << "接收线程开启" << endl;
			Broadcast_thread* broadcast_thread = new Broadcast_thread(merkleTree_root_1, 20011, random_time);
			broadcast_thread->Run();//广播自己计算出的默克尔根
			//然后统计接收到的value个数，如果返回值是-1说明重复最多的个数不足n-f次，如果返回值不是-1则说明至少n-f次，并且hash为返回值
			OS_Thread::Msleep(2*1000+250);//休眠3秒，等所有数据传送都结束，然后统计
			recv_thread->stop();

			std::string max_value_hash = recv_thread->calculate_max_value();
			MerkleRoots = recv_thread->MerkleRoots;
			if(Consensus_times==1)
			{
				MerkleRoots1 = MerkleRoots;
			}
			/*MerkleRoots.assign(recv_thread->MerkleRoots._Unchecked_begin(),recv_thread->MerkleRoots._Unchecked_end());*/
			delete recv_thread;
			std::string propose_value_1;
			bool is_max_value_hash_integer;
			//try
			//{
			//	int x=stoi(max_value_hash);
			//	is_max_value_hash_integer = true;
			//}
			//catch (std::invalid_argument&)
			//{
			//	is_max_value_hash_integer = false;
			//}
			if (max_value_hash.length() > 31)
			{
				is_max_value_hash_integer = false;
			}
			else
			{
				is_max_value_hash_integer = true;
			}
			if (Consensus_times == 1)
			{
				cout << endl;
			}
			if (is_max_value_hash_integer)//如果是整数
			{
				std::cout << "第一轮出现严格小于n-f的情况，转变状态，但是不广播propose" << endl;
				state = no_propose;
			}
			else
			{
				std::cout << "第一轮满足至少接收到value(y) n-f次，准备propose(y)" << endl;
				state = propose_1;
				propose_value_1 = max_value_hash;
			}
			//接下来进入第一次propose阶段

			//TODO 将propose1的值打包成json（已完成) 
			Json::Value propose1;
			Json::FastWriter fwriter_propose1;
			propose1["type"] = "MERKLEROOT";
			propose1["VALUE"] = propose_value_1;
			propose_value_1 = fwriter_propose1.write(propose1);
			//等待同步通知，开始propose第一次
			//TODO 要设置超时时间（暂时不做） 
			//以下可以调用recv_changeState函数/////////////////////////////////
			recv_and_change_state((char*)"PROPOSE");
			//接收到同步通知后先开启接收线程，再随机退避后开启发送线程
			Recv_thread *recv_thread_propose = new Recv_thread();
			recv_thread_propose->Run();
			//等待一段时间后再开始广播，广播前要做随机退避0~2s
			random_time = rand() % 1000 + 1000;
			//OS_Thread::Msleep(random_time);
			//然后开始广播
			//要根据状态来判断是否要propose
			if (state == propose_1)
			{
				broadcast_thread->setMessage(const_cast<char*>(propose_value_1.c_str()));/////这个目前还不知道正不正确
				broadcast_thread->setPort(20011);
				broadcast_thread->setMsleep(random_time);
				broadcast_thread->Run();
			}
			else if (state == no_propose)
			{
				//如果不准备广播的话
				//OS_Thread::Sleep(2);
			}
			//休眠10s，让所有的广播都结束
			OS_Thread::Msleep(2050);
			//然后根据propose的结果决定是否执行算法第9行
			recv_thread_propose->stop();

			std::string propose_z = recv_thread_propose->calculate_maxvalue_proposez();
			int propose_z_times = recv_thread_propose->get_propose_z_times();//接收到propose_z的最大次数
			//try
			//{
			//	int x = stoi(propose_z);
			//	is_max_value_hash_integer = true;
			//}
			//catch (std::invalid_argument&)
			//{
			//	is_max_value_hash_integer = false;
			//}
			if (propose_z.length() > 31)
			{
				is_max_value_hash_integer = false;
			}
			else
			{
				is_max_value_hash_integer = true;
			}
			if (!is_max_value_hash_integer)
			{
				std::cout << "接收到的propose(z)至少f次，";
				if (propose_z == merkleTree_root)
				{
					std::cout << "z值与自身x相同" << endl;
				}
				else
				{
					std::cout << "z与x不同，执行算法x=z" << endl;
					value_x = propose_z;
				}
			}
			else
			{
				std::cout << "接收到propose(z)" << propose_z << "次，不足f次" << endl;
			}
			//接下来进入第三轮，先判断主节点，然后决定是否改变值
			//以下可以调用recv_changeState函数/////////////////////////////////
			recv_and_change_state((char*)"MAINNODE");
			
			//已经获取主节点通知的json,接下来解析主节点
			std::string Main_IP = root["MAINIP"].asString();
			if (Main_IP == localIP)
			{
				std::cout << "本阶段主节点为自己" << endl;
				state = mainNode_broad;
			}
			else
			{
				std::cout << "本阶段主节点为" << Main_IP << endl;
				state = listening_mainNode;
			}
			//接下来决定是广播自身值还是接收主节点的值
			if (state == listening_mainNode)
			{
				//如果是接收主节点的值的话，要判断IP
				root.clear();
				mainNodeFailure = recv_and_change_state((char*)"MAINNODE_VALUE");
				if (mainNodeFailure == 1)
				{

					continue;
				}
				//已经接收到主节点的值w，接下来判断是否改变值
				if (propose_z_times < NodeNumber - (NodeNumber - 1) / 3)//如果严格小于n-f
				{
					std::cout << "Propose次数严格小于n-f,次数为" << propose_z_times << ",x=w" << endl;
					cout << "主节点广播的值w为" << root["VALUE"].asString() << endl;
					value_x = root["VALUE"].asString();
				}
				else
				{
					cout << "propose次数为" << propose_z << "，不改变输入值" << endl;
				}
			}
			if (state == mainNode_broad)
			{
				//如果自己是主节点，就广播自己的值(广播的值应该是自己最初的值）
				std::string MainNode_value_w;
				//TODO 将MerkleTree_root打包成json(已完成） 
				Json::Value mainNode;
				Json::FastWriter fast_writer;
				mainNode["type"] = "MAINNODE_VALUE";
				mainNode["VALUE"] = value_x;
				MainNode_value_w = fast_writer.write(mainNode);
				//要先等待一下在广播自己的值，确保所有的节点都开始接收
				//Sleep(3);
				cout << "主节点开始广播：" << MainNode_value_w << endl;
				broadcast_thread->setMessage(const_cast<char*>(MainNode_value_w.c_str()));
				broadcast_thread->setPort(20000);
				broadcast_thread->setMsleep(400);
				broadcast_thread->Run();
			}
			OS_Thread::Msleep(550);
			delete broadcast_thread;
			delete recv_thread_propose;
			cout << "共识算法中一次循环结束，目前的输入值为x=" << value_x << endl;
		}
		//一轮共识算法结束，接下来对比是否与自己的决策值一样，进行后续操作
		if (value_x == merkleTree_root)
		{
			//如果决策值和最后的值相等直接结束本轮共识，把区块存入数据库
			cout << "决策值和自己的值相同，完成共识算法，共识算法线程(20000)" << endl;
			//TODO 将病历池上传到数据库


		}
		else
		{
			//如果决策值不同，则向第一次广播就正确的节点请求默克尔树
			cout << "决策值和自己的值不同，开始获取默克尔树" << endl;
			std::string Ask_IP;//准备请求默克尔树的IP

			for (int i = 0; i < MerkleRoots1.size(); i++)
			{
				if (MerkleRoots1[i]->MerkleRoot == value_x)
				{
					Ask_IP = MerkleRoots1[i]->src_ip;
					break;
				}
			}
			//找到Ask_IP后请求证明，要求验证自己的病历是否都已经上传，即是否都能被证明

			//验证上传和如果没上传的都在request_MerkleTree_proof里处理
			request_MerkleTree_proof(Ask_IP);


		}
		//sock.Open(local, true);

	}
}
