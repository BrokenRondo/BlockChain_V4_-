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

	//���յ���Ϣ��ʱ�����Ҫ��ʲô
	while (true)
	{
		memset(buf_waiting, 0, 2048);
		int n = sock.RecvFrom(buf_waiting, 2048, peer);
		if (n < 0) {
			std::cout << "socket���ִ���,��ʶ�㷨�̣߳�20000��" << state_str << "propose�׶�" << endl;
			continue;
		}
		//state = recv_mn;
		//if (!(peer.GetIp_str() == MainIP))//�������ͬ��ʱ�ӷ�������������
		//{
		//	std::cout << "δ֪��Դ����������ʶ�㷨�̣߳�20000��" << state_str << "propose�׶�" << endl;
		//}

		//����ͬ��ʱ�ӷ���������
		if (!reader.parse(buf_waiting, root))
		{
			std::cout << "�������ڵ��json�������󣡹�ʶ�㷨�̣߳�20000��" << state_str << "propose�׶�" << endl;
			continue;
		}
		const std::string type = root["type"].asString();//��ȡ���json�����ͣ�����Ӧ����MN����ʾ֪ͨM��N��ʱ���
		if (type != state_str)
		{
			if(type=="ROUND")
			{
				cout << "�ж����ڵ����" << endl;
				return 1;
			}
			std::cout << "�������ڵ��֪ͨ�쳣����������Ϊ" << state_str << "��ʵ������Ϊ" << type << ",��ʶ�㷨�̣߳�20000��" << state_str << "propose�׶�" << endl;
			continue;
		}
		else
		{
			std::cout << "���յ����ڵ��ͬ����Ϣ����Ϣ����Ϊ" << type << endl;
		}
		break;
	}
	return 0;
}

