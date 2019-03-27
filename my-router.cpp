#include "DV.h"
using namespace std;

void wakeUpThread(DV dv, int sockfd, sockaddr_in addr);

int main(int argc, char **argv) {
	DV dv;

	if (argc < 2){ 
		perror("Too Few Arugments");
		return 1;
    }
    else if (argc == 2){
        dv.setName(argv[1][0]);
    }
	else {
		perror("Too Many Arguments");
		return 2;
	}

    dv.fillUpTable("topology.txt");
    dv.printTable();

    sockaddr_in myAddr = dv.getAddr();
    sockaddr_in otherAddr;
    if (dv.getName() == 'A'){
        otherAddr = dv.getAddrOfNeighbour(1);
    }
    else{
        otherAddr = dv.getAddrOfNeighbour(0);
    } 

	// Create a socket using UDP IP
    int sockfd;
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
		perror("createsocket");
		return 3;
	}

    // Allow others to reuse the address
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        return 4;
    }

    // Bind address to socket
    if (bind(sockfd, (struct sockaddr*)&myAddr, sizeof(myAddr)) == -1) {
        perror("bind");
        return 5;
    }
    
    // fork() calls a new process, which is called the child process, which runs
    // concurrently with the parent process. For example:
    // 
    // Program:
    // int main(){
    //      fork();
    //      cout << "This is how to use fork()\n";
    //      return 0;
    // }
    //
    // Output:
    // This is how to use fork()
    // This is how to use fork()

    // I used fork to create a second thread that will wake up the other router if the end up in deadlock
    int pid = fork();
    if (pid == -1) {

        perror("Error Creating fork()");
        return 6;

    }
    else if (pid == 0){
        while(1){

            sleep(10);
            wakeUpThread(dv, sockfd, otherAddr);

       }
    }
    else {
        while(1){
            string message = "Hello World";
	        vector<uint8_t> wire = dv.encode(message);

            if (sendto(sockfd, (const char*)wire.data(), wire.size(), 0, (struct sockaddr*)&otherAddr, sizeof(otherAddr)) == -1) {
                perror("send");
                return 7;
            }

            sleep(1);
            
            uint8_t buf[160] = {0};
            memset(buf, '\0', sizeof(buf));

            socklen_t addrlen = sizeof(sockaddr_in);
            if (recvfrom(sockfd, buf, 160, 0, (struct sockaddr*)&otherAddr, &addrlen) == -1) {
                perror("recv");
                return 8;
            }

            cout << buf << endl;
        }
    }

	return 0;
}

void wakeUpThread(DV dv, int sockfd, sockaddr_in addr){
    string message = "Wake Up!";
	vector<uint8_t> wire = dv.encode(message);

    if (sendto(sockfd, (const char*)wire.data(), wire.size(), 0, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("send");
    }
}