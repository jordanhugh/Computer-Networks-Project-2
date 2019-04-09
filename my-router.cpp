
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
#include <vector>
#include <unistd.h>
#include "DV.h"
#include <chrono>
#include <ctime>

#define MAXLINE 1024 

struct thread_data 
{
   int  thread_port;
   int  origin_port;
};

struct packet_data
{
    std::string type;
    int source;
    int dest;
    int length;
    std::string payload;
};

std::string findNeighbour(int& i, int myPort);
int findPort(std::string neighbour);
void router_setup(int port, int& sockfd);
void* node_router(void *threadarg);
std::string makeDataPacket(int source, int dest);
bool checkIfData(std::string packet);
packet_data getData(std::string message, int port, int nodePort);
void sendDataPacket(std::string packet, int source, int dest);

DV dv;

main(int argc, char **argv)
{
    dv.clearFile();
    int i = 0;
    int sockfd = 0;
    int myPort = atoi(argv[1]);

    router_setup(myPort, sockfd);

    if (myPort == 10006)
    {
        std::string dataPacket = makeDataPacket(10000, 10003);
        sendDataPacket(dataPacket, 10006, 10000);
        return 0;
    }

    std::string neighbours;
    neighbours = findNeighbour(i, myPort);

    std::cout << "My neighbours are:\n" << neighbours << std::endl;

    std::istringstream b(neighbours);
    std::string line[i]; 
    int j = 0;
    while (std::getline(b, line[j]) && j < i-1) 
    {
        if(j < i-1) ++j;
    }

    int port[j];

    for(int j=0; j < i; j++)
    {
        port[j] = findPort(line[j]);
    }

    pthread_t threads[j];
    struct thread_data td[j];
    int rc;

    for(long int t = 0; t < i; t++ ) 
    {
      td[t].thread_port = port[t];
      td[t].origin_port = myPort;
      rc = pthread_create(&threads[t], NULL, node_router, (void *)&td[t]);
      if (rc)
      {
        perror("thread creation failed"); 
        exit(EXIT_FAILURE); 
      }
    }

    for(int t = 0 ; t < i; ++t)
    {
        void* status;
        int k = pthread_join(threads[t], &status);
        if (k != 0)
        {
            perror("thread join failed"); 
            exit(EXIT_FAILURE); 
        }
    }

    pthread_exit(NULL);
    return 0;
}

std::string findNeighbour( int& i, int myPort)
{
    std::ifstream  afile;
    afile.open("topology.txt");

    std::string data;
    std::string body;
    std::string input;
    int l = myPort;
    char letter = (l - 10000) + 65;
    dv.setName(letter);
    dv.fillUpTable("topology.txt");
    dv.printTable();
    
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

void *node_router(void *threadarg) 
{      
    struct thread_data *port_data;
    port_data = (struct thread_data *) threadarg;

    int sockfd = 3;
    char buffer[MAXLINE]; 
    struct sockaddr_in nodeAddr; 

    int port = port_data->origin_port;
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
    int timeout = 0;
    // initialize timer (2s)                                                                                                                                                                                  
    struct timeval tv;
    while (true) 
    {
        // set up watcher  , checks to see data is being recieved                                                                                                                                                                                       
        int nReadyFds = 0;
        readFds = watchFds;
        errFds = watchFds;
        tv.tv_sec =  5;
        tv.tv_usec = 0;

        if ((nReadyFds = select  (maxSockfd + 1, &readFds, NULL, &errFds, &tv)) == -1)
        {
            perror("select");
            exit(EXIT_FAILURE); 
        }

    
        if (nReadyFds == 0) 
        {
            std::string message = dv.sendPacket();
            vector<uint8_t> wire1 = dv.encode(message);
            sendto(sockfd, (const char*)wire1.data(), wire1.size(), MSG_CONFIRM, (const struct sockaddr*) &nodeAddr, sizeof(nodeAddr));
        }

        for(int fd = 0; fd <= maxSockfd; fd++) 
        {   // get one socket for reading
            if (FD_ISSET(fd, &readFds))
            {
                n = recvfrom(sockfd, (char*)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*) &nodeAddr, &len);

                vector<uint8_t> wire2 (&buffer[0], &buffer[1024]);
                std::string message = dv.consume(wire2);
                if (checkIfData(message) == 0)
                {
                    dv.recvPacket(message);
                }   
                else 
                {
                    packet_data data = getData(message, port, nodePort);

                    if(data.dest == port)
                    {
                        std::stringstream ss;
                        ss << "Text phrase: " << data.payload;
                        dv.outputToFile(ss.str());
                        std::cout << ss.str() << std::endl;

                    }

                    else 
                    {
                     
                        int path = dv.getShortestPath(data.dest-10000, 0);
                        sockaddr_in packetAddr = dv.getPortFromIndex(path);
                        vector<uint8_t> wire1 = dv.encode(message);
                        sendto(sockfd, (const char*)wire1.data(), wire1.size(), MSG_CONFIRM, 
                        (const struct sockaddr*) &packetAddr, sizeof(packetAddr));
                       
                    }
                    
                }   
            } 
        }  
    }
    std::cout << "thread is ending\n";
    pthread_exit(NULL);
}

