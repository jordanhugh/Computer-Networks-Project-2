#include "DV.h"
#include <cstdlib>
#include <chrono>
#include <ctime>
#include <iomanip>
using namespace std;

char DV::getName(){
	return name;
}

void DV::setName(char n){
	name = n;
}

int DV::getPort(){
	return port;
}

void DV::setPort(int p){
	port = p;
}

sockaddr_in DV::getPortFromIndex(int i){
	return table[i].getAddr();
}


sockaddr_in DV::getAddr(){
	return addr;
}

void DV::setAddr(){
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port); // Short, network byte order
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));
}

int DV::getNumRouters(){
	return numrouters;
}

void DV::setNumRouters(int num){
	numrouters = num;
}

bool DV::checkIfNeighbour(int i){
	if(name != table[i].getName() && table[i].checkIfValid()){
		return true;
	}
	return false;
}

sockaddr_in DV::getAddrOfNeighbour(int i){
	return table[i].getAddr();
}

int DV::getShortestPath(int dest, int i){
	return shortestPath[dest][0];
}

void DV::initTable(){
	table = new DVnode[numrouters];

	for(int i = 0; i < numrouters; i++){
		table[i].setValidity(false);
		table[i].setName('X');
		table[i].setPort(-1);
		table[i].setCost(INF);
	}
}

void DV::resetTable(){

	//Reallocate Memory for new Table
	DVnode* temp = table;
	table = new DVnode[numrouters];
	delete[] temp;
	
	//Deallocate Memory for old table
	for(int i = 0; i < numrouters; i++){
		table[i].setValidity(false);
		table[i].setName('X');
		table[i].setPort(-1);
		table[i].setCost(INF);
	}
}

void DV::fillUpTable(string filename){
	fstream topology(filename);
	
	if(name >= 'A' && name <= 'F'){
		numrouters = 6;
		initTable();
		initGraph();
		shortestPath = new int*[numrouters];
		for (int i = 0; i < numrouters; i++) {
  			shortestPath[i] = new int[numrouters];
		}

		string line;
		string field;
		while (topology.peek() != EOF){
			getline(topology, line);
			stringstream linestream(line);

			DVnode node;
			node.setValidity(true);

			//Source Router
			getline(linestream, field, ',');
			char n = field[0];

			//Neighbour Router
			getline(linestream, field, ',');
			node.setName(field[0]);

			//Neighbour Port Number
			getline(linestream, field, ',');
			node.setPort(stoi(field));
			node.setAddr();

			//Cost
			getline(linestream, field, ',');
			node.setCost(stoi(field));

			if (name == node.getName()){ // if source router...
				int src = charToNum(name);
				table[src].setName(name);

				port = node.getPort();
				table[src].setPort(port);

				addr = node.getAddr();
				table[src].setAddr();

				table[src].setCost(node.getCost());
			}
			else if (name == n){ // if neighbouring router
				int dest = charToNum(node.getName());
				table[dest] = node;
				initEdge(n, node.getName(), node.getCost());
			}
		}
	}
	else if (name == 'G'){
		numrouters = 7;
		initTable();
		initGraph();
		initExtraNode();
	}
}

void DV::initExtraNode(){
	DVnode node;
	name = 'G';
	port = 10006;
	setAddr();

	int i;
	for(i = 0; i < numrouters - 1; i++){
		node.setValidity(true);
		node.setName(numToChar(i));
		node.setPort(10000 + i);
		node.setAddr();
		node.setCost(rand() % 4);

		table[i] = node;
		initEdge(name, node.getName(), node.getCost());
	}

	node.setValidity(false);
	node.setName(name);
	node.setPort(port);
	node.setAddr();
	node.setCost(0);

	table[i] = node;
}

string DV::printTimestamp(){
	std::stringstream ss;

	auto start = std::chrono::system_clock::now();
	auto end = std::chrono::system_clock::now();

	std::time_t end_time = std::chrono::system_clock::to_time_t(end);
	ss << "Timestamp: " << std::ctime(&end_time);

	return ss.str();
}

std::string DV::printTable(){
	std::stringstream ss;

	ss << left << setw(15) << "Destination";
	ss << left << setw(15) << "Cost";
	ss << left << setw(25) << "Outgoing UDP Port";
	ss << "Destination UDP Port\n";

	for (int i = 0; i < numrouters; i++){
		ss << left << setw(15) << table[i].getName();
		ss << left << setw(15) << table[i].getCost();
		ss << port << "(Node " << name << left << setw(13) << ")";
		ss << table[i].getPort() << "(Node " << table[i].getName() << ")\n";
	}
	ss << "\n";

	return ss.str();
}

