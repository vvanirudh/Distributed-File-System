#include <iostream>
#include <string>
#include <bitset>

using namespace std;

string hextobin(string hex)
{
	string bin = "";
	for(int i=0;i<32;i++)
	{
		string p;
		if(hex[i]=='1') p="0001";
		if(hex[i]=='2') p="0010";
		if(hex[i]=='3') p="0011";
		if(hex[i]=='4') p="0100";
		if(hex[i]=='5') p="0101";
		if(hex[i]=='6') p="0110";
		if(hex[i]=='7') p="0111";
		if(hex[i]=='8') p="1000";
		if(hex[i]=='9') p="1001";
		if(hex[i]=='0') p="0000";
		if(hex[i]=='a') p="1010";
		if(hex[i]=='b') p="1011";
		if(hex[i]=='c') p="1100";
		if(hex[i]=='d') p="1101";
		if(hex[i]=='e') p="1110";
		if(hex[i]=='f') p="1111";

		bin = bin+p;
	}
	return bin;
}

bool compare(string p, string q)
{
	bitset<4> top(p);
	bitset<4> bot(q);

	int t = top.to_ulong();
	int b = bot.to_ulong();
	int r = t-b;
	if(r<0) return false;
	else return true;

}

string subtract(string p, string q)
{
	int length = p.length();
	bitset<length> top(p);
	bitset<length> bot(q);

	int t = top.to_ulong();
	int b = bot.to_ulong();
	int r = t-b;
	bitset<length> res(r);
	return res.to_string();
}

int main()
{
	string md5hex = "856b987c38f421107e5f73fa9fa46db5";
	string md5bin = hextobin(md5hex);
	
	int node = 15;
	string nodebin = "1111";

	int i=0;
	while(true)
	{
		string p = md5bin.substr(i,4);
		string q;
		if(compare(p,nodebin)==true)
		{
			q = subtract(p,nodebin);
		}
		else
		{
			
		}
	}

	return 0;
}