int Consensus::request_MerkleTree_proof(std::string IP)
{
	cout << "��ڵ㣺" << IP << "������֤" << endl;
	Json::Value root;
	Json::FastWriter fwriter;
	Json::Reader reader;
	std::string Merkle_leaves_json;
	root["type"] = "PROOF";
	Json::Value leaves;
	std::vector<std::string>cases;//����ֱ����case_time_mn����
	//TODO Ҫ��ʼ��cases��casesΪ����ԭ��Ҫ�ϴ��Ĺ����������еĲ���(��ʱ����Ҫ) 

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
	//TODO  �����Ƿ��Ѿ��ϴ������û�������û�ϴ��Ĳ���������ɣ�

	cout << "�յ���֤�Ļظ���" << buf << endl;
	if (!reader.parse(buf, root))
	{
		cout << "json����ʧ�ܣ��޷���֤�����Ƿ��ϴ�������δ�ϴ�����" << endl;
		return -1;
	}
	if (root["type"].asString() == "COMMIT")
	{
		cout << "����ȫ���ϴ�����֤�Ƿ���Ҫ�洢������" << endl;//����������ϴ��ˣ�˵�����Լ��ٴ��˲�����Ȼ���ж��費��Ҫ�洢���������Ҫ��ֱ�ӱ���root������ͬ��
		if (check_store())
		{
			cout << "��Ҫ�洢����ʼͬ��" << endl;
			//TODO ͬ�����岡������Ҫ�ȴ�10s������ͬ������ֹ�еĽڵ�û�и��µ�����ֵ��ͬ��ʱ��Ҫ���ٲ�ϵͳ���ж�����ֵ�Ƕ���


		}
		else
		{
			cout << "����Ҫ�洢����������ͬ������" << endl;
		}
	}
	else
	{
		//������û�гɹ��ϴ��Ĳ��������ϴ�,���ϴ�ʱ�ǵ��޸Ĳ������ϴ�ʱ��
		//��json�н�����δ�ϴ�������
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
			//ÿ�ҵ�һ��MD5��ȥ��鲡��������û�в�����MD5ֵ��Ӧ

			if (!reader_add.parse(trapoolstr, value) && !trapoolstr.empty())
				printf("������Json��������\n");
			//���������ػ�ȡmd5ֵ
			int size = value.size();
			vector<CaseBroadcast*>BroadCast_threads;
			bool deleteflag = true;
			for (int i = 0; i < size; i++)
			{
				Json::Value jsontemp;
				jsontemp = value[i];
				//�ж��Ƿ�����ʱ���
				if (standard_to_stamp( (char*)jsontemp["time"].asString().c_str()) > time_n ||standard_to_stamp( (char*)jsontemp["time"].asString().c_str()) < time_m)
					continue;
				Json::StreamWriterBuilder writer;
				std::unique_ptr<Json::StreamWriter> write(writer.newStreamWriter());
				std::ostringstream os;
				write->write(jsontemp, &os);
				write->~StreamWriter();
				std::string Casejson = os.str();
				//�Ѿ���ȡ������ȡmd5ֵ
				MD5* md5 = new  MD5(Casejson);
				std::string md5str = md5->hexdigest();

				if (root["NOTCOMMIT_LEAVES"][j].asString() == md5str)
				{
					//�����������޸�ʱ��ΪĿǰʱ��,����������ʱ��һ������ϴ�ʱ��һ�������дʱ��
					Json::Value casejson;
					Json::Reader casejson_reader;
					if (!casejson_reader.parse(Casejson,casejson))
					{
						cerr << "casejson����ʧ��" << endl;
						continue;
					}
					deleteflag = false;
					std::time_t t = std::time(0);//��ȡϵͳ��ǰʱ��
					value[i]["time"] = num2time( static_cast<int>(t));
					//Ȼ�����¹㲥������ֻ����ԭ���Լ��������еĲ��������¹㲥
					if (casejson["hospital"].asString() != localhospital)
					{
						continue;
					}
					Json::Value broad_again;
					broad_again = value[i];
					std::string broad_again_str = swriter.write(value[i]);
					CaseBroadcast *c_broadcast = new CaseBroadcast(broad_again_str);
					cout << "���¹㲥�Ĳ���"<<broad_again_str << endl;
					//���̷߳Ž�vector�У���������ͷ��ڴ�
					BroadCast_threads.push_back(c_broadcast);
					c_broadcast->Run();
					trapoolstr_json.append(broad_again);

				}
				else
				{
					index_to_delete.push_back(i);
				}
			}
			//�������ûȷ�ϵ�����ͱ������������ں���ɾ����Щ����
			if (deleteflag)
			{
				
			}
			//��forѭ����Ҫɾ���㲥���߳�

			Msleep(300);
			for (int i = 0; i < BroadCast_threads.size(); i++)
			{
				delete BroadCast_threads[i];
			}
			deleteflag = true;
		}
		//����ѭ����ûȷ�ϵ����в������ҵ��ˣ�û��ûȷ�ϱ���Ĳ���ȫ��ɾ��
		//ɾ��ɾ������ֻ��������Ūһ��json
		//for (int i = index_to_delete.size() - 1; i >= 0; i--)//�����һ��ɾ��
		//{
		//	//value.removeIndex(index_to_delete[i],NULL);
		//	//value[index_to_delete[i]] = Json::Value::null;
		//}
		//�����޸���ɵ�����д��ȥ
		trapoolstr = swriter.write(trapoolstr_json);
		cout << "������������" << trapoolstr << endl;
		//TODO д��ȥ��δ�ϴ������ݻ�Ҫ�����㲥һ��////////////(����ɣ�164~174�У�


	}
	return 0;
}

bool Consensus::check_store()
{
	return true;
}






