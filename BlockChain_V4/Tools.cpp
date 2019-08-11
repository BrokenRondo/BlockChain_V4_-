#include "Tools.h"
string getIP()
{
	string Ip;
	// PIP_ADAPTER_INFO结构体指针存储本机网卡信息
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	// 得到结构体大小,用于GetAdaptersInfo参数
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	// 记录网卡数量 
	int netCardNum = 0;
	// 记录每张网卡上的IP地址数量
	int IPnumPerNetCard = 0;
	//调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量;其中stSize参数既是一个输入量也是一个输出量
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		/*如果函数返回的是ERROR_BUFFER_OVERFLOW
		则说明GetAdaptersInfo参数传递的内存空间不够,同时其传出stSize,表示需要的空间大小
		这也是说明为什么stSize既是一个输入量也是一个输出量
		释放原来的内存空间*/
		//delete pIpAdapterInfo;
		// 重新申请内存空间用来存储所有网卡信息
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
		//再次调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量  
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	}
	if (ERROR_SUCCESS == nRel)
	{
		while (pIpAdapterInfo)
		{
			string s = pIpAdapterInfo->Description;
			if (s.find("wireless") != -1 || s.find("Wireless") != -1|| s.find("Realtek") != -1)
			//if (s.find("Realtek") != -1 || s.find("PCIe") != -1)
			{
				//可能网卡有多IP,因此通过循环去判断
				IP_ADDR_STRING* pIpAddrString = &(pIpAdapterInfo->IpAddressList);
				Ip = pIpAddrString->IpAddress.String;//获取无线网卡IP
				pIpAdapterInfo = pIpAdapterInfo->Next;
				//printf("────────────────────────────────────────\n");
				break;
			}
			pIpAdapterInfo = pIpAdapterInfo->Next;
		}
	}
	return Ip;
}
string gettime()
{
	string pt = "";
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	pt += std::to_string(st.wYear) + "-" + std::to_string(st.wMonth) + "-" + std::to_string(st.wDay) + " " + std::to_string(st.wHour) + ":" + std::to_string(st.wMinute) + ":" + std::to_string(st.wSecond);
	return pt;
}
int gettime_minute()
{
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	int minute = st.wMinute;
	return minute;
}
int gettime_second()
{
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	int second = st.wSecond;
	return second;
}
int gettime_msecond()
{
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	int msecond = st.wMilliseconds;
	return msecond;
}
void copy(string src, string dst)
{
	using namespace std;
	ifstream in(src, ios::binary);
	ofstream out(dst, ios::binary);
	if (!in.is_open())
	{
		cout << "error open file " << src << endl;
		//exit(EXIT_FAILURE);
		return;
	}
	if (!out.is_open())
	{
		cout << "error open file " << dst << endl;
		//exit(EXIT_FAILURE);
		return;
	}
	if (src == dst)
	{
		cout << "the src file can't be same with dst file" << endl;
		//exit(EXIT_FAILURE);
		return;
	}
	char buf[2048];
	long long totalBytes = 0;
	while (in)
	{
		//read从in流中读取2048字节，放入buf数组中，同时文件指针向后移动2048字节
		//若不足2048字节遇到文件结尾，则以实际提取字节读取。
		in.read(buf, 2048);
		//gcount()用来提取读取的字节数，write将buf中的内容写入out流。
		out.write(buf, in.gcount());
		totalBytes += in.gcount();
	}
	in.close();
	out.close();
}