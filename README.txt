List of relevant files:
-----------------------

	1. user.cpp : The program that implements all the functionality of a client/user.
	2. node.cpp : The program that implements all the functionality of a server/node.
	3. md5percentile.hpp : A header file which details all the helper functions used in the implementation of the project.
	4. md5percentile.cpp : This file contains the implementations of all the helper functions used in the project.
	5. FileMesh.cfg : This is a sample configuration file that we have used during our testing.
 
Compilation instructions:
-------------------------

	1. Type 'make' to compile all the relevant files and generate two executables.

Configuration file(s):
----------------------

	Our project doesn't use any configuration files except the one that is used to configure the Filemesh namely, 'FileMesh.cfg'

Running instructions:
---------------------

	1. First compile using 'make'
	2. If you want the user to run on your machine, type './user <IP> <PORT>'. Include the IP address of your machine and the port you want the socket to run on, in <IP> and <PORT> respectively.
	3. If you want the node to run on your machine, type './node <nodeID>'. Include the nodeId of the node that you want to run on your machine in <nodeID>. The value of nodeID starts from 0.

Directories:
------------

	The node <nodeID> uses the folder named according to the one given in the configuration file.

TEAM MEMBERS:
Anirudh Vemula
HarshaVardhan Kode
Nishant Nori
