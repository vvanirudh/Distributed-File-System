/*
This file contains the prototypes for all the functions used in calculating md5%N
THe implementations of all this functions are present in md5percentile.cpp
*/


#include <string>

using namespace std;


string hextobin(string hex);
void generate(int*, int);
int md5percentile(string,int);
