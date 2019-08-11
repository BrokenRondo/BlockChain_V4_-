#include "CaseAnalysis.h"
CaseAnalysis::CaseAnalysis(string str)
{
	this->case_str = str;
}
int CaseAnalysis::Routine()//解析病历
{
	//对病历解析解析
	Json::Value root;
	Json::Reader reader;
	if (!reader.parse(case_str, root))
	{
		printf("接收到的病历json格式解析错误！");
		return -1;
	}
	//判断病历来源：来自节点广播？来自医生桌面端？
	bool source = root["source"].asBool();
	const std::string thishash = root["hash"].asString();
	if (source)//来自节点广播
	{
		for (int i = 0; i < Commonhashpool.size(); i++)
		{
			if (thishash == Commonhashpool[i])
			{
				//printf("病历公共病历池已接收过，无需继续处理\n");
				return 1;
			}
		}
		//病历没接收过，入栈
		//互斥锁，防止其他线程修改参数
		OS_Mutex mutex;
		mutex.Lock();
		//将新病历的哈希值压入哈希栈中
		if (Commonhashpool.size() < 20)
		{
			Commonhashpool.push_back(thishash);
		}
		else
		{
			Commonhashpool.pop_back();
			Commonhashpool.push_back(thishash);
		}
		//将病历内容放入公共病历池中
		Json::Value value;
		Json::Reader reader_add;
		if (!reader_add.parse(Commonpool, value) && !Commonpool.empty())
			printf("公共病历池Json格式解析错误\n");
		value.append(root);
		Json::StyledWriter swriter;
		Commonpool = swriter.write(value);
		cout << "公共病历池发生更新\n" + Commonpool + "\n";
		mutex.Unlock();
		//广播case_str
		CaseBroadcast *casebroadcast = new CaseBroadcast(case_str);
		casebroadcast->Run();
		//未释放内存
	}
	else//来自医生桌面端
	{
		for (int i = 0; i < Privatehashpool.size(); i++)
		{
			if (thishash == Privatehashpool[i])
			{
				//printf("病历私有病历池已接收过，无需继续处理\n");
				return 1;
			}
		}
		//病历没接收过，入栈
		//互斥锁，防止其他线程修改参数
		OS_Mutex mutex;
		mutex.Lock();
		//将新病历的哈希值压入哈希栈中
		if (Privatehashpool.size() < 20)
		{
			Privatehashpool.push_back(thishash);
		}
		else
		{
			Privatehashpool.pop_back();
			Privatehashpool.push_back(thishash);
		}
		//将病历内容放入私有病历池中
		Json::Value value;
		Json::Reader reader_add;
		if (!reader_add.parse(Privatepool, value) && !Privatepool.empty())
			printf("私有病历池Json格式解析错误\n");
		value.append(root);
		Json::StyledWriter swriter;
		Privatepool = swriter.write(value);
		cout << "私有病历池发生更新\n" +Privatepool + "\n";
		mutex.Unlock();
		//将私有病历转为公有病历，转发至私有节点（包括自己）的公有病历池
		root["source"] = true;
		root["node"] = LocalIP;
		Json::StyledWriter caseswriter;
		string commoncase = caseswriter.write(root);
		//广播
		CaseBroadcast *casebroadcast = new CaseBroadcast(commoncase);
		casebroadcast->Run();
		//未释放内存
	}
	return 0;
}
