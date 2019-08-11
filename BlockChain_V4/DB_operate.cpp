#include "DB_operate.h"
extern std::string trapoolstr;
bool ConnectDB()
{
	MYSQL* con = new MYSQL;
	//�������ݿ�
	if (mysql_library_init(0, NULL, NULL))
	{
		//could not initialize MySQL library
		delete con;
		return false;
	}
	mysql_init(con);
	MYSQL* ret = mysql_real_connect(con, DB_IP, DB_USERNAME, DB_PASSWORD, DB_NAME, 0, NULL, 0);
	if (!ret)
	{
		//�޷��������ݿ�
		delete con;
		return false;
	}
	else
	{
		//���ӳɹ����Ͽ����ݿ�����
		mysql_close(con);
		mysql_library_end();
		delete con;
		return true;
	}
}

int AddBlock(std::string  hash, std::string time)
{
	//��ȡ��ǰ��������Ŀ
	int count = CountBlock();
	std::string blockname = "block" + std::to_string(count);
	MYSQL* con = new MYSQL;//���ݿ�
	//�������ݿ�
	if (mysql_library_init(0, NULL, NULL))
	{
		//could not initialize MySQL library
		return -2;
	}
	mysql_init(con);
	MYSQL* ret = mysql_real_connect(con, DB_IP, DB_USERNAME, DB_PASSWORD, DB_NAME, 0, NULL, 0);
	if (!ret)
	{
		//�޷��������ݿ�
		return -1;
	}
	else
	{
		//���ӳɹ�
		//����SQL���
		std::string query = "";
		mysql_query(con, query.c_str());
		query = "CREATE TABLE `" + blockname + "` ("
			"`doc_id` char(10) NOT NULL,"
			"`pat_id` char(10) NOT NULL,"
			"`hospital` varchar(64) NOT NULL,"
			"`time` datetime NOT NULL,"
			"`text` text NOT NULL,"
			"`hash` char(255) NOT NULL,"
			"PRIMARY KEY(`hash`)"
			") ENGINE = MyISAM DEFAULT CHARSET = latin1";
		int res = mysql_query(con, query.c_str());
		//��blocknum�����������Ϣ
		query = "INSERT INTO `blocknum` (`name`, `prehash`, `blockindex`, `timestamp`) "
			"VALUES ('" + blockname + "', '" + hash + "', '" + std::to_string(count) + "', '" + time + "');";
		res = mysql_query(con, query.c_str());
		//�Ͽ����ݿ�����
		mysql_close(con);
		mysql_library_end();
		return 0;
	}
}

int CountBlock()
{
	MYSQL* con = new MYSQL;//���ݿ�
	//�������ݿ�
	if (mysql_library_init(0, NULL, NULL))
	{
		//could not initialize MySQL library
		return -2;
	}
	mysql_init(con);
	MYSQL* ret = mysql_real_connect(con, DB_IP, DB_USERNAME, DB_PASSWORD, DB_NAME, 0, NULL, 0);
	if (!ret)
	{
		//�޷��������ݿ�
		return -1;
	}
	else
	{
		//���ӳɹ�
		int count = 0;
		//����SQL���
		std::string query = "";
		mysql_query(con, query.c_str());
		query = "SELECT COUNT(*) FROM `blocknum`";
		int res = mysql_query(con, query.c_str());
		MYSQL_RES* result = mysql_store_result(con);
		if (result)
		{
			MYSQL_ROW row = mysql_fetch_row(result);
			count = atoi(row[0]);
		}
		//�Ͽ����ݿ�����
		mysql_close(con);
		mysql_library_end();
		return count;
	}
}

std::string PreHash(std::string blockname)
{
	std::string hash = "";
	MYSQL* con = new MYSQL;//���ݿ�
	//�������ݿ�
	if (mysql_library_init(0, NULL, NULL))
	{
		hash = "could not initialize MySQL library";
		return hash;
	}
	mysql_init(con);
	MYSQL* ret = mysql_real_connect(con, DB_IP, DB_USERNAME, DB_PASSWORD, DB_NAME, 0, NULL, 0);
	if (!ret)
	{
		hash = "�޷��������ݿ�";
		return hash;
	}
	else
	{
		//���ӳɹ�
		//����SQL���
		std::string query = "";
		mysql_query(con, query.c_str());
		query = "SELECT `prehash` FROM blocknum WHERE `name`='" + blockname + "'";
		int res = mysql_query(con, query.c_str());
		MYSQL_RES* result = mysql_store_result(con);
		if (result)
		{
			MYSQL_ROW row = mysql_fetch_row(result);
			hash = row[0];
		}
		//�Ͽ����ݿ�����
		mysql_close(con);
		mysql_library_end();
		return hash;
	}
}

