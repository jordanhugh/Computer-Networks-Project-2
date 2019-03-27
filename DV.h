#include "DVentry.h"
using namespace std;

#define NUMROUTERS 6

class DV{
private: // For example:
	char name; // name = 'A'
	int port; // port = 10000
	sockaddr_in addr; // myaddr
	DVentry table[NUMROUTERS]; // table for 'B', 'C', ... , 'E'

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
	void initTable(); // initiates Table with values to flag empty entries
	void fillUpTable(string filename); // fills up table with only immediate neighbours
	void printTable(); // prints all entries in table
	vector<uint8_t> encode(string message); // encodes message into string of bites
	string consume(vector<uint8_t> wire); // decodes string of bites into message
};