#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <random>
#include <algorithm>
#include "tictac.h"

#define BACKLOG 5

int main(int argc, char *argv[])
{
    //only argument needed to pass in to server side is the port
    //number on which the app will be hosted, make sure its over 2000
    char serv_char, cli_char;
    tictac board;

    if (argc != 2)
    {
        std::cerr << "please specify a port number greater that 2000" << std::endl;
        exit(0);
    }

    std::cout << "Enter you name" << std::endl;
    std::string temp;
    std::cin >> temp;

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
    std::cout << "server side binded, waiting for client " << std::endl;

    int listenfd;
    listenfd = listen(serverSide, BACKLOG);
    if (listenfd == -1)
    {
        std::cerr << "couldn't listen from socket" << std::endl;
    }

    sockaddr_in6 client;
    socklen_t client_size = sizeof(client);
    int newSockfd = accept(serverSide, (sockaddr *)&client, &client_size);
    if (newSockfd < 0)
    {
        std::cerr << "Error accepting request from client!" << std::endl;
        exit(1);
    }
    char cli_ip[INET6_ADDRSTRLEN];
    const char *ptr_cli_ip = inet_ntop(AF_INET6, &client.sin6_addr, cli_ip, sizeof(cli_ip));
    std::cout << "recieve connection from(ipv6) " << cli_ip << std::endl;
    char cli_name[1024], serv_name[1024];
    strcpy(serv_name, temp.c_str());
    int bytes_recv = 0;
    while (bytes_recv == 0)
    {
        memset(&cli_name, 0, sizeof(cli_name));
        bytes_recv = recv(newSockfd, &cli_name, sizeof(cli_name), 0);
        if (bytes_recv == -1)
        {
            memset(&cli_name, 0, sizeof(cli_name));
            std::cout << "Could not ACQUIRE Player Information!" << std::endl
                      << "Trying again..." << std::endl;
            bytes_recv = 0;
        }
        else
        {
            std::cout << cli_name << " joined the game" << std::endl;
        }
    }

    int bytes_sent = 0;
    while (bytes_sent == 0)
    {
        bytes_sent = send(newSockfd, &serv_name, sizeof(serv_name), 0);
        if (bytes_sent == -1)
        {
            std::cout << "Could not SEND Player Data!"
                      << "Trying Again..." << std::endl;
            bytes_sent = 0;
        }
    }
    const int head_val = 0;
    const int tail_val = 1;
    int serv_coin_val;
    while (1)
    {
        std::cout << "would you like heads or tails for the coin toss?" << std::endl;
        std::string coin_val;
        std::cin >> coin_val;
        std::transform(coin_val.begin(), coin_val.end(), coin_val.begin(), ::tolower);
        if (coin_val == "heads")
        {
            serv_coin_val = 0;
            break;
        }
        else if (coin_val == "tails")
        {
            serv_coin_val = 1;
            break;
        }
    }
    bool serv_turn;
    if (rand() % 2 == serv_coin_val)
    {
        std::cout << "you won the coin flip, so you will go first as X's" << std::endl;
        serv_char = 'x';
        cli_char = 'o';
        serv_turn = true;
    }
    else
    {
        std::cout << "you lost the coin flip, so you will go second as O's" << std::endl;
        serv_char = 'o';
        cli_char = 'x';
        serv_turn = false;
    }
    bytes_sent = 0;
    while (bytes_sent == 0)
    {
        bytes_sent = send(newSockfd, &serv_turn, sizeof(serv_turn), 0);
        if (bytes_sent == -1)
        {
            std::cout << "Could not SEND Player Data!"
                      << "Trying Again..." << std::endl;
            bytes_sent = 0;
        }
    }
    int turn_count = 0;
    int move_buffer[2];
    memset(&move_buffer, 5, sizeof(move_buffer));
    while (board.checkWin() == '-' || turn_count < 9)
    {
        if (serv_turn)
        {
            board.print();
            int col, row;
            while (1)
            {
                std::cout << "please input col coordinate from 0 to 2" << std::endl;
                std::cin >> col;
                if (col == 1 || col == 2 || col == 0)
                {
                    break;
                }
            }
            while (1)
            {
                std::cout << "please input row coordinate from 0 to 2" << std::endl;
                std::cin >> row;
                if (row == 1 || row == 2 || row == 0)
                {
                    break;
                }
            }
            if (board.checkPos(row, col))
            {
                board.setVal(cli_char, row, col);
                move_buffer[0] = row;
                move_buffer[1] = col;
                bytes_sent = send(newSockfd, &move_buffer, sizeof(move_buffer), 0);
                while (bytes_sent == 0)
                {
                    if (bytes_sent == -1)
                    {
                        std::cout << "PLAYER DATA NOT SENT!" << std::endl
                                  << "Trying Again...";
                        bytes_sent = 0;
                    }
                }
                serv_turn = !serv_turn;
                turn_count++;
            }
        }
        else
        {
            bytes_recv = 0;
            while (bytes_recv == 0)
            {
                memset(&move_buffer, 5, sizeof(move_buffer));
                bytes_recv = recv(newSockfd, &move_buffer, sizeof(move_buffer), 0);
                if (bytes_recv == -1 || move_buffer[0] == 5 || move_buffer[1] == 5)
                {
                    memset(&move_buffer, 5, sizeof(move_buffer));
                    std::cout << "COULD NOT ACQUIRE PLAYER INFORMATION!" << std::endl
                              << "Trying Again..." << std::endl;
                    bytes_recv = 0;
                }
            }
            std::cout << "your opponent played with : " << std::endl;
            board.setVal(cli_char, move_buffer[0], move_buffer[1]);
            board.print();
            serv_turn = !serv_turn;
            turn_count++;
        }
    }
    switch (board.checkWin())
    {
    case '-':
        std::cout << "no one won" << std::endl;

    case 'x':
        if (serv_char == 'x')
        {
            std::cout << serv_name << " has won the game" << std::endl;
        }
        else
        {
            std::cout << cli_name << " has won the game" << std::endl;
        }
    case 'o':
        if (serv_char == 'o')
        {
            std::cout << serv_name << " has won the game" << std::endl;
        }
        else
        {
            std::cout << cli_name << " has won the game" << std::endl;
        }
    }

    return 0;
}
