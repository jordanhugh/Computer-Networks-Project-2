#include "DV.h"
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

sockaddr_in DV::getAddr(){
	return addr;
}

sockaddr_in DV::getPortFromIndex(int i){
	return table[i].getAddr();
}

void DV::setAddr(sockaddr_in a){
	addr = a;
}

bool DV::checkIfNeighbour(int i){
	if(table[i].getName() != name && table[i].checkIfValid()){
		return true;
	}
	return false;
}

sockaddr_in DV::getAddrOfNeighbour(int i){
	return table[i].getAddr();
}

void DV::initTable(){
	for(int i = 0; i < NUMROUTERS; i++){
		table[i].setValidity(false);
		table[i].setName('0');
		table[i].setPort(-1);
		table[i].setCost(INF);
	}
}

void DV::fillUpTable(string filename){
	fstream topology(filename);


	initTable();
	/*for(int i = 0; i < NUMROUTERS; i++){
		received[i] = false;
	}*/
	initGraph();
	
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

		if (name == node.getName()){
			port = node.getPort();
			addr = node.getAddr();
		}
		else if (name == n){
			int dest = charToNum(node.getName());
			table[dest] = node;
			initEdge(n, node.getName(), node.getCost());
		}
	}
}

void DV::printTimestamp(){
	auto start = std::chrono::system_clock::now();
	auto end = std::chrono::system_clock::now();

	std::time_t end_time = std::chrono::system_clock::to_time_t(end);
	std::cout << "Timestamp: " << std::ctime(&end_time) << std::endl;
}

void DV::printTable(){
	cout << left << setw(15) << "Destination";
	cout << left << setw(15) << "Cost";
	cout << left << setw(25) << "Outgoing UDP Port";
	cout << "Destination UDP Port\n";

	for (int i = 0; i < NUMROUTERS; i++){
		cout << left << setw(15) << table[i].getName();
		cout << left << setw(15) << table[i].getCost();
		cout << port << "(Node " << name << left << setw(13) << ")";
		cout << table[i].getPort() << "(Node " << table[i].getName() << ")\n";
	}
	cout << "\n";
}

string DV::sendDVupdate(){
	std::stringstream ss;

	//ss << "control,";

	for(int j = 0; j < NUMROUTERS; j++){
		for(int i = 0; i < NUMROUTERS; i++){
			if (graph[j][i] != INF){
				ss << numToChar(j) << ",";
				ss << numToChar(i) << ",";
				ss << table[i].getPort() << ",";
				ss << graph[j][i] << ",";
			}
		}
	}
	ss << "XXX\n";

	return ss.str();
}

void DV::recvDVupdate(string update){
	std::stringstream linestream;

	int i = 0;
	while(update[i] != ',' && update[i] != '\n'){ // For Example...
		linestream << update[i++]; // source = 'A'
	};
	i++;

	if (linestream.str() == "WWW"){
		for (int j = 0; j < NUMROUTERS; j++){ 
			received[j] = false;
    	}
		//cout << "Wake Up!";
	}
	else {
		char src;
		while(linestream.str() != "XXX"){
			DVnode dest;

			src = linestream.str()[0];
			/*if(received[charToNum(src)]){
				break;
			}*/
			//cout << "S=" << src << ",";

			linestream.str("");
			while(update[i] != ','){
				linestream << update[i++]; // dest = 'B'
			};
			dest.setName(linestream.str()[0]);
			//cout << "D=" << dest.getName() << ",";
			i++;
			
			linestream.str("");
			while(update[i] != ','){
				linestream << update[i++]; // port = '10001'
			};
			dest.setPort(stoi(linestream.str()));
			//cout << "P=" << dest.getPort() << ",";
			i++;
			
			linestream.str("");
			while(update[i] != ','){
				linestream << update[i++]; // cost = '4'
			};
			dest.setCost(stoi(linestream.str()));
			//cout << "C=" << dest.getCost() << "\n";
			i++;

			int u = charToNum(src);
			int v = charToNum(dest.getName());
			if (graph[v][u] != dest.getCost()){
				if(table[v].getName() != dest.getName() ){
					table[v].setName(dest.getName());
				}
				if(table[v].getPort() == -1){
					table[v].setPort(dest.getPort());
				}

				
				//printTopology();

				//Print out Timestamp
				printTimestamp();

				//Print Table beforehand
				printTable();

				//Add Edge and Calculate Shortest Distances
				initEdge(src, dest.getName(), dest.getCost());
				bellmanFordAlgorithm();

				//Print DV that caused the changed
				cout << "<" << name << ",";
				cout << dest.getName() << ",";
				cout << dest.getPort() << ",";
				cout << dest.getCost() << ">\n\n";

				//Print Table afterwards
				printTable();


			}

			linestream.str("");
			while(update[i] != ',' && update[i] != '\n'){
				linestream << update[i++]; // termination string = 'XXX'
			};
			i++;
		}
		/*if(!received[charToNum(src)]){
			received[charToNum(src)] = true;
			printTopology();
		};*/
	}
}

int DV::charToNum(char c){
	return (c - 'A');
}

char DV::numToChar(int i){
	return (i + 'A');
}

void DV::initEdge(char source, char destination, int cost){
	int i = charToNum(source);
	int j = charToNum(destination);

	graph[j][i] = cost;
}
 
void DV::initGraph() { 
	for(int j = 0; j < NUMROUTERS; j++){
		for (int i = 0; i < NUMROUTERS; i++){
			graph[j][i] = INF;
		}
	}
}
  
void DV::printTopology() { 
    printf("Source, Destination, Cost\n"); 
    for (int j = 0; j < NUMROUTERS; j++) {
		for (int i = 0; i < NUMROUTERS; i++){
			if(graph[j][i] != INF){
				cout << numToChar(j) << ",";
				cout << numToChar(i) << ",";
				cout << table[i].getPort() << ",";
				cout << graph[j][i] << "\n";
			}
		}
	}
}

void DV::bellmanFordAlgorithm() { 

    int dist[NUMROUTERS]; 
    for (int i = 0; i < NUMROUTERS; i++) {
        dist[i] = INF;
    }
	
	int src = charToNum(name);
    dist[src] = 0;
  
    for (int i = 1; i <= NUMROUTERS - 1; i++) { 
        for (int u = 0; u < NUMROUTERS; u++) { 
			for (int v = 0; v < NUMROUTERS; v++){
				if(graph[v][u] == INF){
					continue;
				}

				if (dist[u] != INF && dist[u] + graph[v][u] < dist[v]) {
					dist[v] = dist[u] + graph[v][u]; 
				}
			}
        }
    } 
   
	/*for (int u = 0; u < NUMROUTERS; u++) { 
		for (int v = 0; v < NUMROUTERS; v++){
			if(graph[v][u] == INF){
				continue;
			}

			if (dist[u] != INF && dist[u] + graph[v][u] < dist[v]) {
				cout << "Graph contains negative cost cycles";
			}
		}
	}*/

	updateTable(dist); 
}

void DV::updateTable(int dist[]){
	for (int i = 0; i < NUMROUTERS; i++){
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

