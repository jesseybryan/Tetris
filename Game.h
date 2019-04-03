#ifndef GAME_INCLUDED
#define GAME_INCLUDED

#include "Well.h"
#include "UserInterface.h"
#include <string>
class Piece;

class Game
{
  public:
    Game(int width, int height);
    ~Game();
    Piece* getNextPiece();
    void play();
    void displayPrompt(std::string s);
    void displayStatus();
    bool playOneLevel();
    void drop(Screen& screen, Well& well, Piece* p, int level);
    void addScore();
    void setGameOver(bool b);
    bool getGameOver() const;
  private:
    Well    m_well;
    Screen  m_screen;
    int     m_level;
    int rowsToBeVaporized;
    bool qPressed;
    int scoreMultiplier;
    int score;
    Piece* firstPiece;
    Piece* nextPiece;
    bool gameOver;
};

#endif // GAME_INCLUDED
