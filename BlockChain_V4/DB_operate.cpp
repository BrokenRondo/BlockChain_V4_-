#include "DB_operate.h"
extern std::string trapoolstr;
bool ConnectDB()
{
	MYSQL* con = new MYSQL;
	//连接数据库
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
		//无法连接数据库
		delete con;
		return false;
	}
	else
	{
		//连接成功，断开数据库连接
		mysql_close(con);
		mysql_library_end();
		delete con;
		return true;
	}
}

int AddBlock(std::string  hash, std::string time)
{
	//获取当前区块链数目
	int count = CountBlock();
	std::string blockname = "block" + std::to_string(count);
	MYSQL* con = new MYSQL;//数据库
	//连接数据库
	if (mysql_library_init(0, NULL, NULL))
	{
		//could not initialize MySQL library
		return -2;
	}
	mysql_init(con);
	MYSQL* ret = mysql_real_connect(con, DB_IP, DB_USERNAME, DB_PASSWORD, DB_NAME, 0, NULL, 0);
	if (!ret)
	{
		//无法连接数据库
		return -1;
	}
	else
	{
		//连接成功
		//构造SQL语句
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
		//向blocknum中添加区块信息
		query = "INSERT INTO `blocknum` (`name`, `prehash`, `blockindex`, `timestamp`) "
			"VALUES ('" + blockname + "', '" + hash + "', '" + std::to_string(count) + "', '" + time + "');";
		res = mysql_query(con, query.c_str());
		//断开数据库连接
		mysql_close(con);
		mysql_library_end();
		return 0;
	}
}

int CountBlock()
{
	MYSQL* con = new MYSQL;//数据库
	//连接数据库
	if (mysql_library_init(0, NULL, NULL))
	{
		//could not initialize MySQL library
		return -2;
	}
	mysql_init(con);
	MYSQL* ret = mysql_real_connect(con, DB_IP, DB_USERNAME, DB_PASSWORD, DB_NAME, 0, NULL, 0);
	if (!ret)
	{
		//无法连接数据库
		return -1;
	}
	else
	{
		//连接成功
		int count = 0;
		//构造SQL语句
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
		//断开数据库连接
		mysql_close(con);
		mysql_library_end();
		return count;
	}
}

std::string PreHash(std::string blockname)
{
	std::string hash = "";
	MYSQL* con = new MYSQL;//数据库
	//连接数据库
	if (mysql_library_init(0, NULL, NULL))
	{
		hash = "could not initialize MySQL library";
		return hash;
	}
	mysql_init(con);
	MYSQL* ret = mysql_real_connect(con, DB_IP, DB_USERNAME, DB_PASSWORD, DB_NAME, 0, NULL, 0);
	if (!ret)
	{
		hash = "无法连接数据库";
		return hash;
	}
	else
	{
		//连接成功
		//构造SQL语句
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
		//断开数据库连接
		mysql_close(con);
		mysql_library_end();
		return hash;
	}
}

int AddCase(std::string blockname, std::string CaseJson)
{
	//解析Json
	Json::Value root;
	Json::Reader reader;
	int n = CaseJson.size();
	if (!reader.parse(CaseJson, root))
	{
		printf("json解析错误！");
		return -1;
	}
	//提取病历要素
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
	//链接数据库
	MYSQL* con = new MYSQL;//数据库
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
		//连接成功
		//构造SQL语句
		std::string query = "";
		mysql_query(con, query.c_str());
		query = "INSERT INTO `" + blockname + "` (`doc_id`, `pat_id`, `hospital`, `time`, `text`, `hash`) "
			"VALUES ('" + Casedoc_id + "', '" + Casepat_id + "', '" + Casehospital + "', '" + Casetime + "', '" + Casetext + "', '" + Casehash + "');";
		int res = mysql_query(con, query.c_str());
		//断开数据库连接
		mysql_close(con);
		mysql_library_end();
		return 0;
	}
}

