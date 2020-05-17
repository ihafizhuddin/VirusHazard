#ifndef _SCORETABLE_
#define _SCORETABLE_


#include "go_image.h"


class Game;

class ScoreTable{

public:

    
    ScoreTable(Game * p, int);

    void draw(int x, int y, int z);

private:

    
    GoSDL::Image mRenderedHeader;

    GoSDL::Image mRenderedScore;

    GoSDL::Image mRenderedLastScore;

    int scoreBoardWidth;

    Game * mGame;
};

#endif
