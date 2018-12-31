#include "tictac.h"
#include <iostream>

int main(){
    tictac *b = new tictac();
    b->setVal('o', 0 , 0);
    b->setVal('o', 1 , 0);
    b->setVal('o', 2 , 0);
    b->print();
    std::cout << b->checkWin();
    return 0;
}