int AddCase(std::string blockname, std::string CaseJson)
{
	//����Json
	Json::Value root;
	Json::Reader reader;
	int n = CaseJson.size();
	if (!reader.parse(CaseJson, root))
	{
		printf("json��������");
		return -1;
	}
	//��ȡ����Ҫ��
	std::string Casedoc_id = root["doc_id"].asString();
	std::string Casepat_id = root["pat_id"].asString();
	std::string Casehospital = root["hospital"].asString();
	std::string Casetext = root["text"].asString();
	std::string pattern = "\'";
	std::regex re(pattern);
	std::string fmt = "\\\'";
	Casetext = regex_replace(Casetext, re, fmt);
	std::string Casehash = root["hash"].asString();
	//std::string Casesig = root["sig"].asString();
	std::string Casetime = root["time"].asString();
	//�������ݿ�
	MYSQL* con = new MYSQL;//���ݿ�
	if (mysql_library_init(0, NULL, NULL))
	{
		return -2;
	}
	mysql_init(con);
	MYSQL* ret = mysql_real_connect(con, DB_IP, DB_USERNAME, DB_PASSWORD, DB_NAME, 0, NULL, 0);
	if (!ret)
	{
		return -1;
	}
	else
	{
		//���ӳɹ�
		//����SQL���
		std::string query = "";
		mysql_query(con, query.c_str());
		query = "INSERT INTO `" + blockname + "` (`doc_id`, `pat_id`, `hospital`, `time`, `text`, `hash`) "
			"VALUES ('" + Casedoc_id + "', '" + Casepat_id + "', '" + Casehospital + "', '" + Casetime + "', '" + Casetext + "', '" + Casehash + "');";
		int res = mysql_query(con, query.c_str());
		//�Ͽ����ݿ�����
		mysql_close(con);
		mysql_library_end();
		return 0;
	}
}

std::string ReadBlock(std::string blockname)
{
	std::string Blockdata = "";
	MYSQL* con = new MYSQL;//���ݿ�
	//�������ݿ�
	if (mysql_library_init(0, NULL, NULL))
	{
		Blockdata = "could not initialize MySQL library";
		return Blockdata;
	}
	mysql_init(con);
	MYSQL* ret = mysql_real_connect(con, DB_IP, DB_USERNAME, DB_PASSWORD, DB_NAME, 0, NULL, 0);
	if (!ret)
	{
		Blockdata = "�޷��������ݿ�";
		return Blockdata;
	}
	else
	{
		//���ӳɹ�
		Json::Value root;
		Json::StreamWriterBuilder writerroot;
		std::unique_ptr<Json::StreamWriter> writeroot(writerroot.newStreamWriter());
		std::ostringstream osroot;
		//����SQL��䣬��ȡ������˲����������Ϣ
		std::string query = "";
		mysql_query(con, query.c_str());
		query = "SELECT * FROM `blocknum` WHERE `name`='" + blockname + "'";
		int res = mysql_query(con, query.c_str());
		MYSQL_RES* result = mysql_store_result(con);
		if (result)
		{
			MYSQL_ROW row = mysql_fetch_row(result);
			root["prehash"] = row[1];
			root["blockindex"] = row[2];
			root["timestamp"] = row[3];
		}
		//����SQL��䣬��ȡ�������в���
		query = "SELECT * FROM `" + blockname + "`";
		res = mysql_query(con, query.c_str());
		result = mysql_store_result(con);
		if (result)
		{
			MYSQL_ROW row;
			unsigned int row_num = mysql_num_rows(result);//��ȡ����
			while ((row = mysql_fetch_row(result)))
			{
				//��ÿ�ݲ������Json��ʽ(ÿ�����ݴ���)
				Json::Value req;
				Json::StreamWriterBuilder writer;
				std::unique_ptr<Json::StreamWriter> write(writer.newStreamWriter());
				std::ostringstream os;
				req["doc_id"] = row[0];
				req["pat_id"] = row[1];
				req["hospital"] = row[2];
				req["time"] = row[3];
				req["text"] = row[4];
				req["hash"] = row[5];
				write->write(req, &os);
				write->~StreamWriter();
				//������벡��
				root["trans"].append(req);
			}
			mysql_free_result(result);
		}
		writeroot->write(root, &osroot);
		writeroot->~StreamWriter();
		Blockdata = osroot.str();
		//�Ͽ����ݿ�����
		mysql_close(con);
		mysql_library_end();
		return Blockdata;
	}
}

