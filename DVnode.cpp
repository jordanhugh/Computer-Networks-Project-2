#include "DVnode.h"
using namespace std;

bool DVnode::checkIfValid(){
	return validity;
}

char DVnode::getName(){
	return name;
}

int DVnode::getPort(){
	return port;
}

sockaddr_in DVnode::getAddr(){
	return addr;
}

int DVnode::getCost(){
	return cost;
}

void DVnode::setValidity(bool v){
	validity = v;
}

void DVnode::setName(char n){
	name = n;
}

void DVnode::setPort(int p){
	port = p;
}

void DVnode::setAddr(){
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port); // Short, network byte order
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));
}

void DVnode::setCost(int c){
	cost = c;
}