int Consensus::Routine()
{
	char buf_waiting[1024] = { 0 };//���buf��������ͬ��ʱ�ӷ�����m��nʱ���mn.�Լ�֮���ͬ����Ϣ
	local.SetIp(localIP.c_str());
	local.SetPortA(20000);
	sock.Open(local, true);
	int mainNodeFailure = 0;
	std::string value_x;
	std::string merkleTree_root;
	state = waiting;
	std::cout << "��ʶ�㷨�߳̿�����20000��" << endl;
	//��ʶ�㷨�߳̿�������Ҫ���������е����ݣ�������Ҫ����ͬ��ʱ�ӵ�����ת��״̬
	//�ʼ��״̬��waiting״̬
	while (true)
	{
		int n = sock.RecvFrom(buf_waiting, 1024, peer);
		NodeNumber = Nodes_vector.size()-1;
		if (n < 0) {
			std::cout << "���յ��������ȵ�����,��ʶ�㷨�̣߳�20000��" << endl;
			continue;
		}
		//sock.Close();
		state = recv_mn;
		if (!(peer.GetIp_str() == MainIP))//�������ͬ��ʱ�ӷ�������������
		{
			std::cout << "δ֪��Դ����ԴΪ" << peer.GetIp_str() << "����������ʶ�㷨�̣߳�20000��" << endl;
			continue;
		}

		//����ͬ��ʱ�ӷ���������
		if (!reader.parse(buf_waiting, root))
		{
			std::cout << "�������ڵ��json�������󣡹�ʶ�㷨�̣߳�20000��";
			continue;
		}
		const std::string type = root["type"].asString();//��ȡ���json�����ͣ�����Ӧ����MN����ʾ֪ͨM��N��ʱ���
		if (type != "MN")
		{
			std::cout << "�������ڵ��֪ͨ��Ч������Ϊ" << type << ",��ʶ�㷨�̣߳�20000��" << endl;
			continue;
		}
		//���������ȷ�����ȡM��N�ֱ��Ƕ���
		//M��NӦ�������ε�
		std::time_t t = std::time(0);//��ȡϵͳ��ǰʱ��
		std::cout << "��ǰunixʱ���Ϊ" << t << endl;

		time_m = root["TIME_M"].asUInt();
		time_n = root["TIME_N"].asUInt();
		//��ȡ��M��N��׼�������һ�׶�
		//����Ҫ�����ʱ����ڵĲ�������ã�Ȼ�����Ĭ�˶���

		//TODO  ���ҷ���ʱ����ڵĲ�����Ȼ����ʱ����Ѳ�������(��һ������ɣ�
		cases_time_mn = FindTraspoolCase_char(time_m, time_n);
		//TODO  ����Ĭ�˶���(����ɣ�
		//����ǿյģ�Ҳ�������ʱ�����û��������Ҫֱ�Ӹ������ַ�����hashֵ������Ҫ����Ĭ�˶���
		if (!cases_time_mn.empty())
		{
			//������վͼ���Ĭ�˶���
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
		//TODO ����Ĭ�˶���֮ǰҪ��ÿ�������Լ�����Ӧ��MD5����vector�У������) 
		cout << "�������merkleroot:" << merkleTree_root << endl;
		value_x = merkleTree_root;
		int Consensus_times = 0;
		while (true) //���ѭ������f+1�׶εĹ�ʶ
		{
			//����Ҫ�ȴ�ͬ��ʱ�ӵ�֪ͨ,ROUND��ʾ,��һ������Start����Stop�����Ƿ�����
			if (mainNodeFailure==0)
			{
				recv_and_change_state((char*)"ROUND");//��һ��leaderû����ŵȴ�ROUND��ͬ��������ֱ������ȥ
			}
			mainNodeFailure = 0;
			if (root["State"].asString() == "STOP")
			{
				cout << "���׶ι�ʶ�㷨��������" << Consensus_times << "��" << endl;
				cout << "���վ���ֵΪ" << value_x << endl;;
				break;
			}
			else
			{
				cout << "���ι�ʶ�㷨��" << Consensus_times << "�ο�ʼ" << endl;
			}
			Consensus_times++;
			
			//Ȼ��㲥�Լ��������Ĭ�˶��������ڷ��ִ����ʱ��������Ĭ�˶�����
			state = ready_to_broadcast;

			//TODO ����ֱ�ӹ㲥merkleTree_root,��Ҫ���ϳ�json������ɣ� 
			Json::Value merkletree_root_json;
			Json::FastWriter f_Writer;
			merkletree_root_json["type"] = "MERKLEROOT";
			merkletree_root_json["VALUE"] = value_x;
			std::string merkleTree_root_1 = f_Writer.write(merkletree_root_json);


			//������յ�һ�ֹ㲥ֵ��״̬
			state = recv_1;
			//����Ҫ���������̣߳�Ȼ����ܿ����㲥�̣߳����ҹ㲥�߳̿���ʱ��Ҫ��΢��һ��
			Recv_thread *recv_thread = new Recv_thread();
			recv_thread->Run();

			//OS_Thread::Sleep(2);//2���Ժ����㲥��Ϊ�˱���Ӵ����縺�أ�����˱�0~5��
			srand((int)time(nullptr));
			int random_time = rand() % 1000 + 1000;//1~3s///////////////////////////////////////////////////////////////////////////////ע���������Ӧ�÷��ڹ㲥���߳��Ҫ��
			//OS_Thread::Msleep(random_time);
			cout << "�����߳̿���" << endl;
			Broadcast_thread* broadcast_thread = new Broadcast_thread(merkleTree_root_1, 20011, random_time);
			broadcast_thread->Run();//�㲥�Լ��������Ĭ�˶���
			//Ȼ��ͳ�ƽ��յ���value�������������ֵ��-1˵���ظ����ĸ�������n-f�Σ��������ֵ����-1��˵������n-f�Σ�����hashΪ����ֵ
			OS_Thread::Msleep(2*1000+250);//����3�룬���������ݴ��Ͷ�������Ȼ��ͳ��
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
			if (is_max_value_hash_integer)//���������
			{
				std::cout << "��һ�ֳ����ϸ�С��n-f�������ת��״̬�����ǲ��㲥propose" << endl;
				state = no_propose;
			}
			else
			{
				std::cout << "��һ���������ٽ��յ�value(y) n-f�Σ�׼��propose(y)" << endl;
				state = propose_1;
				propose_value_1 = max_value_hash;
			}
			//�����������һ��propose�׶�

			//TODO ��propose1��ֵ�����json�������) 
			Json::Value propose1;
			Json::FastWriter fwriter_propose1;
			propose1["type"] = "MERKLEROOT";
			propose1["VALUE"] = propose_value_1;
			propose_value_1 = fwriter_propose1.write(propose1);
			//�ȴ�ͬ��֪ͨ����ʼpropose��һ��
			//TODO Ҫ���ó�ʱʱ�䣨��ʱ������ 
			//���¿��Ե���recv_changeState����/////////////////////////////////
			recv_and_change_state((char*)"PROPOSE");
			//���յ�ͬ��֪ͨ���ȿ��������̣߳�������˱ܺ��������߳�
			Recv_thread *recv_thread_propose = new Recv_thread();
			recv_thread_propose->Run();
			//�ȴ�һ��ʱ����ٿ�ʼ�㲥���㲥ǰҪ������˱�0~2s
			random_time = rand() % 1000 + 1000;
			//OS_Thread::Msleep(random_time);
			//Ȼ��ʼ�㲥
			//Ҫ����״̬���ж��Ƿ�Ҫpropose
			if (state == propose_1)
			{
				broadcast_thread->setMessage(const_cast<char*>(propose_value_1.c_str()));/////���Ŀǰ����֪��������ȷ
				broadcast_thread->setPort(20011);
				broadcast_thread->setMsleep(random_time);
				broadcast_thread->Run();
			}
			else if (state == no_propose)
			{
				//�����׼���㲥�Ļ�
				//OS_Thread::Sleep(2);
			}
			//����10s�������еĹ㲥������
			OS_Thread::Msleep(2050);
			//Ȼ�����propose�Ľ�������Ƿ�ִ���㷨��9��
			recv_thread_propose->stop();

			std::string propose_z = recv_thread_propose->calculate_maxvalue_proposez();
			int propose_z_times = recv_thread_propose->get_propose_z_times();//���յ�propose_z��������
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
				std::cout << "���յ���propose(z)����f�Σ�";
				if (propose_z == merkleTree_root)
				{
					std::cout << "zֵ������x��ͬ" << endl;
				}
				else
				{
					std::cout << "z��x��ͬ��ִ���㷨x=z" << endl;
					value_x = propose_z;
				}
			}
			else
			{
				std::cout << "���յ�propose(z)" << propose_z << "�Σ�����f��" << endl;
			}
			//��������������֣����ж����ڵ㣬Ȼ������Ƿ�ı�ֵ
			//���¿��Ե���recv_changeState����/////////////////////////////////
			recv_and_change_state((char*)"MAINNODE");
			
			//�Ѿ���ȡ���ڵ�֪ͨ��json,�������������ڵ�
			std::string Main_IP = root["MAINIP"].asString();
			if (Main_IP == localIP)
			{
				std::cout << "���׶����ڵ�Ϊ�Լ�" << endl;
				state = mainNode_broad;
			}
			else
			{
				std::cout << "���׶����ڵ�Ϊ" << Main_IP << endl;
				state = listening_mainNode;
			}
			//�����������ǹ㲥����ֵ���ǽ������ڵ��ֵ
			if (state == listening_mainNode)
			{
				//����ǽ������ڵ��ֵ�Ļ���Ҫ�ж�IP
				root.clear();
				mainNodeFailure = recv_and_change_state((char*)"MAINNODE_VALUE");
				if (mainNodeFailure == 1)
				{

					continue;
				}
				//�Ѿ����յ����ڵ��ֵw���������ж��Ƿ�ı�ֵ
				if (propose_z_times < NodeNumber - (NodeNumber - 1) / 3)//����ϸ�С��n-f
				{
					std::cout << "Propose�����ϸ�С��n-f,����Ϊ" << propose_z_times << ",x=w" << endl;
					cout << "���ڵ�㲥��ֵwΪ" << root["VALUE"].asString() << endl;
					value_x = root["VALUE"].asString();
				}
				else
				{
					cout << "propose����Ϊ" << propose_z << "�����ı�����ֵ" << endl;
				}
			}
			if (state == mainNode_broad)
			{
				//����Լ������ڵ㣬�͹㲥�Լ���ֵ(�㲥��ֵӦ�����Լ������ֵ��
				std::string MainNode_value_w;
				//TODO ��MerkleTree_root�����json(����ɣ� 
				Json::Value mainNode;
				Json::FastWriter fast_writer;
				mainNode["type"] = "MAINNODE_VALUE";
				mainNode["VALUE"] = value_x;
				MainNode_value_w = fast_writer.write(mainNode);
				//Ҫ�ȵȴ�һ���ڹ㲥�Լ���ֵ��ȷ�����еĽڵ㶼��ʼ����
				//Sleep(3);
				cout << "���ڵ㿪ʼ�㲥��" << MainNode_value_w << endl;
				broadcast_thread->setMessage(const_cast<char*>(MainNode_value_w.c_str()));
				broadcast_thread->setPort(20000);
				broadcast_thread->setMsleep(400);
				broadcast_thread->Run();
			}
			OS_Thread::Msleep(550);
			delete broadcast_thread;
			delete recv_thread_propose;
			cout << "��ʶ�㷨��һ��ѭ��������Ŀǰ������ֵΪx=" << value_x << endl;
		}
		//һ�ֹ�ʶ�㷨�������������Ա��Ƿ����Լ��ľ���ֵһ�������к�������
		if (value_x == merkleTree_root)
		{
			//�������ֵ������ֵ���ֱ�ӽ������ֹ�ʶ��������������ݿ�
			cout << "����ֵ���Լ���ֵ��ͬ����ɹ�ʶ�㷨����ʶ�㷨�߳�(20000)" << endl;
			//TODO ���������ϴ������ݿ�


		}
		else
		{
			//�������ֵ��ͬ�������һ�ι㲥����ȷ�Ľڵ�����Ĭ�˶���
			cout << "����ֵ���Լ���ֵ��ͬ����ʼ��ȡĬ�˶���" << endl;
			std::string Ask_IP;//׼������Ĭ�˶�����IP

			for (int i = 0; i < MerkleRoots1.size(); i++)
			{
				if (MerkleRoots1[i]->MerkleRoot == value_x)
				{
					Ask_IP = MerkleRoots1[i]->src_ip;
					break;
				}
			}
			//�ҵ�Ask_IP������֤����Ҫ����֤�Լ��Ĳ����Ƿ��Ѿ��ϴ������Ƿ��ܱ�֤��

			//��֤�ϴ������û�ϴ��Ķ���request_MerkleTree_proof�ﴦ��
			request_MerkleTree_proof(Ask_IP);


		}
		//sock.Open(local, true);

	}
}
