#include "DVentry.h"
using namespace std;

bool DVentry::checkIfValid(){
	return validity;
}

char DVentry::getName(){
	return name;
}

int DVentry::getPort(){
	return port;
}

sockaddr_in DVentry::getAddr(){
	return addr;
}

int DVentry::getCost(){
	return cost;
}

void DVentry::setValidity(bool v){
	validity = v;
}

void DVentry::setName(char n){
	name = n;
}

void DVentry::setPort(int p){
	port = p;
}

void DVentry::setAddr(){
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port); // Short, network byte order
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));
}

void DVentry::setCost(int c){
	cost = c;
}