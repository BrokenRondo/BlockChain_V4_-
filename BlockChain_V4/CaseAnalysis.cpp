#include "CaseAnalysis.h"
CaseAnalysis::CaseAnalysis(string str)
{
	this->case_str = str;
}
int CaseAnalysis::Routine()//��������
{
	//�Բ�����������
	Json::Value root;
	Json::Reader reader;
	if (!reader.parse(case_str, root))
	{
		printf("���յ��Ĳ���json��ʽ��������");
		return -1;
	}
	//�жϲ�����Դ�����Խڵ�㲥������ҽ������ˣ�
	bool source = root["source"].asBool();
	const std::string thishash = root["hash"].asString();
	if (source)//���Խڵ�㲥
	{
		for (int i = 0; i < Commonhashpool.size(); i++)
		{
			if (thishash == Commonhashpool[i])
			{
				//printf("���������������ѽ��չ��������������\n");
				return 1;
			}
		}
		//����û���չ�����ջ
		//����������ֹ�����߳��޸Ĳ���
		OS_Mutex mutex;
		mutex.Lock();
		//���²����Ĺ�ϣֵѹ���ϣջ��
		if (Commonhashpool.size() < 20)
		{
			Commonhashpool.push_back(thishash);
		}
		else
		{
			Commonhashpool.pop_back();
			Commonhashpool.push_back(thishash);
		}
		//���������ݷ��빫����������
		Json::Value value;
		Json::Reader reader_add;
		if (!reader_add.parse(Commonpool, value) && !Commonpool.empty())
			printf("����������Json��ʽ��������\n");
		value.append(root);
		Json::StyledWriter swriter;
		Commonpool = swriter.write(value);
		cout << "���������ط�������\n" + Commonpool + "\n";
		mutex.Unlock();
		//�㲥case_str
		CaseBroadcast *casebroadcast = new CaseBroadcast(case_str);
		casebroadcast->Run();
		//δ�ͷ��ڴ�
	}
	else//����ҽ�������
	{
		for (int i = 0; i < Privatehashpool.size(); i++)
		{
			if (thishash == Privatehashpool[i])
			{
				//printf("����˽�в������ѽ��չ��������������\n");
				return 1;
			}
		}
		//����û���չ�����ջ
		//����������ֹ�����߳��޸Ĳ���
		OS_Mutex mutex;
		mutex.Lock();
		//���²����Ĺ�ϣֵѹ���ϣջ��
		if (Privatehashpool.size() < 20)
		{
			Privatehashpool.push_back(thishash);
		}
		else
		{
			Privatehashpool.pop_back();
			Privatehashpool.push_back(thishash);
		}
		//���������ݷ���˽�в�������
		Json::Value value;
		Json::Reader reader_add;
		if (!reader_add.parse(Privatepool, value) && !Privatepool.empty())
			printf("˽�в�����Json��ʽ��������\n");
		value.append(root);
		Json::StyledWriter swriter;
		Privatepool = swriter.write(value);
		cout << "˽�в����ط�������\n" +Privatepool + "\n";
		mutex.Unlock();
		//��˽�в���תΪ���в�����ת����˽�нڵ㣨�����Լ����Ĺ��в�����
		root["source"] = true;
		root["node"] = LocalIP;
		Json::StyledWriter caseswriter;
		string commoncase = caseswriter.write(root);
		//�㲥
		CaseBroadcast *casebroadcast = new CaseBroadcast(commoncase);
		casebroadcast->Run();
		//δ�ͷ��ڴ�
	}
	return 0;
}