std::string ReadBlock(std::string blockname)
{
	std::string Blockdata = "";
	MYSQL* con = new MYSQL;//数据库
	//连接数据库
	if (mysql_library_init(0, NULL, NULL))
	{
		Blockdata = "could not initialize MySQL library";
		return Blockdata;
	}
	mysql_init(con);
	MYSQL* ret = mysql_real_connect(con, DB_IP, DB_USERNAME, DB_PASSWORD, DB_NAME, 0, NULL, 0);
	if (!ret)
	{
		Blockdata = "无法连接数据库";
		return Blockdata;
	}
	else
	{
		//连接成功
		Json::Value root;
		Json::StreamWriterBuilder writerroot;
		std::unique_ptr<Json::StreamWriter> writeroot(writerroot.newStreamWriter());
		std::ostringstream osroot;
		//构造SQL语句，获取区块除了病例以外的信息
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
		//构造SQL语句，获取区块所有病例
		query = "SELECT * FROM `" + blockname + "`";
		res = mysql_query(con, query.c_str());
		result = mysql_store_result(con);
		if (result)
		{
			MYSQL_ROW row;
			unsigned int row_num = mysql_num_rows(result);//获取行数
			while ((row = mysql_fetch_row(result)))
			{
				//将每份病历组成Json形式(每行数据处理)
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
				//区块插入病例
				root["trans"].append(req);
			}
			mysql_free_result(result);
		}
		writeroot->write(root, &osroot);
		writeroot->~StreamWriter();
		Blockdata = osroot.str();
		//断开数据库连接
		mysql_close(con);
		mysql_library_end();
		return Blockdata;
	}
}

int DeleteBlock()
{

	MYSQL* con = new MYSQL;//数据库
	//连接数据库
	if (mysql_library_init(0, NULL, NULL))
	{
		//could not initialize MySQL library
		return -2;
	}
	mysql_init(con);
	MYSQL* ret = mysql_real_connect(con, DB_IP, DB_USERNAME, DB_PASSWORD, DB_NAME, 0, NULL, 0);
	if (!ret)
	{
		//无法连接数据库
		return -1;
	}
	else
	{
		//连接成功
		int count = CountBlock();
		//构造SQL语句
		std::string query = "";
		mysql_query(con, query.c_str());
		//删除所有区块
		for (int i = 0; i < count; i++)
		{
			query = "DROP TABLE block" + std::to_string(i);
			int res = mysql_query(con, query.c_str());
		}
		//删除区块记录
		query = "TRUNCATE blocknum";
		int res = mysql_query(con, query.c_str());
		//断开数据库连接
		mysql_close(con);
		mysql_library_end();
		return count;
	}
}