int DeleteBlock()
{

	MYSQL* con = new MYSQL;//���ݿ�
	//�������ݿ�
	if (mysql_library_init(0, NULL, NULL))
	{
		//could not initialize MySQL library
		return -2;
	}
	mysql_init(con);
	MYSQL* ret = mysql_real_connect(con, DB_IP, DB_USERNAME, DB_PASSWORD, DB_NAME, 0, NULL, 0);
	if (!ret)
	{
		//�޷��������ݿ�
		return -1;
	}
	else
	{
		//���ӳɹ�
		int count = CountBlock();
		//����SQL���
		std::string query = "";
		mysql_query(con, query.c_str());
		//ɾ����������
		for (int i = 0; i < count; i++)
		{
			query = "DROP TABLE block" + std::to_string(i);
			int res = mysql_query(con, query.c_str());
		}
		//ɾ�������¼
		query = "TRUNCATE blocknum";
		int res = mysql_query(con, query.c_str());
		//�Ͽ����ݿ�����
		mysql_close(con);
		mysql_library_end();
		return count;
	}
}

int WriteBlock(std::string Blockjson)
{
	//��ȡJson��ȡ������Ϣ
	Json::Value root;
	Json::Reader reader;
	if (!reader.parse(Blockjson, root))
	{
		printf("json��������");
		return -1;
	}
	std::string blockindex = root["blockindex"].asString();
	int e = CountBlock();
	if (atoi(blockindex.c_str()) != e)
		return -2;
	std::string prehash = root["prehash"].asString();
	std::string timestamp = root["timestamp"].asString();
	//�������
	AddBlock(prehash, timestamp);
	int size = root["trans"].size();
	for (int i = 0; i < size; i++)
	{
		Json::Value jsontemp;
		jsontemp = root["trans"][i];
		Json::StreamWriterBuilder writer;
		std::unique_ptr<Json::StreamWriter> write(writer.newStreamWriter());
		std::ostringstream os;
		write->write(jsontemp, &os);
		write->~StreamWriter();
		std::string Casejson = os.str();
		//���������������
		if (AddCase("block" + blockindex, Casejson) == -1)
			std::cout << "����д��ʧ��\n";
	}
}
std::vector<std::string> FindCase(int m, int n)
{
	std::string starttime = num2time(m);
	std::string endtime = num2time(n);
	std::vector<std::string> resultvec;
	MYSQL* con = new MYSQL;//���ݿ�
	//�������ݿ�
	if (mysql_library_init(0, NULL, NULL))
	{
		//could not initialize MySQL library
		return resultvec;
	}
	mysql_init(con);
	MYSQL* ret = mysql_real_connect(con, DB_IP, DB_USERNAME, DB_PASSWORD, DB_NAME, 0, NULL, 0);
	if (!ret)
	{
		//�޷��������ݿ�
		return resultvec;
	}
	else
	{
		//���ӳɹ�
		//����SQL���
		std::string query = "";
		mysql_query(con, query.c_str());
		//ѭ��
		int count = CountBlock() - 1;//����������	
		int casenum = 0;//�������Ĳ�������
		bool haved = false;//��δ�ҵ�����Ҫ��Ĳ���
		for (int i = count; i >= 0; i--)
		{
			query = "SELECT COUNT(*) FROM block" + std::to_string(i) + " WHERE `time`>'" + starttime + "' AND `time`<'" + endtime + "'";
			int res = mysql_query(con, query.c_str());
			MYSQL_RES* result = mysql_store_result(con);
			if (result)
			{
				MYSQL_ROW row = mysql_fetch_row(result);
				casenum = atoi(row[0]);//��ʱ����ڵĲ�������������ʼ>0����ʼ����
				if (casenum > 0)
				{
					//��ȡ�����������뵽������
					std::vector<std::string> temp = FindBlockCase(m, n, i);
					resultvec.insert(resultvec.end(), temp.begin(), temp.end());
					//�Ѿ���ʼ�ҵ�����Ҫ��Ĳ�����
					haved = true;
				}
				else if (haved)
				{
					//�ж�֮ǰ�Ƿ�������Ҫ��Ĳ���������˵����ʱ������Ѿ�û��
					break;
				}
			}
		}
		//�Ͽ����ݿ�����
		mysql_close(con);
		mysql_library_end();
		//��resultvec����
		sort(resultvec.begin(), resultvec.end());
		return resultvec;
	}
}
std::vector<std::string> FindBlockCase(int m, int n, int index)
{
	std::string starttime = num2time(m);
	std::string endtime = num2time(n);
	std::vector<std::string> resultvec;
	MYSQL* con = new MYSQL;//���ݿ�
	//�������ݿ�
	if (mysql_library_init(0, NULL, NULL))
	{
		//could not initialize MySQL library
		return resultvec;
	}
	mysql_init(con);
	MYSQL* ret = mysql_real_connect(con, DB_IP, DB_USERNAME, DB_PASSWORD, DB_NAME, 0, NULL, 0);
	if (!ret)
	{
		//�޷��������ݿ�
		return resultvec;
	}
	else
	{
		//���ӳɹ�
		int count = CountBlock();
		//����SQL���
		std::string query = "";
		mysql_query(con, query.c_str());
		query = "SELECT * FROM block" + std::to_string(index) + " WHERE `time`>'" + starttime + "' AND `time`<'" + endtime + "'";
		int res = mysql_query(con, query.c_str());
		MYSQL_RES* result = mysql_store_result(con);
		if (result)
		{
			MYSQL_ROW row;
			unsigned int row_num = mysql_num_rows(result);//��ȡ����
			unsigned int field_num = mysql_num_fields(result);//��ȡ����
			while ((row = mysql_fetch_row(result)))
			{
				//��װ�����γ��ַ���
				Json::Value req;
				Json::StreamWriterBuilder writer;
				std::unique_ptr<Json::StreamWriter> write(writer.newStreamWriter());
				std::ostringstream os;
				req["pat_id"] = row[1];
				req["doc_id"] = row[0];
				req["text"] = row[4];
				req["hash"] = row[5];
				req["time"] = row[3];
				req["hospital"] = row[2];
				write->write(req, &os);
				write->~StreamWriter();
				std::string jsondata = os.str();
				//����md5
				MD5* md5 = new  MD5(jsondata);
				std::string md5str = md5->hexdigest();
				resultvec.push_back(md5str);
			}
			mysql_free_result(result);
		}
		//�Ͽ����ݿ�����
		mysql_close(con);
		mysql_library_end();
		return resultvec;
	}
}
std::string num2time(int n)
{
	//��1970��1��1��00:00:00������������	
	std::stringstream ss;
	long long seconds = n;
	std::time_t t(seconds);
	ss << std::put_time(std::localtime(&t), "%F %T");
	return  ss.str();
}
int standard_to_stamp(char *str_time)//��׼ʱ��ת��Ϊunixʱ���
{
	struct tm stm;
	int iY, iM, iD, iH, iMin, iS;
	memset(&stm, 0, sizeof(stm));
	iY = atoi(str_time);
	iM = atoi(str_time + 5);
	iD = atoi(str_time + 8);
	iH = atoi(str_time + 11);
	iMin = atoi(str_time + 14);
	iS = atoi(str_time + 17);
	stm.tm_year = iY - 1900;
	stm.tm_mon = iM - 1;
	stm.tm_mday = iD;
	stm.tm_hour = iH;
	stm.tm_min = iMin;
	stm.tm_sec = iS;
	/*printf("%d-%0d-%0d %0d:%0d:%0d\n", iY, iM, iD, iH, iMin, iS);*/   //��׼ʱ���ʽ���磺2016:08:02 12:12:30
	return (int)mktime(&stm);
}

