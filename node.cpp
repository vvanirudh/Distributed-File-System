#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc,char** argv)
{
	int sock;
	struct sockaddr_in server;