void DV::reset(string filename){
	fstream topology(filename);
	cout << "Part 1";
	char temp = name;
	resetTable();
	resetGraph();
	name = temp;
	cout << "Part 2";
	if(name >= 'A' && name <= 'F'){
		cout << "Part 3";
		string line;
		string field;
		while (topology.peek() != EOF){
			getline(topology, line);
			stringstream linestream(line);
			cout << "Part 4";
			DVnode node;
			node.setValidity(true);

			//Source Router
			getline(linestream, field, ',');
			char n = field[0];

			//Neighbour Router
			getline(linestream, field, ',');
			node.setName(field[0]);

			//Neighbour Port Number
			getline(linestream, field, ',');
			node.setPort(stoi(field));
			node.setAddr();

			//Cost
			getline(linestream, field, ',');
			node.setCost(stoi(field));

			if (name == node.getName()){ // if source router...
				int src = charToNum(name);
				table[src].setName(name);

				port = node.getPort();
				table[src].setPort(port);

				addr = node.getAddr();
				table[src].setAddr();

				table[src].setCost(node.getCost());
			}
			else if (name == n){ //if neighbouring router
				int dest = charToNum(node.getName());
				table[dest] = node;
				initEdge(n, node.getName(), node.getCost());
			}
		}
	}
	else if (name == 'G'){
		initExtraNode();
	}
}

string DV::sendPacket(){
	std::stringstream packet;

	//if(timeout){
	//	packet << "TIMEOUT\n";
	//}
	
		for(int j = 0; j < numrouters; j++){
			for(int i = 0; i < numrouters; i++){
				if (graph[j][i] != INF){
					packet << numToChar(j) << ",";
					packet << numToChar(i) << ",";
					packet << table[i].getPort() << ",";
					packet << graph[j][i] << ",";
				}
			}
		}
		packet << "END\n";
	

	return packet.str();
}

int DV::recvPacket(string packet){
	std::stringstream linestream;

	int i = 0;
	while(packet[i] != ',' && packet[i] != '\n'){ // For Example...
		linestream << packet[i++]; // source = 'A'
	};
	i++;
	if (linestream.str() == "TIMEOUT"){
		return -1;
	}
	else if (linestream.str() == "EXTRANODE"){
		return -2;
	}
	else {
		while(linestream.str() != "END"){
			DVnode dest;

			char src = linestream.str()[0];
			//cout << "S=" << src << ",";

			linestream.str("");
			while(packet[i] != ','){
				linestream << packet[i++]; // dest = 'B'
			};
			dest.setName(linestream.str()[0]);
			//cout << "D=" << dest.getName() << ",";
			i++;
			
			linestream.str("");
			while(packet[i] != ','){
				linestream << packet[i++]; // port = '10001'
			};
			dest.setPort(stoi(linestream.str()));
			//cout << "P=" << dest.getPort() << ",";
			i++;
			
			linestream.str("");
			while(packet[i] != ','){
				linestream << packet[i++]; // cost = '4'
			};
			dest.setCost(stoi(linestream.str()));
			//cout << "C=" << dest.getCost() << "\n";
			i++;

			int u = charToNum(src);
			int v = charToNum(dest.getName());
			if (graph[u][v] != dest.getCost()){
				if(table[u].getName() != src){
					table[u].setName(src);
				}
				if(table[v].getName() != dest.getName() ){
					table[v].setName(dest.getName());
				}
				cout << dest.getPort();
				if(table[v].getPort() != dest.getPort()){
					table[v].setPort(dest.getPort());
				}

				//Print out Timestamp
				std::string output = printTimestamp();
				outputToFile(output);

				//Print Table beforehand
				output = printTable();
				outputToFile(output);

				//Add Edge and Calculate Shortest Distances
				initEdge(src, dest.getName(), dest.getCost());
				bellmanFordAlgorithm();

				//Print DV that caused the changed
				std::stringstream ss;
				ss << "<" << src << ",";
				ss << dest.getName() << ",";
				ss << dest.getPort() << ",";
				ss << dest.getCost() << ">\n\n";
				output = ss.str();
				outputToFile(output);

				//Print Table afterwards
				output = printTable();
				outputToFile(output);
			}

			linestream.str("");
			while(packet[i] != ',' && packet[i] != '\n'){
				linestream << packet[i++]; // termination string = 'XXX'
			};
			i++;
		}

		return 0;
	}
}