std::vector<std::string> FindTraspoolCase(int m, int n)
{
	//std::string starttime = num2time(m);
	//std::string endtime = num2time(n);
	std::vector<std::string> resultvec;
	//����������
	Json::Value value;
	Json::Reader reader_add;
	if (!reader_add.parse(trapoolstr, value) && !trapoolstr.empty())
		printf("������Json��������\n");
	//���������ػ�ȡmd5ֵ
	int size = value.size();
	for (int i = 0; i < size; i++)
	{
		Json::Value jsontemp;
		jsontemp = value[i];
		//�ж��Ƿ�����ʱ���
		if (standard_to_stamp(const_cast<char*>(jsontemp["time"].asString().c_str())) > n || standard_to_stamp(const_cast<char*>(jsontemp["time"].asString().c_str())) < m)
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
		resultvec.push_back(md5str);
	}
	//����
	sort(resultvec.begin(), resultvec.end(), std::less<std::string>());
	return resultvec;
}
std::vector<char*> FindTraspoolCase_char(int m, int n)
{
	std::string starttime = num2time(m);
	std::string endtime = num2time(n);
	std::vector<char*> resultvec;
	//����������
	Json::Value value;
	Json::Reader reader_add;
	if ((!reader_add.parse(trapoolstr, value) )|| trapoolstr.empty())
		printf("������Json��������\n");
	//���������ػ�ȡmd5ֵ
	int size = value.size();
	for (int i = 0; i < size; i++)
	{
		Json::Value jsontemp;
		jsontemp = value[i];
		//�ж��Ƿ�����ʱ���
		if (standard_to_stamp(const_cast<char*>(jsontemp["time"].asString().c_str())) > n || standard_to_stamp(const_cast<char*>(jsontemp["time"].asString().c_str())) < m)
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
		char* buff = new char[256];
		strcpy(buff,md5str.c_str());
		resultvec.push_back(buff);
	}
	//����
	sort(resultvec.begin(), resultvec.end(), std::less<std::string>());
	return resultvec;
}