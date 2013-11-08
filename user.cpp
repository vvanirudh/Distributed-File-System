#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <string>
#include <arpa/inet.h>
#include "md5percentile.hpp"
#include <unistd.h>
#include <string.h>
#include <vector>
#include <time.h>
#include <cstdlib>
#include <sstream>

using namespace std;

int main(int argc, char** argv)
{
	int udpsock;
	struct sockaddr_in myaddr,toNode;
	char buff[1024];


	string IP = "127.0.0.1";
	int PORT = 4000;

	int randomNode;
	vector<string> nodes;

	ifstream file;
	file.open("FileMesh.cfg");
	string line;
	while(getline(file,line))
	{
		if(line!="") nodes.push_back(line);
	}
	file.close();
	cout<<"What node should I send the request to? Give the nodeID"<<endl;
	cin>>randomNode;
	if(randomNode>nodes.size() || randomNode<0)
	{
		cout<<"Invalid nodeID"<<endl;
		return 0;
	}

	string operation;
	cout<<"What operation?(store/retrieve)"<<endl;
	cin>>operation;

	string filename;
	cout<<"Enter the file name that you want to retrieve/store"<<endl;
	cin>>filename;

	string md5sum="";

	if(operation=="store")
	{
		system(("md5 "+filename +"| awk '{printf $4}' > temp").c_str());  // CHANGE THIS SO THAT IT RUNS PERFECTLY ON LINUX. THIS WORKS ON MAC
		ifstream readfile;
		readfile.open("temp");
		getline(readfile,line);
		md5sum=line;
		readfile.close();
		system("rm temp");
	}
	else if(operation=="retrieve")
	{
		ifstream md5file;
		md5file.open("md5mapping.txt");
		while(getline(md5file,line))
		{
			string name = line.substr(0,line.find(" "));
			if(name==filename) md5sum = line.substr(line.find(" ")+1);
		}
		md5file.close();
		if(md5sum=="")
		{
			cout<<"file not found"<<endl;
			return 0;
		}
	}


	if((udpsock = socket(PF_INET,SOCK_DGRAM,0)) < 0)
	{
		printf("UDP socket creation failed!\n");
		return 0;
	}

	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = inet_addr(IP.c_str());
	myaddr.sin_port = htons(PORT);
	memset(&(myaddr.sin_zero),'\0',8);

	if(bind(udpsock,(struct sockaddr*)&myaddr,sizeof(myaddr))<0)
	{
		printf("UDP socket binding failed\n");
		return 0;
	}

	toNode.sin_family = AF_INET;
	string toNodeAddress = nodes[randomNode];
	toNode.sin_addr.s_addr = inet_addr((toNodeAddress.substr(0,toNodeAddress.find(":"))).c_str());
	toNode.sin_port = htons(atoi((toNodeAddress.substr(toNodeAddress.find(":")+1,toNodeAddress.find(" ")-toNodeAddress.find(":"))).c_str()));
	memset(&(toNode.sin_zero),'\0',8);

	string portnum;
	stringstream out;
	out<<PORT;
	portnum = out.str();
	string request = md5sum+" "+IP+":"+portnum+" "+operation;
	

	int numOfBytesSent = sendto(udpsock,request.c_str(),1024,0,(struct sockaddr*)&toNode,sizeof(struct sockaddr));
	if(numOfBytesSent<0)
	{
		cout<<"Sending failed"<<endl;
		return 0;
	}


	return 0;
}