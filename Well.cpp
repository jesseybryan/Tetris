#include "Well.h"
#include "Game.h"
#include "Piece.h"
#include "UserInterface.h"
#include <vector>
using namespace std;

Well::Well(int width, int height)
: m_width(width), m_height(height), setPieces(width+2, vector<char>(height+1, ' '))
{
    //Construct Well Walls
    for (int j = 0; j < m_height+1; j++){
        setPieces[0][j] = '@';
        setPieces[width+1][j] = '@';
    }
    //Construct Well Floor
    for(int i = 1; i < m_width+1; i++){
        setPieces[i][height] = '@';
    }
}

void Well::display(Screen& screen, int x, int y) const
{
    //for some reason, i and j do not make the whole screen when initialized to
    //x and y. For example, when I pass 0 and 0 for x and y, if I set i and j = to
    //x and y, the whole screen isn't displayed but if I just set i and j to 0 myself,
    //the whole screen appears
    for(int i = 0; i < setPieces.size(); i++){
        for(int j = 0; j < setPieces[0].size(); j++){
            screen.gotoXY(i, j);
            screen.printChar(setPieces[i][j]);
        }
    }
    screen.refresh();
}

int Well::wellHeight() const{
    return m_height;
}

int Well::wellWidth() const{
    return m_width;
}

//erases all the blocks in the well
void Well::clearScreen(){
    for(int i = 1; i < setPieces.size()-1; i++){
        for(int j = 1; j < setPieces[0].size()-1; j++){
            setPieces[i][j] = ' ';
        }
    }
}

void Well::setBlock(int x, int y, char c){
    if (x < 0 || x > wellWidth()+1 || y > wellHeight() || y < 0)//check that x and y are in bounds
        return;
    setPieces[x][y] = c;
}

char Well::getBlock(int x, int y) const{
    if (x < 0 || x > wellWidth()+1 || y > wellHeight() || y < 0)//check that x and y are in bounds
        return ' ';
    return setPieces[x][y];
}

bool Well::checkOpenBelow(Piece *p) const{
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            if (p->getBoundingBoxChar(i, j) == '#'){//where there is a character
                if (p->getX() + i >= 0 && p->getX() + i <= wellWidth() + 1 && p->getY() + j <= wellHeight() && p->getY() + j >= 0){ //check that x and y are in bounds
                    if (setPieces[p->getX() + i][p->getY() + j] == '@' || setPieces[p->getX() + i][p->getY() + j] == '$' || setPieces[p->getX() + i][p->getY() + j] == '*'){
                    return false; //if there is a set block there, it is not open
                    }
                }
            }
        }
    }
    return true;
}

bool Well::checkOpenLeft(Piece *p) const{
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            if (p->getBoundingBoxChar(i, j) == '#'){
                if (p->getX() + i -1 >= 0 && p->getX() + i - 1 <= wellWidth() + 1 && p->getY() + j <= wellHeight() && p->getY() + j >= 0){
                    if (setPieces[p->getX() + i - 1][p->getY() + j] == '@' || setPieces[p->getX() + i - 1][p->getY() + j] == '$' || setPieces[p->getX() + i - 1][p->getY() + j] == '*'){
                    return false; //if there is a set block to the left, it is not open
                    }
                }
            }
        }
    }
    return true;
}

bool Well::checkOpenRight(Piece *p) const{
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            if (p->getBoundingBoxChar(i, j) == '#'){
                if (p->getX() + i + 1 >= 0 && p->getX() + i + 1 <= wellWidth() + 1 && p->getY() + j <= wellHeight() && p->getY() + j >= 0){
                    if(setPieces[p->getX() + i + 1][p->getY() + j] == '@' || setPieces[p->getX() + i + 1][p->getY() + j] == '$' || setPieces[p->getX() + i + 1][p->getY() + j] == '*'){
                    return false; //if there is a set block to the right, it is not open
                    }
                }
            }
        }
    }
    return true;
}

bool Well::checkIfFullRow(Well well, int row) const{
    if (row >= 0 && row <= wellHeight()){ //check that the row is valid
        for(int i = 1; i < well.wellWidth() + 1; i++){
            if (setPieces[i][row] == ' ')
                return false; //if any column has a space in this row, the row is not full
        }
    }
    return true;
}

void Well::vaporize(Screen& screen, Well well, int row){
    if (row >= 0 && row <= wellHeight()){ //check that the row is valid
        for(; row > 0 ; row--){ //loop to the top of the screen
            for (int i = 1; i < well.wellWidth() + 1; i++){
                char c = setPieces[i][row - 1];
                setPieces[i][row] = c; //lowering each row
                screen.gotoXY(i, row);
                screen.printChar(c);
            }
        }
    }
}

