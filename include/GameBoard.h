#ifndef GAME_BOARD_H
#define GAME_BOARD_H

#include "GameBoardSounds.h"

#include "Board.h"
#include "FloatingScore.h"
#include "ScoreTable.h"

#include "go_image.h"

class Game;
class StateGame;

class GameBoard
{
public:
    void setGame(Game *, StateGame *);
    void endGame(int score);

    void loadResources();

    void update();
    void draw();
    void mouseButtonDown(int, int);
    void mouseButtonUp(int, int);

private:

    bool overVirus (int mx, int my);

    Coord getCoord (int mx, int my);

    bool checkClickedSquare(int mx, int my);

    void createFloatingScores();

    void playMatchSound();

    Game * mGame;

    StateGame * mStateGame;

    enum tState
    {
        eNoBoard,

        eBoardAppearing,
        eBoardFilling,
        eBoardDisappearing,

        eSteady,

        eVirusSelected,
        eVirusSwitching,
        eVirusDisappearing,

        eTimeFinished,
        eShowingScoreTable
    };

    tState mState = eNoBoard;


    Coord mSelectedSquareFirst;

    Coord mSelectedSquareSecond;

    MultipleMatch mGroupedSquares;

    Board mBoard;

    GameBoardSounds mGameBoardSounds;

    GoSDL::Image mImgSelector;

    GoSDL::Image mImgBoard;
    GoSDL::Image mImgOne;
    GoSDL::Image mImgTwo;
    GoSDL::Image mImgThree;
    GoSDL::Image mImgFour;
    GoSDL::Image mImgFive;
    GoSDL::Image mImgSix;
    GoSDL::Image mImgSeven;


    int mAnimationCurrentStep = 0;

    int mAnimationLongTotalSteps = 50;

    int mAnimationShortTotalSteps = 17;

    int mMultiplier = 1;

    vector<FloatingScore> mFloatingScores;

    std::shared_ptr<ScoreTable> scoreTable;
};

#endif