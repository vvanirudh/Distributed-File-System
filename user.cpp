/*
This file contains all the functionality implemented by the user to store/retrieve a file in/from a node.
*/

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
	
	//IP and port of the TCP server socket are to be given through commandline
	if(argc < 3){
		cout<<"Please enter the IP and Port of the tcp socket to be created at the client as commandline arguments\n";
		return 0;
	}

	//IP and port of the TCP server socket that is to be opened are given through command line (given manually) 
	string IP (argv[1]);
	int PORT = (atoi(argv[2]));

	//Node to send the request
	int randomNode;
	//Info of all the nodes stored in this vector from the cfg file
	vector<string> nodes;

	ifstream file;
	file.open("FileMesh.cfg");
	string line;
	while(getline(file,line))
	{
		if(line!="") nodes.push_back(line);
	}
	file.close();
	
	//taking input of the node to send the request 	
	cout<<"What node should I send the request to? Give the nodeID"<<endl;
	cin>>randomNode;
	//checking if the node is valid or not
	if(randomNode>nodes.size() || randomNode<0)
	{
		cout<<"Invalid nodeID"<<endl;
		return 0;
	}

	//taking input of the operation
	string operation;
	cout<<"What operation?(store/retrieve)"<<endl;
	cin>>operation;
	
	//taking input of the filename
	string filename;
	cout<<"Enter the file name that you want to retrieve/store"<<endl;
	cin>>filename;

	string md5sum="";

	if(operation=="store")
	{
		//this function gets the md5 sum by using a system call and stores it
		system(("md5sum "+filename +"| awk '{printf $1}' > temp").c_str());  // CHANGE THIS SO THAT IT RUNS PROPERLY ON THE PARTICULAR OS
		ifstream readfile;
		readfile.open("temp");
		getline(readfile,line);
		md5sum=line;
		readfile.close();
		system("rm temp");
	}
	else if(operation=="retrieve")
	{
		// If operation is retrieve, the user gives the md5sum itself
		md5sum = filename;

	}

	//Opens the UDP socket
	if((udpsock = socket(PF_INET,SOCK_DGRAM,0)) < 0)
	{
		printf("UDP socket creation failed!\n");
		return 0;
	}

	// Setting the myaddr(sockaddr struct)'s values to user's address
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = inet_addr(IP.c_str());
	myaddr.sin_port = htons(PORT);
	memset(&(myaddr.sin_zero),'\0',8);

	//Binding the udp socket to the address and port
	if(bind(udpsock,(struct sockaddr*)&myaddr,sizeof(myaddr))<0)
	{
		printf("UDP socket binding failed\n");
		return 0;
	}

	// Setting the toNode(sockaddr struct)'s values to node's address
	toNode.sin_family = AF_INET;
	string toNodeAddress = nodes[randomNode];
	toNode.sin_addr.s_addr = inet_addr((toNodeAddress.substr(0,toNodeAddress.find(":"))).c_str());
	toNode.sin_port = htons(atoi((toNodeAddress.substr(toNodeAddress.find(":")+1,toNodeAddress.find(" ")-toNodeAddress.find(":"))).c_str()));
	memset(&(toNode.sin_zero),'\0',8);

	// creating the message to be sent on the udp socket
	string portnum;
	stringstream out;
	out<<PORT;
	portnum = out.str();
	string request = md5sum+" "+IP+":"+portnum+" "+operation;
	
	//sending the message over UDP
	int numOfBytesSent = sendto(udpsock,request.c_str(),1024,0,(struct sockaddr*)&toNode,sizeof(struct sockaddr));
	if(numOfBytesSent<0)
	{
		cout<<"Sending failed"<<endl;
		return 0;
	}

	close(udpsock);
	int tcpsock_l,tcpsock;


	//Creating a TCP socket after the udp message is sent
	if((tcpsock_l = socket(PF_INET,SOCK_STREAM,0)) < 0)
	{
		printf("TCP socket creation failed!\n");
		return 0;
	}


	//Binding the socket to the ipaddr and port sent to the node  
	if(bind(tcpsock_l,(struct sockaddr*)&myaddr,sizeof(myaddr))<0)
	{
		printf("TCP socket binding failed\n");
		return 0;
	}

	//Listening on the socket
	if(listen(tcpsock_l, 10) < 0){
		printf("listening failed\n");
		return 0;		
	}
	//Accept (blocking) function 
	socklen_t size = sizeof(struct sockaddr_in);
	if ((tcpsock = accept(tcpsock_l, (struct sockaddr *)&connNode, &size )) < 0 ){
		printf("accepting failed\n");
		return 0;	
	}

	//After accepting connection
	if(operation == "store"){
		//Opening the input file stream
		ifstream fileIn;
		int len,sentBytes;
		string line;
		fileIn.open(filename.c_str());
		if(fileIn.is_open()){
			while (getline (fileIn,line)){
				//sending the file line by line in the loop
				line+="\n";
				len = strlen(line.c_str());
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
	//Retrieve
	else if(operation == "retrieve"){	
		
        char buffer[1024];
        int numOfBytes;
  		ofstream storefile;
		//File is stored as <md5sum>.txt		
		storefile.open((md5sum+".txt").c_str());
		
		while( (numOfBytes = recv(tcpsock,buffer,1024,0)) > 0 ){
			//Adding a \0 at the end of the bytes to get the whole message as a string
			buffer[numOfBytes] = '\0';
			string data(buffer);
			//Writing into the file
			storefile<<data;
		}
		//closing the filestream
		storefile.close();
		
		//error checking
		if(numOfBytes<0)
		{
			printf("Error in receiving data\n");
		}
		else if(numOfBytes==0)
		{
			printf("Finished retrieving the file\n");
		}


	}
	//Closing the sockets
	close(tcpsock_l);
	close(tcpsock);

	return 0;
}
