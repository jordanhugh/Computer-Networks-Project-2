#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdint.h>
#include <pthread.h>
using namespace std;

class DVnode{
private: // For Example:
	bool validity; // validity = true
	char name; // name = 'B'
	int port; //port = '10001'
	sockaddr_in addr; // addr = otherAddr
	int cost; //cost = 4
	
public: 
	//Get'er and Set'er Methods
	bool checkIfValid(); 
	char getName();
	int getPort();
	sockaddr_in getAddr();
	int getCost();
	void setValidity(bool v);
	void setName(char n);
	void setPort(int p);
	void setAddr();
	void setCost(int c);
};