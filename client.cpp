#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{

    // arguments needed to pass in to client side is the port
    //number on which the server is hosted and the ip address

    if (argc != 3)
    {
        std::cerr << "please specify ip address and port number of server(in that order)" << std::endl;
        exit(0);
    }
    std::cout << "what is your name? " << std::endl;
    std::string temp;
    std::cin >> temp;

    int status;

    char buf[1000];

    addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;     // use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_STREAM; //connection is run until one side disconnects
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(argv[1], argv[2], &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        //print error if address info returns non-zero
    }

    //open stream oriented socket with internet address
    // keep track of the server socket descriptor
    int clientSide = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (clientSide < 0)
    {
        std::cerr << "Error establishing the server socket" << std::endl;
        exit(0);
    }

    connect(clientSide, res->ai_addr, res->ai_addrlen);
    if (status < 0)
    {
        std::cout << "Error connecting to socket!" << std::endl;
    }

    std::cout << "established connection with server" << std::endl;
    char cli_name[1024], serv_name[1024];
    int bytes_sent;
    strcpy(cli_name, temp.c_str());
    bytes_sent = send(clientSide, &cli_name, sizeof(cli_name), 0);
    if (bytes_sent == -1)
    {
        std::cout << "PLAYER DATA NOT SENT!" << std::endl
                  << "Trying Again...";
    }
    int bytes_recv = 0;
    while (bytes_recv == 0)
    {
        memset(serv_name, 0, sizeof(serv_name));
        bytes_recv = recv(clientSide, &serv_name, sizeof(serv_name), 0);
        if (bytes_recv == -1)
        {
            std::cout << "COULD NOT ACQUIRE PLAYER INFORMATION!" << std::endl
                      << "Trying Again..." << std::endl;
        }
    }
    std::cout << "YOu have joined a game with  " << serv_name << std::endl;
    return 0;
}