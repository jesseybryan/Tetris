#include "Game.h"
#include "Well.h"
#include "Piece.h"
#include "UserInterface.h"
#include <string>
#include <algorithm> //for max
#include <queue>
using namespace std;

const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT = 25;

const int WELL_X = 0;
const int WELL_Y = 0;

const int PROMPT_Y = 20;
const int PROMPT_X = 0;

const int SCORE_X = 16;
const int SCORE_Y = 8;

const int ROWS_LEFT_X = 16;
const int ROWS_LEFT_Y = 9;

const int LEVEL_X = 16;
const int LEVEL_Y = 10;

const int NEXT_PIECE_TITLE_X = 16;
const int NEXT_PIECE_TITLE_Y = 3;

const int NEXT_PIECE_X = 16;
const int NEXT_PIECE_Y = 4;

Game::Game(int width, int height)
: m_well(width, height), m_screen(SCREEN_WIDTH, SCREEN_HEIGHT), m_level(1), rowsToBeVaporized(5), qPressed(false), scoreMultiplier(0), score(0), firstPiece(nullptr), nextPiece(nullptr), gameOver(false)
{}

Game::~Game(){}

Piece* Game::getNextPiece(){
    Piece* p;
    switch (chooseRandomPieceType()) {
        case PIECE_FOAM:
            p = new FoamBomb(this, m_well);
            break;
        case PIECE_VAPOR:
            p = new VaporBomb(this, m_well);
            break;
        case PIECE_CRAZY:
            p = new CrazyShape(this, m_well);
            break;
        case PIECE_I:
            p = new Piece_I(this, m_well);
            break;
        case PIECE_O:
            p = new Piece_O(this, m_well);
            break;
        case PIECE_S:
            p = new Piece_S(this, m_well);
            break;
        case PIECE_Z:
            p = new Piece_Z(this, m_well);
            break;
        case PIECE_L:
            p = new Piece_L(this, m_well);
            break;
        case PIECE_J:
            p = new Piece_J(this, m_well);
            break;
        case PIECE_T:
            p = new Piece_T(this, m_well);
            break;
    }
    return p;
}


void Game::play()
{
    m_well.display(m_screen, WELL_X, WELL_Y); //display the well
    displayStatus();  //  score, rows left, level
    displayPrompt("Press the Enter key to begin playing Chetyris!");
    waitForEnter();  // [in UserInterface.h]
    for(;;)
    {
        firstPiece = getNextPiece();
        nextPiece = getNextPiece();
        while(!playOneLevel()){ //while there are more rows to vaporize
            displayStatus(); //now nextPiece will be displayed
            drop(m_screen, m_well, firstPiece, m_level); //drop the firstPiece
            if (qPressed || gameOver) //user quit or lost
                break;
            displayStatus();
            delete firstPiece;
            firstPiece = nextPiece; //grab the nextPiece to drop
            nextPiece = getNextPiece(); //grab a new nextPiece
        }
        if (qPressed || gameOver){
            delete firstPiece;
            delete nextPiece;
            break;
        }
        displayPrompt("Good job!  Press the Enter key to start next level!");
        waitForEnter();
        m_well.clearScreen();
        m_level++;
        rowsToBeVaporized = m_level*5;
        displayStatus();
        delete firstPiece;
        delete nextPiece;
    }
    displayPrompt("Game Over!  Press the Enter key to exit!");
    waitForEnter();
}


void Game::displayPrompt(std::string s)     
{
    m_screen.gotoXY(PROMPT_X, PROMPT_Y);
    m_screen.printStringClearLine(s);   // overwrites previous text
    m_screen.refresh();
}

void Game::displayStatus()
{
    if (nextPiece != nullptr){ //if we have a nextPiece, display it
        m_screen.gotoXY(NEXT_PIECE_TITLE_X, NEXT_PIECE_TITLE_Y);
        m_screen.printString("Next piece:");
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++){
                m_screen.gotoXY(NEXT_PIECE_X+i, NEXT_PIECE_Y+j);
                m_screen.printChar(' ');
            }
        }
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++){
                if(nextPiece->getBoundingBoxChar(i, j) == '#'){
                    m_screen.gotoXY(NEXT_PIECE_X+i, NEXT_PIECE_Y+j);
                    m_screen.printChar(nextPiece->getBoundingBoxChar(i, j));
                }
            }
        }
    }
    m_screen.gotoXY(SCORE_X, SCORE_Y);
    m_screen.printString("Score:");
    m_screen.gotoXY(SCORE_X+11, SCORE_Y);
    m_screen.printStringClearLine(" ");
    string scoreString = to_string(score);
    m_screen.gotoXY(SCORE_X + 18 - scoreString.size(), SCORE_Y);
    m_screen.printStringClearLine(scoreString);
    
    m_screen.gotoXY(ROWS_LEFT_X, ROWS_LEFT_Y);
    m_screen.printString("Rows left: ");
    string rowString = to_string(rowsToBeVaporized);
    m_screen.gotoXY(ROWS_LEFT_X+11, ROWS_LEFT_Y);
    m_screen.printStringClearLine(" ");
    m_screen.gotoXY(ROWS_LEFT_X + 18 - rowString.size(), ROWS_LEFT_Y);
    m_screen.printStringClearLine(rowString);
    
    m_screen.gotoXY(LEVEL_X, LEVEL_Y);
    m_screen.printString("Level:");
    m_screen.gotoXY(LEVEL_X+11, LEVEL_Y);
    m_screen.printStringClearLine(" ");
    string levelString = to_string(m_level);
    m_screen.gotoXY(LEVEL_X + 18 - levelString.size(), LEVEL_Y);
    m_screen.printStringClearLine(levelString);
    m_screen.refresh();
}

