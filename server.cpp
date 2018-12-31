#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#define BACKLOG 5

int main(int argc, char *argv[])
{
    //only argument needed to pass in to server side is the port
    //number on which the app will be hosted, make sure its over 2000

    if (argc != 2)
    {
        std::cerr << "please specify a port number greater that 2000" << std::endl;
        exit(0);
    }
    addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;     // use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_STREAM; //connection is run until one side disconnects
    hints.ai_flags = AI_PASSIVE;

    int sockfd;
    if ((sockfd = getaddrinfo(NULL, argv[1], &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(sockfd));
        //print error if address info returns non-zero
    }
    //open stream oriented socket with internet address
    // keep track of the server socket descriptor
    int serverSide = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (serverSide < 0)
    {
        std::cerr << "Error establishing the server socket" << std::endl;
        exit(1);
    }
    //bind the socket to local address
    int bindfd;
    bindfd = bind(serverSide, res->ai_addr, res->ai_addrlen);
    if (bindfd != 0)
    {
        std::cerr << "Failed to bind" << std::endl;
        exit(1);
    }
    std::cout << "server side binded " << std::endl;

    int listenfd;
    listenfd = listen(serverSide, BACKLOG);
    if(listenfd != 0){
        std::cerr << "couldn't listen from socket" << std::endl;
    }

    sockaddr_in6 client;
    socklen_t client_size = sizeof(client);
    int newSockfd = accept(serverSide, (sockaddr *)&client, &client_size);
    if(newSockfd < 0)
    {
        std::cerr << "Error accepting request from client!" << std::endl;
        exit(1);
    }
    char cli_ip[INET6_ADDRSTRLEN];
    std::cout << cli_ip << std::endl;
    const char *ptr_cli_ip = inet_ntop(AF_INET6, &client.sin6_addr, cli_ip, sizeof(cli_ip));
    std::cout << "recieve connection from(ipv6) " << cli_ip << std::endl;
}