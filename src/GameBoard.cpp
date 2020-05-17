#include "GameBoard.h"

#include "Game.h"
#include "StateGame.h"
#include <functional>

using namespace std::placeholders;


void GameBoard::setGame(Game * game, StateGame * stateGame)
{
    mGame = game;
    mStateGame = stateGame;

    mBoard.generate();

    mState = eBoardAppearing;
    mAnimationCurrentStep = 0;
}

void GameBoard::endGame(int score)
{
    if (mState == eTimeFinished || mState == eShowingScoreTable)
        return;

    mBoard.dropAllViruses();
    mState = eTimeFinished;

    // Generate the score table
    scoreTable = std::make_shared<ScoreTable>(mGame, score);
}

void GameBoard::loadResources()
{
    mImgBoard.setWindowAndPath(mGame, "image/background.png");
    mImgOne.setWindowAndPath(mGame, "image/virusOne.png");
    mImgTwo.setWindowAndPath(mGame, "image/virusTwo.png");
    mImgThree.setWindowAndPath(mGame, "image/virustThree.png");
    mImgFour.setWindowAndPath(mGame, "image/virusFour.png");
    mImgFive.setWindowAndPath(mGame, "image/virusFive.png");
    mImgSix.setWindowAndPath(mGame, "image/virusSix.png");
    mImgSeven.setWindowAndPath(mGame, "image/virusSeven.png");

    // Load the image for the square selector
    mImgSelector.setWindowAndPath(mGame, "image/cursor.png");

    // Initialise the sounds
    mGameBoardSounds.loadResources();
}

void GameBoard::update()
{
    // Default state, do nothing
    if (mState == eSteady)
    {
        mMultiplier = 0;
        mAnimationCurrentStep = 0;
    }

    // Board appearing, viruses are falling
    else if (mState == eBoardAppearing)
    {
        // Update the animation frame
        mAnimationCurrentStep ++;

        // If the Animation.has finished, switch to steady state
        if (mAnimationCurrentStep == mAnimationLongTotalSteps)
        {
            mState = eSteady;
        }
    }

    // Two winning viruses are switching places
    else if (mState == eVirusSwitching)
    {
        // Update the animation frame
        mAnimationCurrentStep ++;

        // If the Animation.has finished, matching viruses should disappear
        if (mAnimationCurrentStep == mAnimationShortTotalSteps)
        {
            // Winning games should disappear
            mState = eViruesDisappearing;

            // Reset the animation
            mAnimationCurrentStep = 0;

            // Swap the viruses in the board
            mBoard.swap(mSelectedSquareFirst.x, mSelectedSquareFirst.y,
                        mSelectedSquareSecond.x, mSelectedSquareSecond.y);

            // Increase the mMultiplier
            ++mMultiplier;

            // Play matching sound
            playMatchSound();

            // Create floating scores for the matching group
            createFloatingScores();
        }
    }

    // Matched viruses are disappearing
    else if (mState == eVirusesDisappearing)
    {
        // Update the animation frame
        mAnimationCurrentStep ++;

        // If the Animation.has finished
        if (mAnimationCurrentStep == mAnimationShortTotalSteps)
        {
            // Empty spaces should be filled with new viruses
            mState = eBoardFilling;

            // Delete the squares that were matched in the board
            for(size_t i = 0; i < mGroupedSquares.size(); ++i)
            {
                for(size_t j = 0; j < mGroupedSquares[i].size(); ++j)
                {
                    mBoard.del(mGroupedSquares[i][j].x, mGroupedSquares[i][j].y);
                }
            }

            // Calculate fall movements
            mBoard.calcFallMovements();

            // Reset the animation
            mAnimationCurrentStep = 0;
        }
    }

    // New viruses are falling to their proper places
    else if (mState == eBoardFilling)
    {
        // Update the animation frame
        mAnimationCurrentStep ++;

        // If the Animation.has finished
        if (mAnimationCurrentStep == mAnimationShortTotalSteps)
        {
            // Play the fall sound
            mGameBoardSounds.playSoundFall();

            // Switch to the normal state
            mState = eSteady;

            // Reset the animation
            mAnimationCurrentStep = 0;

            // Reset animations in the board
            mBoard.endAnimations();

            // Check if there are matching groups
            mGroupedSquares = mBoard.check();

            // If there are...
            if (! mGroupedSquares.empty())
            {
                // Increase the score mMultiplier
                ++mMultiplier;

                // Create the floating scores
                createFloatingScores();

                // Play matching sound
                playMatchSound();

                // Go back to the viruses-fading mState
                mState = eVirusesDisappearing;
            }

            // If there are neither current solutions nor possible future solutions
            else if (mBoard.solutions().empty())
            {
                // Make the board disappear
                mState = eBoardDisappearing;

                // Make all the virus want to go outside the board
                mBoard.dropAllViruses();
            }
        }
    }

    // The entire board is disappearing to show a new one
    else if (mState == eBoardDisappearing)
    {
        // Update the animation frame
        mAnimationCurrentStep ++;

        // If the Animation.has finished
        if (mAnimationCurrentStep == mAnimationLongTotalSteps)
        {
            // Reset animation counter
            mAnimationCurrentStep = 0;

            // Generate a brand new board
            mBoard.generate();

            // Switch state
            mState = eBoardAppearing;
        }
    }

    // The board is disappearing because the time has run out
    else if (mState == eTimeFinished)
    {
        // Update the animation frame
        mAnimationCurrentStep ++;

        // If the Animation.has finished
        if (mAnimationCurrentStep == mAnimationLongTotalSteps)
        {
            // Reset animation counter
            mAnimationCurrentStep = 0;

            // Switch state
            mState = eShowingScoreTable;
        }
    }

    // Remove the hidden floating score
    mFloatingScores.erase(
        remove_if (mFloatingScores.begin(),
                   mFloatingScores.end(),
                   std::bind<bool>(&FloatingScore::ended, _1)),
        mFloatingScores.end());

}

