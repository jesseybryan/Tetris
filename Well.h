#ifndef WELL_INCLUDED
#define WELL_INCLUDED

#include <vector>
class Screen;
class Game;
class Piece;

class Well
{
  public:
    Well(int width, int m_height);
    void display(Screen& screen, int x, int y) const;
    int wellHeight() const;
    int wellWidth() const;
    void clearScreen();
    void setBlock(int x, int y, char c);
    char getBlock(int x, int y) const;
    bool checkOpenBelow(Piece* p) const;
    bool checkOpenLeft(Piece* p) const;
    bool checkOpenRight(Piece* p) const;
    bool checkIfFullRow(Well well, int row) const;
    void vaporize(Screen& screen, Well well, int row);
  private:
    int m_width;
    int m_height;
    //2D Vector holds 
    std::vector<std::vector<char>> setPieces;
};

#endif // WELL_INCLUDED