int DV::charToNum(char c){
	return (c - 'A');
}

char DV::numToChar(int i){
	return (i + 'A');
}

void DV::initEdge(char source, char destination, int cost){
	int j = charToNum(source);
	int i = charToNum(destination);

	graph[j][i] = cost;
}
 
void DV::initGraph() { 
	graph = new int*[numrouters];
	for (int i = 0; i < numrouters; i++) {
  		graph[i] = new int[numrouters];
	}

	for(int j = 0; j < numrouters; j++){
		for (int i = 0; i < numrouters; i++){
			graph[j][i] = INF;
		}
	}
}

void DV::resetGraph() { 

	//Allocate Memory for new table
	int** temp = graph;
	graph = new int*[numrouters];
	for (int i = 0; i < numrouters; i++) {
  		graph[i] = new int[numrouters];
	}
	for(int j = 0; j < numrouters; j++){
		for (int i = 0; i < numrouters; i++){
			graph[j][i] = INF;
		}
	}

	//Deallocate Memory for old table
	for(int i = 0; i < numrouters - 1; i++){
		delete[] temp[i];
	}
	delete[] temp;
}
  
void DV::printTopology() { 
    cout << "Source, Destination, Cost\n";
    for (int j = 0; j < numrouters; j++) {
		for (int i = 0; i < numrouters; i++){
			if(graph[j][i] != INF){
				cout << numToChar(j) << ",";
				cout << numToChar(i) << ",";
				cout << table[i].getPort() << ",";
				cout << graph[j][i] << "\n";
			}
		}
	}
}

void DV::findShortestPath(int parent[], int j, int u, int &v){
	if (parent[j] == -1){
		return;
	}

	findShortestPath(parent, parent[j], u, v);
	shortestPath[u][v++] = j;
}

void DV::bellmanFordAlgorithm() {
    int dist[numrouters];
	int parent[numrouters];

    for (int i = 0; i < numrouters; i++) {
        dist[i] = INF;
		parent[i] = -1;
    }

	for (int j = 0; j < numrouters; j++){
		for(int i = 0; i < numrouters; i++){
			shortestPath[j][i] = -1;
		}
	}
	
	int src = charToNum(name);
    dist[src] = 0;
  
    for (int i = 1; i <= numrouters - 1; i++) { 
        for (int u = 0; u < numrouters; u++) { 
			for (int v = 0; v < numrouters; v++){
				if(graph[u][v] == INF){
					continue;
				}

				if (dist[u] != INF && dist[u] + graph[u][v] < dist[v]) {
					dist[v] = dist[u] + graph[u][v];
					parent[v] = u;
				}
			}
        }
    } 

	updateTable(dist);

	int v;
	for (int u = 0; u < numrouters; u++)
	{
		v = 0;
		findShortestPath(parent, u, u, v);
	}

	// Print Shortest Paths for 
	/*
	for(int j = 0; j < numrouters; j++){
		cout << numToChar(j) << " ";
	}
	cout << endl;

	for(int j = 0; j < numrouters; j++){
		for (int i = 0; i < numrouters; i++){
			if (shortestPath[j][i] == -1){
				cout << " ";
			}
			cout << shortestPath[j][i] << " ";
		}
		cout << endl;
	}*/
}

void DV::updateTable(int dist[]){
	for (int i = 0; i < numrouters; i++){
		table[i].setCost(dist[i]);
	}
}

vector<uint8_t> DV::encode(string message) {
	vector<uint8_t> wire(message.begin(), message.end());

	return wire;
}

string DV::consume(vector<uint8_t> wire) {
	string message(wire.begin(), wire.end());

	return message;
}


void DV::outputToFile(std::string input){
	std::stringstream ss;
	ss << "routing-output" << name << ".txt";
	string filename = ss.str();

	std::ofstream file(filename, ios::app);

    file << input << endl;

    file.close(); 
}

void DV::clearFile(){
	//std::stringstream ss;
	//ss << "routing-output" << name << ".txt";
	//std::string filename = ss.str();

	//std::ifstream File;
	//File.open(filename.c_str(), std::ifstream::out | std::ifstream::trunc);
	//if(!File.is_open() || File.fail()){
	//	File.open();
	//	cout << "Error erasing content!";
	//}

	//File.close();
}