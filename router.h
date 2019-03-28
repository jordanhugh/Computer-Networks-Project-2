
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <iostream>
#include <errno.h>
#include <sstream>

#define MAXLINE 1024 

 class router
   {
    public:
    int sockfd; 
    char buffer[MAXLINE]; 

    int port;
    int nodePort;
    struct sockaddr_in addr, nodeAddr; 

    router(int argc, char **argv);
   };


