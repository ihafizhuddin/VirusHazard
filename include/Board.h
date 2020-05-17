#ifndef _BOARD_H_
#define _BOARD_H_

#include <array>
using namespace std;

#include "log.h"
#include "Square.h"
#include "MultipleMatch.h"



class Board{
public:
    Board();
  
    void swap(int x1, int y1, int x2, int y2);
 
    void del(int x, int y);

    void generate();

    void calcFallMovements();

    void dropAllViruses();

    MultipleMatch check();

    vector<Coord> solutions();

    void endAnimations();

    std::array< std::array<Square, 8>, 8> squares;

    friend ostream& operator <<(ostream& out, Board & B);
};

#endif
