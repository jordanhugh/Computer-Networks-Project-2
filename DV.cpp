#include "DV.h"
using namespace std;

DV::DV(){}

DV::~DV(){}

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

void DV::setAddr(sockaddr_in a){
	addr = a;
}

sockaddr_in DV::getAddrOfNeighbour(int i){
	return table[i].getAddr();
}

bool DV::checkIfAlreadyReceived(int i){
	return received[i];
}

void DV::setAsAlreadyReceived(int i){
	received[i] = true;
}

void DV::setAsNotReceived(int i){
	received[i] = false;
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
	for(int i = 0; i < NUMROUTERS; i++){
		received[i] = false;
	}
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
			setAddr(node.getAddr());
		}
		else if (name == n){
			int i = charToNum(node.getName());
			table[i] = node;
			initEdge(n, node.getName(), node.getCost());
		}
	}
}

void DV::printTable(){
	for (int i = 0; i < NUMROUTERS; i++){
		cout << "Name: " << table[i].getName() << endl;
		cout << "Port: " << table[i].getPort() << endl;
		cout << "Cost: " << table[i].getCost() << endl;
		cout << endl;
	}
}

string DV::sendDVupdate(){
	std::stringstream ss;

	for(int i = 0; i < NUMROUTERS; i ++){
		if(table[i].checkIfValid()){
			ss << name << ",";
			ss << table[i].getName() << ",";
			ss << table[i].getPort() << ","; 
			ss << table[i].getCost() << ",";
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
			if(received[charToNum(src)]){
				break;
			}
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
				initEdge(src, dest.getName(), dest.getCost());
				printArr(); 
			}

			linestream.str("");
			while(update[i] != ',' && update[i] != '\n'){
				linestream << update[i++]; // termination string = 'XXX'
			};
			i++;
		}
		if(!received[charToNum(src)]){
			received[charToNum(src)] = true;
			printArr();
		};
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
  
void DV::printArr() { 
    printf("Source, Destination, Cost\n"); 
    for (int j = 0; j < NUMROUTERS; j++) {
		for (int i = 0; i < NUMROUTERS; i++){
			if(graph[j][i] != INF){
				cout << numToChar(j) << ",";
				cout << numToChar(i) << ",";
				cout << graph[j][i] << "\n";
			}
		}
	}
} 

int DV::minDistance(int dist[], bool sptSet[]) { 
	int min = INF;
    int min_i;
   
    for (int i = 0; i < NUMROUTERS; i++) {
    	if (sptSet[i] == false && dist[i] <= min) {
        	min = dist[i];
			min_i = i; 
	 	}
	}
    return min_i; 
} 
   
int DV::printSolution(int dist[]) { 
	printf("Vertex   Distance from Source\n"); 
	for (int i = 0; i < NUMROUTERS; i++) {
		cout << i << "to" << dist[i];
	}
}

void DV::dijkstraAlgorithm() { 
    int dist[NUMROUTERS];
	bool sptSet[NUMROUTERS]; 
   
    for (int i = 0; i < NUMROUTERS; i++) {
        dist[i] = INF;
		sptSet[i] = false; 
	}
   
	int src = charToNum(name);
    dist[src] = 0; 
   
    for (int i = 0; i < NUMROUTERS - 1; i++) { 
       	int u = minDistance(dist, sptSet); 
   
        sptSet[u] = true; 
   
        for (int v = 0; v < NUMROUTERS; v++) {
        	if (!sptSet[v] && graph[u][v] && dist[u] != INF && dist[u] + graph[u][v] < dist[v]) 
            dist[v] = dist[u] + graph[u][v]; 
     	} 
	}
    printSolution(dist); 
} 

/* //Work in Progress...
void DV::BellmanFordAlgorithm() { 
    //int vertices = graph->vertices; 
    //int edges = graph->edges; 
    int dist[NUMROUTERS]; 
  
    // Initiate the distances from the starting vertice to all the other vertices as INFINITE
    for (int i = 0; i < NUMROUTERS; i++) {
        dist[i] = INF;
    }
    dist[charToNum(name)] = 0;
  
    // Relaxes all the edges 'V - 1' times to find the shortest path to all other vertices
    int u, v;
	int cost;
    for (int i = 1; i <= NUMROUTERS - 1; i++) { 

        for (int j = 0; j < edges; j++) { 

            u = charToNum(graph->edge[j].source);
            v = charToNum(graph->edge[j].destination);
            cost = graph->edge[j].cost; 
            if (dist[u] != INF && dist[u] + cost < dist[v]) {
                dist[v] = dist[u] + cost; 
            }
        } 
    } 
  
    // Repeat to check for negative paths 
    for (int i = 0; i < edges; i++) { 
        u = charToNum(graph->edge[i].source);
        v = charToNum(graph->edge[i].destination);
        cost = graph->edge[i].cost; 
        if (dist[u] != INF && dist[u] + cost < dist[v]) 
            printf("Graph contains negative cost cycle"); 
    } 
  
    return; 
}*/

vector<uint8_t> DV::encode(string message) {
	vector<uint8_t> wire(message.begin(), message.end());

	return wire;
}

string DV::consume(vector<uint8_t> wire) {
	string message(wire.begin(), wire.end());

	return message;
}
