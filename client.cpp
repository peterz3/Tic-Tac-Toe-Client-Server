#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include "tictac.h"

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
    tictac board;

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
    int bytes_sent = 0;
    strcpy(cli_name, temp.c_str());
    bytes_sent = send(clientSide, &cli_name, sizeof(cli_name), 0);
    while (bytes_sent == 0)
    {
        if (bytes_sent == -1)
        {
            std::cout << "PLAYER DATA NOT SENT!" << std::endl
                      << "Trying Again...";
            bytes_sent = 0;
        }
    }
    int bytes_recv = 0;
    while (bytes_recv == 0)
    {
        memset(serv_name, 0, sizeof(serv_name));
        bytes_recv = recv(clientSide, &serv_name, sizeof(serv_name), 0);
        if (bytes_recv == -1)
        {
            memset(serv_name, 0, sizeof(serv_name));
            std::cout << "COULD NOT ACQUIRE PLAYER INFORMATION!" << std::endl
                      << "Trying Again..." << std::endl;
            bytes_recv = 0;
        }
    }
    std::cout << "You have joined a game with  " << serv_name << std::endl;
    std::cout << "Please wait while " << serv_name << " does a coin flip" << std::endl;
    bool serv_turn;
    bytes_recv = 0;
    while (bytes_recv == 0)
    {
        memset(&serv_turn, 0, sizeof(serv_turn));
        bytes_recv = recv(clientSide, &serv_turn, sizeof(serv_turn), 0);
        if (bytes_recv == -1)
        {
            memset(&serv_turn, 0, sizeof(serv_turn));
            std::cout << "COULD NOT ACQUIRE PLAYER INFORMATION!" << std::endl
                      << "Trying Again..." << std::endl;
            bytes_recv = 0;
        }
    }
    bool client_turn = !serv_turn;
    char serv_char, cli_char;
    if (client_turn)
    {
        std::cout << "You won the coin flip you will go first as X's" << std::endl;
        cli_char = 'x';
        serv_char = 'o';
    }
    else
    {
        std::cout << "You lost the coin flip you will go second as O's" << std::endl;
        cli_char = 'o';
        serv_char = 'x';
    }
    int turn_count = 0;
    int move_buffer[2];
    memset(&move_buffer, 5, sizeof(move_buffer));
    while (board.checkWin() == '-' || turn_count < 9)
    {
        if (client_turn)
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
                bytes_sent = send(clientSide, &move_buffer, sizeof(move_buffer), 0);
                while (bytes_sent == 0)
                {
                    if (bytes_sent == -1)
                    {
                        std::cout << "PLAYER DATA NOT SENT!" << std::endl
                                  << "Trying Again...";
                        bytes_sent = 0;
                    }
                }
                client_turn = !client_turn;
                turn_count++;
            }
        }
        else
        {
            bytes_recv = 0;
            while (bytes_recv == 0)
            {
                memset(&move_buffer, 5, sizeof(move_buffer));
                bytes_recv = recv(clientSide, &move_buffer, sizeof(move_buffer), 0);
                if (bytes_recv == -1 || move_buffer[0] == 5 || move_buffer[1] == 5)
                {
                    memset(&move_buffer, 5, sizeof(move_buffer));
                    std::cout << "COULD NOT ACQUIRE PLAYER INFORMATION!" << std::endl
                              << "Trying Again..." << std::endl;
                    bytes_recv = 0;
                }
            }
            std::cout << "your opponent played with : " << std::endl;
            board.setVal(serv_char, move_buffer[0], move_buffer[1]);
            board.print();
            client_turn = !client_turn;
            turn_count++;
        }
    }
    switch(board.checkWin()){
        case '-':
        std::cout << "no one won" << std::endl;

        case 'x': 
        if(serv_char == 'x'){ std::cout << serv_name <<" has won the game" << std::endl;
        } 
        else{
            std::cout << cli_name <<" has won the game" << std::endl;
        } 
        case 'o':
        if(serv_char == 'o'){ std::cout << serv_name <<" has won the game" << std::endl;
        } 
        else{
            std::cout << cli_name <<" has won the game" << std::endl;
        }
    }

    return 0;
}