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
using namespace std;

int main(int argc, char **argv) {

	if (argc < 2) {
		perror("arguments");
		return 1;
	}

	return 0;
}