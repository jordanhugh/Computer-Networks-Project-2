#include "DVnode.h"
#include <bits/stdc++.h> 
using namespace std;

#define NUMROUTERS 6
const int INF = INT_MAX;
  
/*
struct Edge { 
    char source;
    char destination;
    int cost; 
}; 

struct Graph { 

    int vertices;
    int edges;
    struct Edge* edge; // represent graph as an array of edges
}; 
*/

class DV{
private: // For example:
	char name; // name = 'A'
	int port; // port = 10000
	sockaddr_in addr; // myaddr
	bool received[NUMROUTERS];
	DVnode table[NUMROUTERS]; // table for 'B', 'C', ... , 'E'
	int graph[NUMROUTERS][NUMROUTERS];

public:
	DV();
	~DV();
	char getName();
	void setName(char n);
	int getPort();
	void setPort(int p);
	sockaddr_in getAddr();
	void setAddr(sockaddr_in a);
	sockaddr_in getAddrOfNeighbour(int i);
	bool checkIfAlreadyReceived(int i);
	void setAsAlreadyReceived(int i);
	void setAsNotReceived(int i);
	void initTable(); // initiates Table with values to flag empty entries
	void fillUpTable(string filename); // fills up table with only immediate neighbours

	string sendDVupdate();
	void recvDVupdate(string update);
	int charToNum(char c);
	char numToChar(int i);
	void initEdge(char source, char destination, int cost);
	void initGraph(/*int vertices, int edges*/);
	void printArr();
	int minDistance(int dist[], bool sptSet[]);
	int printSolution(int dist[]);
	void dijkstraAlgorithm();
	//void BellmanFordAlgorithm(struct Graph* graph, int source);

	void printTable(); // prints all entries in table
	vector<uint8_t> encode(string message); // encodes message into string of bites
	string consume(vector<uint8_t> wire); // decodes string of bites into message
};