bool Game::playOneLevel()
{
    if (rowsToBeVaporized == 0)
        return true;
    return false;
}

void Game::drop(Screen &screen, Well& well, Piece *p, int level){
    double milliseconds = max(1000 - (100*(level-1)), 100);
    Timer timer;
    bool faster = false;
    bool levelOver = false;
    queue<int> rowsToDelete;
    if (gameOver){ //if the game is over, display where the piece was going to be and end it
        p->displayPiece(this, m_well, '#');
        well.display(m_screen, WELL_X, WELL_Y);
        return;
    }
    for(;well.checkOpenBelow(p); p->setY(p->getY() + 1)){ //keep dropping until something is below
        well.display(screen, WELL_X, WELL_Y);
        timer.start();
        while (timer.elapsed() < milliseconds)
        {
            char ch;
            if (getCharIfAny(ch))
            {
                switch (ch)
                {
                    case 'q':
                    case 'Q':
                        qPressed = true;
                        return;
                    case ' ':
                        p->moveToBottom(screen, well, this); //move as far down as you can
                        for(int j = 0; j < well.wellHeight(); j++){ //check if any rows are full
                                if (well.checkIfFullRow(well, j)){
                                    rowsToDelete.push(j);
                                    scoreMultiplier++;
                                    rowsToBeVaporized--;
                                    if (rowsToBeVaporized < 0){
                                        rowsToBeVaporized = 0;
                                        continue;
                                    }
                                    if (rowsToBeVaporized == 0){
                                        levelOver=true;;
                                    }
                                }
                        }
                        if (!levelOver){ //if the level isn't over, get all the rows we saved to delete
                            while(!rowsToDelete.empty()){
                                int row = rowsToDelete.front();
                                rowsToDelete.pop();
                                well.vaporize(screen, well, row);
                            }
                        }
                        addScore();
                        well.display(screen, WELL_X, WELL_Y);
                        return; //end it
                    case ARROW_LEFT:
                    case 'a':
                        if(p->getPieceType() == PIECE_CRAZY){ //if it's a crazy piece, check right
                            if (well.checkOpenRight(p)){
                                p->movePiece(this, well, -1);
                                well.display(screen, WELL_X, WELL_Y);
                            }
                        }
                        else{ //it's a normal piece, so check left while moving left
                            if(well.checkOpenLeft(p)){
                                p->movePiece(this, well, -1);
                                well.display(screen, WELL_X, WELL_Y);
                            }
                        }
                        break;
                    case ARROW_RIGHT:
                    case 'd':
                        if(p->getPieceType() == PIECE_CRAZY){ //if it's a crazy piece, check left
                            if (well.checkOpenLeft(p)){
                                p->movePiece(this, well, 1);
                                well.display(screen, WELL_X, WELL_Y);
                            }
                        }
                        else{ //it's a normal piece, so check right while moving right
                            if (well.checkOpenRight(p)){
                                p->movePiece(this, well, 1);
                                well.display(screen, WELL_X, WELL_Y);
                            }
                        }
                        break;
                    case ARROW_DOWN:
                    case 's':
                        faster = true;
                        break; //break the timer and go faster
                    case ARROW_UP:
                    case 'w':
                        p->rotate(this, well);
                        well.display(screen, WELL_X, WELL_Y);
                        break;
                }
            }
            if (faster){
                faster = false;
                break;
            }
        }//end while
        p->movePiece(this, well, 0); //don't move the piece left or right
        well.display(screen, WELL_X, WELL_Y);
    }//end for
    p->setY(p->getY()-1); //loop went one spot too far
    if (p->getPieceType() == PIECE_VAPOR){
        p->displayPiece(this, well, ' ');
    }
    else if (p->getPieceType() == PIECE_FOAM){
        p->displayPiece(this, well, '*');
    }
    else{
        p->displayPiece(this, well, '$');
    }
    p->setBlocksInPlace(this, screen, well); //set the blocks
    for(int j = 0; j < well.wellHeight(); j++){ //check for full rows and vaporize
        if (well.checkIfFullRow(well, j)){
            rowsToDelete.push(j);
            scoreMultiplier++;
            rowsToBeVaporized--;
            if (rowsToBeVaporized < 0){
                rowsToBeVaporized = 0;
                continue;
            }
            if (rowsToBeVaporized == 0){
                levelOver = true;
            }
        }
    }
    if (!levelOver){
        while(!rowsToDelete.empty()){
            int row = rowsToDelete.front();
            rowsToDelete.pop();
            well.vaporize(screen, well, row);
        }
    }
    addScore();
    well.display(screen, WELL_X, WELL_Y);
}


void Game::addScore(){
    if (scoreMultiplier == 1)
        score += 100;
    else if (scoreMultiplier == 2)
        score += 200;
    else if (scoreMultiplier == 3)
        score += 400;
    else if (scoreMultiplier == 4)
        score += 800;
    else if (scoreMultiplier == 5)
        score += 1600;
    scoreMultiplier = 0;
}

void Game::setGameOver(bool b){
    gameOver = b;
}

bool Game::getGameOver() const{
    return gameOver;
}