void GameBoard::draw()
{
    // Get mouse position
    int mX = (int) mGame -> getMouseX();
    int mY = (int) mGame -> getMouseY();

    // Draw the selector if the mouse is over a virus
    if (overVirus(mX, mY) )
    {
        // Draw the selector over that virus
        mImgSelector.draw(
            241 + getCoord(mX, mY).x * 65,
            41 + getCoord(mX, mY).y * 65,
            4);
    }

    // Draw the selector if a virus has been selected
    if (mState == eVirusSelected)
    {
        mImgSelector.draw(241 + mSelectedSquareFirst.x * 65,
              41 + mSelectedSquareFirst.y * 65,
              4, 1, 1, 0, 255, {0, 255, 255, 255});
    }

    

    // Draw each floating score
    std::for_each(mFloatingScores.begin(),
      mFloatingScores.end(),
      std::bind(&FloatingScore::draw, _1));

    // If game has finished, draw the score table
    if (mState == eShowingScoreTable)
    {
        scoreTable -> draw(241 + (65 * 8) / 2 - 150  , 105, 3);
    }


    // On to the virus drawing procedure. Let's have a pointer to the image of each virus
    GoSDL::Image * img = NULL;

    // Top left position of the board
    int posX = 241;
    int posY = 41;

    for(int i = 0; i < 8; ++i)
    {
        for(int j = 0; j < 8; ++j)
        {
            // Reset the pointer
            img = NULL;

            // Check the type of each square and
            // save the proper image in the img pointer
            switch(mBoard.squares[i][j])
            {
                case sqWhite:
                img = &mImgWhite;
                break;

                case sqRed:
                img = &mImgRed;
                break;

                case sqPurple:
                img = &mImgPurple;
                break;

                case sqOrange:
                img = &mImgOrange;
                break;

                case sqGreen:
                img = &mImgGreen;
                break;

                case sqYellow:
                img = &mImgYellow;
                break;

                case sqBlue:
                img = &mImgBlue;
                break;

                case sqEmpty:
                img = NULL;
                break;
            }

            if (img == NULL)
            {
                continue;
            }

            float imgX = posX + i * 65;
            float imgY = posY + j * 65;
            float imgAlpha = 255;

            // When the board is first appearing, all the viruses are falling
            if (mState == eBoardAppearing)
            {
                imgY = Animacion::easeOutQuad(
                        float(mAnimationCurrentStep),
                        float(posY + mBoard.squares[i][j].origY * 65),
                        float(mBoard.squares[i][j].destY * 65),
                        float(mAnimationLongTotalSteps));
            }

            // When two correct viruses have been selected, they switch positions
            else if (mState == eVirusSwitching)
            {
                // If the current virus is the first selected square
                if (mSelectedSquareFirst.equals(i,j))
                {
                    imgX = Animacion::easeOutQuad(
                        float(mAnimationCurrentStep),
                        float(posX + i * 65),
                        float((mSelectedSquareSecond.x - mSelectedSquareFirst.x) * 65),
                        float(mAnimationShortTotalSteps));

                    imgY = Animacion::easeOutQuad(
                        float(mAnimationCurrentStep),
                        float(posY + j * 65),
                        float((mSelectedSquareSecond.y - mSelectedSquareFirst.y) * 65),
                        float(mAnimationShortTotalSteps));
                }

                // If the current virus is the second selected square
                else if (mSelectedSquareSecond.equals(i,j))
                {
                    imgX = Animacion::easeOutQuad(
                        float(mAnimationCurrentStep),
                        float(posX + i * 65),
                        float((mSelectedSquareFirst.x - mSelectedSquareSecond.x) * 65),
                        float(mAnimationShortTotalSteps));

                    imgY = Animacion::easeOutQuad(
                        float(mAnimationCurrentStep),
                        float(posY + j * 65),
                        float((mSelectedSquareFirst.y - mSelectedSquareSecond.y) * 65),
                        float(mAnimationShortTotalSteps));
                }
            }

            // When the two selected viruses have switched, the matched viruses disappear
            else if (mState == eVirusDisappearing)
            {
                if (mGroupedSquares.matched(Coord(i, j)))
                {
                    imgAlpha = 255 * (1 -(float)mAnimationCurrentStep/mAnimationShortTotalSteps);
                }
            }

            // When matched viruses have disappeared, spaces in the board must be filled
            else if (mState == eBoardFilling)
            {
                if (mBoard.squares[i][j].mustFall)
                {
                    imgY = Animacion::easeOutQuad(
                        float(mAnimationCurrentStep),
                        float(posY + mBoard.squares[i][j].origY * 65),
                        float(mBoard.squares[i][j].destY * 65),
                        float(mAnimationShortTotalSteps));
                }
            }

            // When there are no more matching movements, the board disappears
            else if (mState == eBoardDisappearing || mState == eTimeFinished)
            {
                imgY = Animacion::easeInQuad(
                        float(mAnimationCurrentStep),
                        float(posY + mBoard.squares[i][j].origY * 65),
                        float(mBoard.squares[i][j].destY * 65),
                        float(mAnimationLongTotalSteps));
            }

            else if (mState == eShowingScoreTable)
            {
                continue;
            }

            img->draw(imgX, imgY, 3, 1, 1, 0, imgAlpha);
        }


    }
}

