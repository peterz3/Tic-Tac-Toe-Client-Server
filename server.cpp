#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;

#define BACKLOG 5

int main(int argc, char *argv[]){
    //only argument needed to pass in to server side is the port
    //number on which the app will be hosted, make sure its over 2000

    if(argc != 2 ){
        cerr << "please specify a port number greater that 2000" << endl;
        exit(0);
    }
        addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_STREAM; //connection is run until one side disconnects
    hints.ai_flags = AI_PASSIVE;

    int sockfd;
    if((sockfd = getaddrinfo(NULL, argv[1] , &hints, &res)) != 0){
       fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(sockfd));
       //print error if address info returns non-zero
    }
        //open stream oriented socket with internet address
    // keep track of the server socket descriptor
    int serverSide = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(serverSide < 0)
    {
        cerr << "Error establishing the server socket" << endl;
        exit(0);
    }
    //bind the socket to local address
    bind(serverSide, res->ai_addr,res->ai_addrlen);
    cout << "server side binded " << endl;
}