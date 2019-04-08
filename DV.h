#include "DVnode.h"
#include <bits/stdc++.h> 
using namespace std;

#define NUMROUTERS 6
const int INF = INT_MAX;

class DV{
private: // For example:
	char name; // name = 'A'
	int port; // port = 10000
	sockaddr_in addr; // myaddr
	DVnode table[NUMROUTERS]; // table for 'B', 'C', ... , 'E'
	int graph[NUMROUTERS][NUMROUTERS]; // graph for cost of each edge
	bool received[NUMROUTERS];

public:
	char getName();
	void setName(char n);
	int getPort();
	void setPort(int p);
	sockaddr_in DV::getPortFromIndex(int i);
	sockaddr_in getAddr();
	void setAddr(sockaddr_in a);
	bool checkIfNeighbour(int i);
	sockaddr_in getAddrOfNeighbour(int i);

	void initTable(); // initiates Table with values to flag empty entries
	void fillUpTable(string filename); // fills up table with only immediate neighbours
	void printTimestamp();
	void printTable(); // prints all entries in table
	string sendDVupdate();
	void recvDVupdate(string update);
	int charToNum(char c);
	char numToChar(int i);
	void initEdge(char source, char destination, int cost);
	void initGraph();
	void printTopology();
	void bellmanFordAlgorithm();
	void updateTable(int dist[]);

	vector<uint8_t> encode(string message); // encodes message into string of bites
	string consume(vector<uint8_t> wire); // decodes string of bites into message
	
};