void GameBoard::mouseButtonDown(int mouseX, int mouseY)
{
    // A virus was clicked
    if (overVirus(mouseX, mouseY))
    {
        mGameBoardSounds.playSoundSelect();

        // If there's no virus selected
        if (mState == eSteady)
        {
            mState = eVirusSelected;

            mSelectedSquareFirst.x = getCoord(mouseX, mouseY).x;
            mSelectedSquareFirst.y = getCoord(mouseX, mouseY).y;
        }

        // If there was previous a virus selected
        else if (mState == eVirusSelected)
        {
            // If the newly clicked virus is a winning one
            if (checkClickedSquare(mouseX, mouseY))
            {
                // Switch the state and reset the animation
                mState = eVirusSwitching;
                mAnimationCurrentStep = 0;
            }
            else
            {
                mState = eSteady;
                mSelectedSquareFirst.x = -1;
                mSelectedSquareFirst.y = -1;
            }
        }

    } //*/
}

void GameBoard::mouseButtonUp(int mX, int mY)
{
    if (mState == eVirusSelected)
    {
        // Get the coordinates where the mouse was released
        Coord res = getCoord(mX, mY);

        // If the square is different from the previously selected one
        if (res != mSelectedSquareFirst && checkClickedSquare(mX, mY))
        {
            // Switch the state and reset the animation
            mState = eVirusSwitching;
            mAnimationCurrentStep = 0;
        }
    }
}

void GameBoard::createFloatingScores()
{
    // For each match in the group of matched squares
    for (Match & m : mGroupedSquares)
    {
        // Create a new floating score image
        mFloatingScores.emplace_back(FloatingScore(mGame,
           m.size() * 5 * mMultiplier,
           m.midSquare().x,
           m.midSquare().y, 80));

        mStateGame->increaseScore(m.size() * 5 * mMultiplier);
    }
}

void GameBoard::playMatchSound()
{
    if (mMultiplier == 1)
    {
        mGameBoardSounds.playSoundMatch1();
    }
    else if (mMultiplier == 2)
    {
        mGameBoardSounds.playSoundMatch2();
    }
    else
    {
        mGameBoardSounds.playSoundMatch3();
    }
}

/// Tests if the mouse is over a virus
bool GameBoard::overVirus (int mX, int mY)
{
    return (mX > 241 && mX < 241 + 65 * 8 &&
        mY > 41 && mY < 41 + 65 * 8);
}

/// Returns the coords of the virus the mouse is over
Coord GameBoard::getCoord (int mX, int mY)
{
    return Coord((mX - 241) / 65 ,
       (mY - 41) / 65 );
}

bool GameBoard::checkClickedSquare(int mX, int mY)
{
    // Get the selected square
    mSelectedSquareSecond = getCoord(mX, mY);

    // If it's a contiguous square
    if (abs(mSelectedSquareFirst.x - mSelectedSquareSecond.x) +
        abs(mSelectedSquareFirst.y - mSelectedSquareSecond.y) == 1)
    {
        // Create a temporal board with the movement already performed
        Board temporal = mBoard;
        temporal.swap(mSelectedSquareFirst.x, mSelectedSquareFirst.y,
                      mSelectedSquareSecond.x, mSelectedSquareSecond.y);

        // Check if there are grouped viruses in that new board
        mGroupedSquares = temporal.check();

        // If there are winning movements
        if (! mGroupedSquares.empty())
        {
            return true;
        }
    }

    return false;
}
