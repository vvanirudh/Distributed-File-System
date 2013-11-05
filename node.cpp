#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <string>

using namespace std;

int main(int argc,char** argv)
{
	ifstream file;
	file.open("FileMesh.cfg");
	string line;
	int nodeID = atoi(argv[1]);
	for(int i=0;i<nodeID+1;i++)
	{
		getline(file,line);
	}

	int colonindex = line.find(":");
	string ip_addr = line.substr(0,colonindex);
	int spaceindex = line.find(" ");
	string port = line.substr(colonindex+1,spaceindex-colonindex);
	cout<<ip_addr<<","<<port<<endl;

	file.close();

	int sock;
	struct sockaddr_in server,client;
	socklen_t clientLen;
	char buff[1024];
	int n;

	if((sock = socket(PF_INET, SOCK_DGRAM, 0))<0)
	{
		printf("Socket not created. Failed!\n");
		return 0;
	}

	printf("Created socket %d\n",sock);

	server.sin_family = AF_INET;
	server.sin_port = htons(5000);
	server.sin_addr.s_addr = INADDR_ANY;
	memset(&(server.sin_zero),'\0',8);

	if(bind(sock,(struct sockaddr*)&server,sizeof(server)) <0)
	{
		printf("Bind failed\n");
		return 0;
	}

	do{
		clientLen = sizeof(client);
		n = recvfrom(sock,buff,1024,0,(struct sockaddr*)&client,&clientLen);
		if(n<0)
		{
			printf("Cannot receive data\n");
			continue;
<<<<<<< HEAD
		}
		if(n>0)
		{
			string msg = string(buff);
			string md5 = msg.substr(0,64);
				
		}		
=======
		}	
>>>>>>> parent of fa50465... Server now reading from file and running
	}while(1);

	return 0;
}

