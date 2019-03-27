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

void DV::initTable(){
	for(int i = 0; i < NUMROUTERS; i++){
		table[i].setValidity(false);
		table[i].setName('0');
		table[i].setPort(-1);
		table[i].setCost(-1);
	}
}

void DV::fillUpTable(string filename){
	fstream topology(filename);

	initTable();
	
	string line;
	string field;
	while (topology.peek() != EOF){
		getline(topology, line);
		stringstream linestream(line);

		DVentry entry;
		entry.setValidity(true);

		//Source Router
		getline(linestream, field, ',');
		char n = field[0];

		//Neighbour Router
		getline(linestream, field, ',');
		entry.setName(field[0]);

		//Neighbour Port Number
		getline(linestream, field, ',');
		entry.setPort(stoi(field));
		entry.setAddr();

		//Cost
		getline(linestream, field, ',');
		entry.setCost(stoi(field));

		if (name == entry.getName()){
			setAddr(entry.getAddr());
		}
		else if (name == n){
			int index = entry.getName() - 'A';
			table[index] = entry;
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

vector<uint8_t> DV::encode(string message) {
	vector<uint8_t> wire(message.begin(), message.end());

	return wire;
}

string DV::consume(vector<uint8_t> wire) {
	string message(wire.begin(), wire.end());

	return message;
}