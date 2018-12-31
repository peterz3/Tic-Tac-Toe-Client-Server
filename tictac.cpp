#include "tictac.h"
#include <iostream>

char tictac::checkWin(){
            for(int i = 0; i < 3; i++){
                if(board[0][i] == 'x' && board[1][i] == 'x' && board[2][i] == 'x'){
                    return 'x';
                }
                if(board[0][i] == 'o' && board[1][i] == 'o' && board[2][i] == 'o'){
                    return 'o';
                }
            }
            for(int i = 0; i < 3; i++){
                if(board[i][0] == 'x' && board[i][1] == 'x' && board[i][2] == 'x'){
                    return 'x';
                }
                if(board[i][0] == 'o' && board[i][1] == 'o' && board[i][2] == 'o'){
                    return 'o';
                }
            }
            if(board[0][0] == 'x' && board[1][1] == 'x' && board[2][2] == 'x'){
                    return 'x';
                }
                if(board[0][0] == 'o' && board[1][1] == 'o' && board[2][2] == 'o'){
                    return 'o';
                }
                return '-';
        }

void tictac::print(){
    for(int i = 0; i < 3; i ++ ){
        std::cout << std::endl;
        for(int j = 0; j < 3; j++){
            std::cout << board[i][j] << "/t";
        }
    }
}

void tictac::setVal(char val, int i, int j){
    board[i][j] = val;
}