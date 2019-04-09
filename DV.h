#include "DVnode.h"
#include <bits/stdc++.h> 
#include <stdio.h>
#include <fstream>
#include <cstdio>
#include <iostream>
using namespace std;

const int INF = INT_MAX;

class DV{
private: // For example:
	char name; // name = 'A'
	int port; // port = 10000
	sockaddr_in addr; // myaddr
	int numrouters; 
	DVnode* table; // table for 'B', 'C', ... , 'E'
	int** graph; // graph for cost of each edge
	int** shortestPath; 

public:
	char getName();
	void setName(char n);
	int getPort();
	void setPort(int p);
	sockaddr_in getAddr();
	sockaddr_in getPortFromIndex(int i);
	void setAddr();
	int getNumRouters();
	void setNumRouters(int num);
	bool checkIfNeighbour(int i);
	sockaddr_in getAddrOfNeighbour(int i);
	int getShortestPath(int i, int j);


	void initTable(); // initiates Table with values to flag empty entries
	void resetTable();
	void fillUpTable(string filename); // fills up table with only immediate neighbours
	void initExtraNode();
	string printTimestamp();
	string printTable(); // prints all entries in table
	void reset(string filename);

	string sendPacket();
	int recvPacket(string packet);
	int charToNum(char c);
	char numToChar(int i);
	void initEdge(char source, char destination, int cost);
	void initGraph();
	void resetGraph();
	void printTopology();
	void findShortestPath(int parent[], int i, int u, int &v);
	void bellmanFordAlgorithm();
	void updateTable(int dist[]);

	vector<uint8_t> encode(string message); // encodes message into string of bites
	string consume(vector<uint8_t> wire); // decodes string of bites into message
	void outputToFile(std::string input);
	void clearFile();
	
};