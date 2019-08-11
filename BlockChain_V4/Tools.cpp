#include "Tools.h"
string getIP()
{
	string Ip;
	// PIP_ADAPTER_INFO�ṹ��ָ��洢����������Ϣ
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	// �õ��ṹ���С,����GetAdaptersInfo����
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	// ��¼�������� 
	int netCardNum = 0;
	// ��¼ÿ�������ϵ�IP��ַ����
	int IPnumPerNetCard = 0;
	//����GetAdaptersInfo����,���pIpAdapterInfoָ�����;����stSize��������һ��������Ҳ��һ�������
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		/*����������ص���ERROR_BUFFER_OVERFLOW
		��˵��GetAdaptersInfo�������ݵ��ڴ�ռ䲻��,ͬʱ�䴫��stSize,��ʾ��Ҫ�Ŀռ��С
		��Ҳ��˵��ΪʲôstSize����һ��������Ҳ��һ�������
		�ͷ�ԭ�����ڴ�ռ�*/
		//delete pIpAdapterInfo;
		// ���������ڴ�ռ������洢����������Ϣ
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
		//�ٴε���GetAdaptersInfo����,���pIpAdapterInfoָ�����  
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
				//���������ж�IP,���ͨ��ѭ��ȥ�ж�
				IP_ADDR_STRING* pIpAddrString = &(pIpAdapterInfo->IpAddressList);
				Ip = pIpAddrString->IpAddress.String;//��ȡ��������IP
				pIpAdapterInfo = pIpAdapterInfo->Next;
				//printf("��������������������������������������������������������������������������������\n");
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
		//read��in���ж�ȡ2048�ֽڣ�����buf�����У�ͬʱ�ļ�ָ������ƶ�2048�ֽ�
		//������2048�ֽ������ļ���β������ʵ����ȡ�ֽڶ�ȡ��
		in.read(buf, 2048);
		//gcount()������ȡ��ȡ���ֽ�����write��buf�е�����д��out����
		out.write(buf, in.gcount());
		totalBytes += in.gcount();
	}
	in.close();
	out.close();
}