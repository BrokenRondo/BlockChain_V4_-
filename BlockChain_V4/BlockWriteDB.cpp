#include "BlockWriteDB.h"
int BlockWriteDB::Routine()
{
	std::cout << "���½ڵ��߳�����\n";
	std::ifstream fin("block.txt");
	std::string temp = "";
	std::string alldata = "";
	while (getline(fin, temp))
	{
		alldata += temp;
	}
	temp = "";
	fin.close();
	for (int i = 0; i < alldata.size()-2; i++)
	{
		if (alldata[i] == '#'&&alldata[i + 1] == '#'&&alldata[i + 2] == '#')
		{
			//��ȡJson
			std::cout << "д�����飺\n"<< WriteBlock(temp.c_str())<<std::endl;
			i = i + 2;
			temp = "";
		}
		else
		{
			temp += alldata[i];
		}
	}
	//std::ofstream of("block.txt");
	//of.close();
	return 0;
}