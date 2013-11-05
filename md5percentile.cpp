#include <iostream>
#include <string>

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

void generate(int* arr, int n)
{
	arr[0]=1;
	for(int i=1;i<128;i++)
	{
		int b = 2*arr[i-1];
		if(b>=n) b = b-n;
		arr[i] = b;
	}
}

int md5percentile(string md5, int numNodes)
{
	string md5bin = hextobin(md5hex);
	int array[128];
	generate(array,numNodes);
	int sum = 0;
	int j=0;
	for(int i=0;i<128;i++)
	{
		if(md5bin[i]=='1')
		{
			j++;
			sum = sum + array[127-i];
		}
	}
	sum = sum%numNodes;
	return sum;
}
