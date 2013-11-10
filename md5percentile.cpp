/*
This file contains our implementation of the md5%N calculation needed to find out the destined node for a given file. 
The prototypes for the functions defined here are present in the file md5percentile.hpp
*/


#include <iostream>
#include <string>
#include "md5percentile.hpp"

using namespace std;

//This function converts a given hex string to it's binary form
string hextobin(string hex)
{
	string bin = "";
	for(int i=0;i<32;i++) //Functions basically as a lookup table for the conversion from hex digit to binary digits
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
	return bin; //return the binary string
}

//This function generates all the remainders obtained when (2^K) is divided by N in the array arr where K = 0 to 127
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

//This function calculates the md5(given as hex) modulo N(num of nodes)
int md5percentile(string md5, int numNodes)
{
	string md5bin = hextobin(md5); //Convert the given md5 from hex format to binary format
	int array[128];
	generate(array,numNodes); //Generate all the remainders of 2^K when didvided by N
	int sum = 0;
	int j=0;
	for(int i=0;i<128;i++)
	{
		if(md5bin[i]=='1')
		{
			j++;
			sum = sum + array[127-i];//Lookup the remainders from the array and add them up
		}
	}
	sum = sum%numNodes;//take the remainder of the whole sum when divided by N
	return sum;
}