std::string makeDataPacket(int source, int dest)
{
    std::string payload = "Hey there this is a message\n";
    std::stringstream ss;
    ss << "data,";
    ss << source << ",";
	ss << dest << ",";
    ss << payload.length() << ",";
	ss << payload << ".";

    std::cout << ss.str() << std::endl;
    
    return ss.str();
}

bool checkIfData(std::string packet)
{
    std::string field;
    std::stringstream linestream(packet);
    getline(linestream, field, ',');
	std::string type = field;
    if (type != "data")
    {
        return 0;
    }
    return 1;
}

packet_data getData(std::string message, int port, int nodePort)
{
    packet_data data;
    std::string field;

    std::stringstream linestream(message);
    getline(linestream, field, ',');
	data.type = field;

    getline(linestream, field, ',');
	data.source = stoi(field);

    getline(linestream, field, ',');
	data.dest = stoi(field);

    getline(linestream, field, ',');
	data.length = stoi(field);

    getline(linestream, field, '.');
	data.payload = field;

    std::stringstream info;

    auto start = std::chrono::system_clock::now();
	auto end = std::chrono::system_clock::now();
	std::time_t end_time = std::chrono::system_clock::to_time_t(end);
	info << "Timestamp: " << std::ctime(&end_time);
    info << "Type: " << data.type << "\n";
    info << "Source: " << data.source << "\n";
    info << "Destination: " << data.dest << "\n";
    info << "Current Port: " << port << "\n";
    info << "Previous Port: " << nodePort << "\n";

    std::cout << info.str() << std::endl;
    dv.outputToFile(info.str());


    return data;

}

void sendDataPacket(std::string packet, int source, int dest)
{

    int sockfd = 3;
    char buffer[MAXLINE]; 
    struct sockaddr_in nodeAddr; 

    int port = source;
    int nodePort = dest;
    
    std::cout << "port " << port << " is connecting to port " << nodePort << std::endl;

    memset(&nodeAddr, 0, sizeof(nodeAddr)); 
    //Information of the node we wish to connect to
    nodeAddr.sin_family = AF_INET;
    nodeAddr.sin_port = htons(nodePort);

    if (inet_aton("127.0.0.1", &nodeAddr.sin_addr) == 0)
    {
        perror("inet_aton failed"); 
        exit(EXIT_FAILURE); 
    }                                                                                                                                                             
    
    vector<uint8_t> wire1 = dv.encode(packet);
    sendto(sockfd, (const char*)wire1.data(), wire1.size(), MSG_CONFIRM, (const struct sockaddr*) &nodeAddr, sizeof(nodeAddr));
        
}


