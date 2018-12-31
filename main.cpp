#include "tictac.h"

int main(){
    tictac *b = new tictac();
   b->setVal('x', 0 , 0);
    b->print();

    return 0;
}