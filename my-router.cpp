
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
#include <fstream>
#include <pthread.h>
//#include <boost/thread.hpp>

#define MAXLINE 1024 

std::string findNeighbour(int& i, char** router);
void router_setup(int port, int& sockfd);
int findPort(std::string neighbour);
void* node_router(void *threadarg);

struct thread_data {
   int  thread_port;
   char* origin_port;
   int origin_sockfd;
};


main(int argc, char **argv)
{
    int i = 0;
    int sockfd = 0;

    router_setup(atoi(argv[1]), sockfd);

    std::string neighbours;
    neighbours = findNeighbour(i, argv);

    std::cout << "My neighbours are:\n" << neighbours << std::endl;
    
    std::istringstream b(neighbours);
    std::string line[i]; 
    int j = 0;
    while (std::getline(b, line[j]) && j < i-1) 
    {
        if(j < i-1) {++j;}
    }

    int port[j];

    for(int j=0; j < i; j++)
    {
        port[j] = findPort(line[j]);
    }
   pthread_t threads[j];
   struct thread_data td[j];
   int rc;
   int t;

   for( t = 0; t <= j; t++ ) {
      std::cout <<"main() : creating thread, " << t << std::endl;
      td[t].thread_port = port[t];
      td[t].origin_port = argv[1];
      td[t].origin_sockfd = sockfd;
      rc = pthread_create(&threads[t], NULL, node_router, (void *)&td[t]);
      if (rc)
      {
        perror("thread creation failed"); 
        exit(EXIT_FAILURE); 
      }
   }

    pthread_exit(NULL);
    return 0;
}

std::string findNeighbour( int& i, char** router)
{
    std::ifstream  afile;
    afile.open("topology.txt");

    std::string data;
    std::string body;
    std::string input;
    int l = atoi(router[1]);
    char letter = (l - 10000) + 65;
 
    if (afile.is_open())
    {
        while ( std::getline (afile, data) )
        {       
            if (data.at(0) == letter)
            {
                body = body + data + "\n";
                i++;
            }
        }
        afile.close();
    }

    return body;
}

int findPort(std::string neighbour)
{
    char neigh[11];
    strcpy(neigh, neighbour.c_str());
    std::string port;
    for(int i = 4; i < 9; i++)
    {
        port = port + neigh[i];
    }
    return atoi(port.c_str());
}
void router_setup(int port, int& sockfd)
{
    struct sockaddr_in addr;

     // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
    { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
    memset(&addr, 0, sizeof(addr)); 
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
}

void *node_router(void *threadarg) {      
    struct thread_data *port_data;
    port_data = (struct thread_data *) threadarg;

    int sockfd = port_data->origin_sockfd;
    char buffer[MAXLINE]; 
    struct sockaddr_in nodeAddr; 

    int port = atoi(port_data->origin_port);
    int nodePort = port_data->thread_port;

    std::cout << "port " << port << " is connecting to port " << nodePort << std::endl;

    fd_set readFds;
    fd_set errFds;
    fd_set watchFds;
    //Clears the bit for the file descriptor fd in the file descriptor set fdset
    FD_ZERO(&readFds);
    FD_ZERO(&errFds);
    FD_ZERO(&watchFds);
    
    int maxSockfd = sockfd;

    FD_SET(sockfd, &watchFds);

    memset(&nodeAddr, 0, sizeof(nodeAddr)); 
    //Information of the node we wish to connect to
    nodeAddr.sin_family = AF_INET;
    nodeAddr.sin_port = htons(nodePort);


    if (inet_aton("127.0.0.1", &nodeAddr.sin_addr) == 0)
    {
        perror("inet_aton failed"); 
        exit(EXIT_FAILURE); 
    }
    int n; 
    socklen_t len;
    int timeout;
    // initialize timer (2s)                                                                                                                                                                                  
    struct timeval tv;
    bool connection = true;
    while (connection == true) 
    {
        // set up watcher  , checks to see data is being recieved    
                                                                                                                                                                                            
        int nReadyFds = 0;
        readFds = watchFds;
        errFds = watchFds;
        tv.tv_sec = 5;
        tv.tv_usec = 0;

       // std::cout << "setting up watcher" << std::endl;
        if ((nReadyFds = select(maxSockfd + 1, &readFds, NULL, &errFds, &tv)) == -1)
        {
        std::cout << "not working" << std::endl;
        perror("select");
        exit(EXIT_FAILURE); 
        }

        if (nReadyFds == 0) 
        {
            //std::cout << "send hello to port: " << nodePort << std::endl;
            sendto(sockfd, "hello", strlen("hello"), MSG_CONFIRM, (const struct sockaddr*) &nodeAddr, sizeof(nodeAddr));
            timeout++;
            if (timeout == 10)
            {
                std::cout << "connection to port " << nodePort << " will be dropped\n";
                connection = false;
            }
        }
        for(int fd = 0; fd <= maxSockfd; fd++) 
        {   // get one socket for reading
            if (FD_ISSET(fd, &readFds))
            {
                n = recvfrom(sockfd, (char*)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*) &nodeAddr, &len);
                //std::cout << buffer << " from port " << nodePort << std::endl;
                if(buffer != NULL) timeout = 0; 
            }
        }  
    }
    std::cout << "thread is ending\n";
    pthread_exit(NULL);
}