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

	// Create a socket using UCP IP
    int sockfd;
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
		perror("createsocket");
		return 2;
	}

    // Allow others to reuse the address
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        return 3;
    }

    // Bind address to socket
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(40000); // Short, network byte order
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        return 4;
    }



	return 0;
}