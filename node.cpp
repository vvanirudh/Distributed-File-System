#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <string>
#include <arpa/inet.h>
#include "md5percentile.hpp"

using namespace std;

int main(int argc,char** argv)
{
	if(argc==1||argc>2) //If the command line arguments are not given properly
	{
		printf("Please give the node number(0 - N-1) as a command line argument\n");
		return 0;
	}
	//Opening the configuration file
	ifstream file;
	file.open("FileMesh.cfg");
	string line;
	int nodeID = atoi(argv[1]); //read in the nodeID of the current node from the command line argument
	getline(file,line);
	int numNodes = atoi(line.c_str()); //read in the total number of nodes from the first line of the configuration file.
	for(int i=0;i<nodeID;i++)
	{
		getline(file,line); //Traverse through lines of the file
	}
	//Get the ip address and the port of the present node
	int colonindex = line.find(":"); 
	string ip_addr = line.substr(0,colonindex); //Ip address as a string
	int spaceindex = line.find(" ");
	string port = line.substr(colonindex+1,spaceindex-colonindex); //port number as a string
	string folderName = line.substr(spaceindex+1);
	cout<<numNodes<<","<<ip_addr<<","<<port<<","<<folderName<<endl;
	int portnum = atoi(port.c_str()); //Convert the port from string to int


	file.close();//close the file

	//Socket variables
	int sock;//socket descriptor
	struct sockaddr_in server,client,toNode; //structs which hold socket information
	socklen_t clientLen;//the variable which holds the length of client address
	char buff[1024];//the array which stored the message received from client(user program)
	int numOfBytesReceived = 0;//number of bytes received

	if((sock = socket(PF_INET, SOCK_DGRAM, 0))<0) //If socket is not created
	{
		printf("Socket not created. Failed!\n");
		return 0;
	}

	printf("Created socket %d\n",sock);

	server.sin_family = AF_INET; //set the family
	server.sin_port = htons(portnum); //assign the port
	server.sin_addr.s_addr = inet_addr(ip_addr.c_str()); //assign the ipaddress to the socket	
	memset(&(server.sin_zero),'\0',8);//set the remaining value of the struct to zeros

	if(bind(sock,(struct sockaddr*)&server,sizeof(server)) <0) //If the socket info is not bound properly to the socket
	{
		printf("Bind failed\n");
		return 0;
	}

	//Listening loop
	do{
		clientLen = sizeof(client);
		numOfBytesReceived = recvfrom(sock,buff,1024,0,(struct sockaddr*)&client,&clientLen); //Data received from client on the socket and stored in buff
		if(numOfBytesReceived<0) //If data receiving is failed
		{
			printf("Cannot receive data\n");
			continue;
		}

		if(numOfBytesReceived>0) //If data is successfully received
		{
			string msg = string(buff); // Retrieve the message as a string
			string md5 = msg.substr(0,32); //Retrieve the md5sum from the received data
			int nodeToBeSent = md5percentile(md5,numNodes); //Calculate md5sum % N where N = num of nodes
			if(nodeToBeSent!=nodeID) //If the node to which the request should be sent is not the same as the current node
			{
				//Open the configuration file and read the ipaddress and port of the destined node
				ifstream dfile("FileMesh.cfg");
				for(int i=0;i<nodeToBeSent+1;i++) {getline(dfile,line);}
				colonindex = line.find(":");
				ip_addr = line.substr(0,colonindex);//ip address of the destined node
				spaceindex = line.find(" ");
				port = line.substr(colonindex+1,spaceindex-colonindex);
				portnum = atoi(port.c_str()); //port number of the destined node

				//Put the corresponding info into a struct sockaddr
				toNode.sin_family = AF_INET; //family set
				toNode.sin_port = htons(portnum); //port number is assigned
				toNode.sin_addr.s_addr = inet_addr(ip_addr.c_str()); //ip address is assigned
				memset(&(toNode.sin_zero),'\0',8); //the rest of the struct is given value zero

				int numOfBytesSent = sendto(sock,buff,1024,0,(struct sockaddr*)&toNode,sizeof(struct sockaddr)); //Send the data received to the destined node
				if(numOfBytesSent<0) //If the sending failed
				{
					printf("Sending failed\n");
					return 0;
				}
			}
			else //if the current node is the destined node for the request
			{
				//Now it should open up a TCP connection with the client 
				//Retrieve the IP address and the port number of the client from the data received.
				string ipaddrOfClientAndPort = msg.substr(33);
				colonindex = ipaddrOfClientAndPort.find(":");
				string ipaddrOfClient = ipaddrOfClientAndPort.substr(0,colonindex); //ipaddress of the client in string format
				spaceindex = ipaddrOfClientAndPort.find(" ");
				string portOfClient = ipaddrOfClientAndPort.substr(colonindex+1,spaceindex-colonindex);
				int portnumOfClient = atoi(portOfClient.c_str()); //Port number of the client
				string operation = ipaddrOfClientAndPort.substr(ipaddrOfClientAndPort.find(" ")+1);

                int tcpsock;
                struct sockaddr_in user;
                socklen_t userLen;
                char buff[1024];
                int numOfBytes;

                if((tcpsock = socket(PF_INET,SOCK_STREAM,0))<0)
                {
                    printf("TCP Socket not created. Failed!\n");
                    return 0;
                }
                printf("TCP Socket created \n");
                
                if(operation=="store")
                {
                	user.sin_family = AF_INET;
                	user.sin_port = htons(portnumOfClient);
                	user.sin_addr.s_addr = inet_addr(ipaddrOfClient.c_str());
                	memset(&(user.sin_zero),'\0',8);

                	connect(tcpsock,(struct sockaddr*)user,sizeof(struct sockaddr));
                }

			}
		}		
	}while(1);

	close(sock);
	return 0;
}

