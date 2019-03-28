#include "router.h"

router::router(int argc, char **argv) 
{ 

    port = atoi(argv[1]);
    nodePort = atoi(argv[2]);
      
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
    { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
      
   memset(&addr, 0, sizeof(addr)); 
   memset(&nodeAddr, 0, sizeof(nodeAddr)); 
      
    // Filling server information 
    addr.sin_family    = AF_INET; // IPv4 
    addr.sin_addr.s_addr = INADDR_ANY; 
    addr.sin_port = htons(port); 
      
    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&addr, sizeof(addr)) < 0 )
    { 
        perror("Bind failed"); 
        exit(EXIT_FAILURE); 
    }

    //Information of the node we wish to connect to
    nodeAddr.sin_family = AF_INET;
    nodeAddr.sin_port = htons(nodePort);

    if (inet_aton("127.0.0.1", &nodeAddr.sin_addr) == 0)
    {
        perror("inet_aton failed"); 
        exit(EXIT_FAILURE); 
    }

   
} 


