#include "RSASIG.h"
#include "Jsoncpp/json.h"

using namespace CryptoPP;
using namespace std;
#pragma comment(lib, "cryptlib.lib")
//auto keys = RsaGenerateHexKeyPair(2048);
extern string signatures;
extern KeyPairHex keys;
struct Public_Key
{
	string IP;
	string key;
};
extern vector<Public_Key> public_keys;
extern string LocalIP;



bool verify_sig(string sig, string peer_ip, string hash)
{
	string public_key;
	for(unsigned int i=0;i<public_keys.size();i++)
	{
		if (public_keys[i].IP == peer_ip)
		{
			public_key = public_keys[i].key;
			break;
		}
	}
	if (RsaVerifyString(public_key, hash, sig))
	{
		Json::Value root;
		Json::Reader reader;
		Json::StyledWriter swriter;
		if (signatures.empty())
		{
			root["hash"] = hash;
			Json::Value Signature;
			Signature["IP"] = peer_ip;
			Signature["sig"] = sig;
			root["sigs"].append(Signature);
			signatures = "";
			signatures = swriter.write(root);
		}
		else if (!reader.parse(signatures, root))
		{
			cerr << "解析signatures出错\n";
		}
		else
		{
			Json::Value value;
			value["IP"] = peer_ip;
			value["sig"] = sig;
			root["sigs"].append(value);
			signatures = "";
			signatures = swriter.write(root);
		}
		return true;
	}
	return false;
}

bool verify_sig_follower(string sig, string peer_ip, string hash)
{
	string public_key;
	for (unsigned int i = 0; i < public_keys.size(); i++)
	{
		if (public_keys[i].IP == peer_ip)
		{
			public_key = public_keys[i].key;
			break;
		}
	}
	if (RsaVerifyString(public_key, hash, sig))
	{
		return true;
	}
	return false;
}
string signature(string hash)
{
	auto signature(RsaSignString(keys.privateKey, hash));
	return signature;
}
/*
 签名的json格式：
 {
 "hash":"343df343eda11786e",
 "sigs":
 [
 {
 "IP":"192.168.105",
 "sig":"xxx"
 },
 {
 "IP":"192.168.0.106",
 "sig":"xxx"
 }
 ]
 }
 */