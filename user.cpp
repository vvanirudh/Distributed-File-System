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
	struct sockaddr_in myaddr,toNode,connNode;
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
		system(("md5sum "+filename +"| awk '{printf $1}' > temp").c_str());  // CHANGE THIS SO THAT IT RUNS PERFECTLY ON LINUX. THIS WORKS ON MAC
		ifstream readfile;
		readfile.open("temp");
		getline(readfile,line);
		md5sum=line;
		readfile.close();
		system("rm temp");
	}
	else if(operation=="retrieve")
	{
		/*ifstream md5file;
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
		}*/
		md5sum = filename;

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

	close(udpsock);
	int tcpsock_l,tcpsock;

	if((tcpsock_l = socket(PF_INET,SOCK_STREAM,0)) < 0)
	{
		printf("TCP socket creation failed!\n");
		return 0;
	}


	if(bind(tcpsock_l,(struct sockaddr*)&myaddr,sizeof(myaddr))<0)
	{
		printf("TCP socket binding failed\n");
		return 0;
	}
	int optval =1 ;
	setsockopt(tcpsock_l,SOL_SOCKET,SO_REUSEADDR, &optval, sizeof(optval));

	if(listen(tcpsock_l, 10) < 0){
		printf("listening failed\n");
		return 0;		
	}
	socklen_t size = sizeof(struct sockaddr_in);
	if ((tcpsock = accept(tcpsock_l, (struct sockaddr *)&connNode, &size )) < 0 ){
		printf("accepting failed\n");
		return 0;	
	}

	if(operation == "store"){
		ifstream fileIn;
		int len,sentBytes;
		string line;
		fileIn.open(filename.c_str());
		if(fileIn.is_open()){
			while (getline (fileIn,line)){
				line+="\n";
				len = strlen(line.c_str());
				//len+1 characters are sent because the '\0' character is also to be sent
				if( (sentBytes = send(tcpsock,line.c_str(), len, 0)) < 0) {
					printf("sending failed\n");
					return 0;
				}
			}
		}
		else{
			printf("Failed to open file\n");
		}
	}
	else if(operation == "retrieve"){	
        char buffer[1024];
        int numOfBytes;
  		ofstream storefile;
		storefile.open((md5sum+".txt").c_str());

		while( (numOfBytes = recv(tcpsock,buffer,1024,0)) > 0 ){
			buffer[numOfBytes] = '\0';
			string data(buffer);
			//cout<<numOfBytes<<"\n" ;
			cout<<data;
			storefile<<data;
		}

		storefile.close();

		if(numOfBytes<0)
		{
			printf("Error in receiving data\n");
			//return 0;
		}
		else if(numOfBytes==0)
		{
			printf("Finished retrieving the file\n");
			//return 0;
		}


	}
	close(tcpsock_l);
	close(tcpsock);

	return 0;
}
