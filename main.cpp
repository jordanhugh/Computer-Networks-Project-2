
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
#include "router.h"

main(int argc, char **argv){

    router node1(argc, argv);

    char* hold;

    hold = argv[1];
    argv[1] = argv[2];
    argv[2] = hold;

    router node2(argc, argv);

    int port1 = node1.port;
    int port2 = node2.port;

    std::cout << "Port number 1: " << port1 << std::endl;
    std::cout << "Port number 2: " << port2 << std::endl;
    
    return 0;
}