int WriteBlock(std::string Blockjson)
{
	//读取Json获取区块信息
	Json::Value root;
	Json::Reader reader;
	if (!reader.parse(Blockjson, root))
	{
		printf("json解析错误！");
		return -1;
	}
	std::string blockindex = root["blockindex"].asString();
	int e = CountBlock();
	if (atoi(blockindex.c_str()) != e)
		return -2;
	std::string prehash = root["prehash"].asString();
	std::string timestamp = root["timestamp"].asString();
	//添加区块
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
		//病例添加至区块中
		if (AddCase("block" + blockindex, Casejson) == -1)
			std::cout << "病历写入失败\n";
	}
}
std::vector<std::string> FindCase(int m, int n)
{
	std::string starttime = num2time(m);
	std::string endtime = num2time(n);
	std::vector<std::string> resultvec;
	MYSQL* con = new MYSQL;//数据库
	//连接数据库
	if (mysql_library_init(0, NULL, NULL))
	{
		//could not initialize MySQL library
		return resultvec;
	}
	mysql_init(con);
	MYSQL* ret = mysql_real_connect(con, DB_IP, DB_USERNAME, DB_PASSWORD, DB_NAME, 0, NULL, 0);
	if (!ret)
	{
		//无法连接数据库
		return resultvec;
	}
	else
	{
		//连接成功
		//构造SQL语句
		std::string query = "";
		mysql_query(con, query.c_str());
		//循环
		int count = CountBlock() - 1;//最高区块序号	
		int casenum = 0;//搜索出的病例数量
		bool haved = false;//还未找到满足要求的病例
		for (int i = count; i >= 0; i--)
		{
			query = "SELECT COUNT(*) FROM block" + std::to_string(i) + " WHERE `time`>'" + starttime + "' AND `time`<'" + endtime + "'";
			int res = mysql_query(con, query.c_str());
			MYSQL_RES* result = mysql_store_result(con);
			if (result)
			{
				MYSQL_ROW row = mysql_fetch_row(result);
				casenum = atoi(row[0]);//该时间段内的病例数量，若开始>0，则开始搜索
				if (casenum > 0)
				{
					//获取病例，并放入到数组中
					std::vector<std::string> temp = FindBlockCase(m, n, i);
					resultvec.insert(resultvec.end(), temp.begin(), temp.end());
					//已经开始找到满足要求的病例了
					haved = true;
				}
				else if (haved)
				{
					//判断之前是否有满足要求的病例，有则说明该时间段内已经没了
					break;
				}
			}
		}
		//断开数据库连接
		mysql_close(con);
		mysql_library_end();
		//对resultvec排序
		sort(resultvec.begin(), resultvec.end());
		return resultvec;
	}
}
std::vector<std::string> FindBlockCase(int m, int n, int index)
{
	std::string starttime = num2time(m);
	std::string endtime = num2time(n);
	std::vector<std::string> resultvec;
	MYSQL* con = new MYSQL;//数据库
	//连接数据库
	if (mysql_library_init(0, NULL, NULL))
	{
		//could not initialize MySQL library
		return resultvec;
	}
	mysql_init(con);
	MYSQL* ret = mysql_real_connect(con, DB_IP, DB_USERNAME, DB_PASSWORD, DB_NAME, 0, NULL, 0);
	if (!ret)
	{
		//无法连接数据库
		return resultvec;
	}
	else
	{
		//连接成功
		int count = CountBlock();
		//构造SQL语句
		std::string query = "";
		mysql_query(con, query.c_str());
		query = "SELECT * FROM block" + std::to_string(index) + " WHERE `time`>'" + starttime + "' AND `time`<'" + endtime + "'";
		int res = mysql_query(con, query.c_str());
		MYSQL_RES* result = mysql_store_result(con);
		if (result)
		{
			MYSQL_ROW row;
			unsigned int row_num = mysql_num_rows(result);//获取行数
			unsigned int field_num = mysql_num_fields(result);//获取列数
			while ((row = mysql_fetch_row(result)))
			{
				//组装病例形成字符串
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
				//计算md5
				MD5* md5 = new  MD5(jsondata);
				std::string md5str = md5->hexdigest();
				resultvec.push_back(md5str);
			}
			mysql_free_result(result);
		}
		//断开数据库连接
		mysql_close(con);
		mysql_library_end();
		return resultvec;
	}
}
std::string num2time(int n)
{
	//从1970年1月1日00:00:00所经过的秒数	
	std::stringstream ss;
	long long seconds = n;
	std::time_t t(seconds);
	ss << std::put_time(std::localtime(&t), "%F %T");
	return  ss.str();
}
int standard_to_stamp(char *str_time)//标准时间转化为unix时间戳
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
	/*printf("%d-%0d-%0d %0d:%0d:%0d\n", iY, iM, iD, iH, iMin, iS);*/   //标准时间格式例如：2016:08:02 12:12:30
	return (int)mktime(&stm);
}

std::vector<std::string> FindTraspoolCase(int m, int n)
{
	//std::string starttime = num2time(m);
	//std::string endtime = num2time(n);
	std::vector<std::string> resultvec;
	//解析病历池
	Json::Value value;
	Json::Reader reader_add;
	if (!reader_add.parse(trapoolstr, value) && !trapoolstr.empty())
		printf("病历池Json解析错误\n");
	//遍历病历池获取md5值
	int size = value.size();
	for (int i = 0; i < size; i++)
	{
		Json::Value jsontemp;
		jsontemp = value[i];
		//判断是否满足时间段
		if (standard_to_stamp(const_cast<char*>(jsontemp["time"].asString().c_str())) > n || standard_to_stamp(const_cast<char*>(jsontemp["time"].asString().c_str())) < m)
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
		resultvec.push_back(md5str);
	}
	//排序
	sort(resultvec.begin(), resultvec.end(), std::less<std::string>());
	return resultvec;
}
std::vector<char*> FindTraspoolCase_char(int m, int n)
{
	std::string starttime = num2time(m);
	std::string endtime = num2time(n);
	std::vector<char*> resultvec;
	//解析病历池
	Json::Value value;
	Json::Reader reader_add;
	if ((!reader_add.parse(trapoolstr, value) )|| trapoolstr.empty())
		printf("病历池Json解析错误\n");
	//遍历病历池获取md5值
	int size = value.size();
	for (int i = 0; i < size; i++)
	{
		Json::Value jsontemp;
		jsontemp = value[i];
		//判断是否满足时间段
		if (standard_to_stamp(const_cast<char*>(jsontemp["time"].asString().c_str())) > n || standard_to_stamp(const_cast<char*>(jsontemp["time"].asString().c_str())) < m)
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
		char* buff = new char[256];
		strcpy(buff,md5str.c_str());
		resultvec.push_back(buff);
	}
	//排序
	sort(resultvec.begin(), resultvec.end(), std::less<std::string>());
	return resultvec;
}