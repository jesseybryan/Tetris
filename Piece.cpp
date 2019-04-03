#include "Piece.h"
#include "Well.h"
#include "Game.h"
#include "UserInterface.h"
using namespace std;

Piece::Piece(Game* game, PieceType pieceType)
: m_x(4), m_y(0), rotation(0), m_game(game), m_pieceType(pieceType)
{
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            for(int k = 0; k < 4; k++){
                orientation[i][j][k] = ' ';
            }
        }
    }
}

Piece::~Piece(){}

PieceType Piece::getPieceType() const{
    return m_pieceType;
}

int Piece::getX() const{
    return m_x;
}

int Piece::getY() const{
    return m_y;
}

//find the distance the first # is off the left side of the bounding box
int Piece::getXOffset() const{
    bool getXOffset = false;
    int i = 0;
    for(; i < 4; i++){
        for(int y = 0; y < 4; y++){
            if (orientation[rotation][i][y] == '#'){
                getXOffset = true;
                break;
            }
        }
        if (getXOffset)
            break;
    }
    return i;
}

char Piece::getOrientation(int r, int x, int y) const{
    return orientation[r][x][y];
}

int Piece::getRotation() const{
    return rotation;
}

int Piece::getPieceWidth() const{
    bool firstX = true;
    int count = 0;
    for(int i = 0; i < 4; i++){
        for(int y = 0; y < 4; y++){
            if (orientation[rotation][i][y] == '#' && firstX){
                firstX = false;
                count++;
            }
        }
        firstX = true;
    }
    return count;
}

char Piece::getBoundingBoxChar(int x, int y) const{
    if(x >= 0 && x < 4 && y >= 0 && y < 4)
        return orientation[rotation][x][y];
    else //send back blank as error
        return ' ';
}

int Piece::nextRotation(){
    if (rotation + 1 == 4)
        return 0;
    return rotation + 1;
}



void Piece::setX(int x){
    m_x = x;
}

void Piece::setY(int y){
    m_y = y;
}

void Piece::setOrientation(int r, int x, int y, char c, Game* game, Well& well){
    if (m_x + x >= 0 && m_x + x <= well.wellWidth()+1 && m_y + y - 1 >= 0 && m_y + y - 1 <= well.wellHeight()){ //check bounds
        if (well.getBlock(getX() + x, getY() + y-1) == '$' || well.getBlock(getX() + x, getY() + y-1) == '*'){ //if we are constructing inside a piece, the game is over
            game->setGameOver(true);
        }
    }
    orientation[r][x][y] = c;
}


void Piece::movePiece(Game* game, Well& well, int x){
    displayPiece(game, well, ' ');//erase where it is
    m_y--;
    displayPiece(game, well, ' ');//erase where it was
    m_y++;
    m_x += x;
    displayPiece(game, well, '#'); //reconstruct
}

void Piece::moveToBottom(Screen& screen, Well& well, Game* game){
    //deconstruct the piece where it is and where it was
    displayPiece(game, well, ' ');
    m_y--;
    displayPiece(game, well, ' ');
    m_y++;
    //move to the bottom by finding the first solid piece below it
    bool foundFirstSolidPiece = false;
    for(;well.checkOpenBelow(this); m_y++){
        for(int j = m_x + getXOffset(); j < m_x + getPieceWidth(); j++){
            if (j >= 0 && j <= well.wellWidth()+1 && m_y <= well.wellHeight() && m_y >= 0){
                char c = well.getBlock(j, m_y);
                if(c == '@' || c == '$' || c == '*'){
                    foundFirstSolidPiece = true;
                    break;
                }
            }
        }
        if(foundFirstSolidPiece)
            break;
    }
    m_y--;
    if (getPieceType() == PIECE_VAPOR){
        displayPiece(game, well, ' ');
    }
    else if (getPieceType() == PIECE_FOAM){
        displayPiece(game, well, '*');
    }
    else{
        displayPiece(game, well, '$');
    }
    setBlocksInPlace(game, screen, well);
    screen.refresh();
}

void Piece::rotate(Game* game, Well& well){
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if (orientation[nextRotation()][i][j] == '#'){ //if the next rotation overlaps a set block
                if (m_x + i >= 0 && m_x + i <= well.wellWidth()+1 && m_y + j <= well.wellHeight() && m_y + j >= 0){ //check bounds
                    if (well.getBlock(m_x + i, m_y + j) == '@' || well.getBlock(m_x + i, m_y + j) == '$' ||well.getBlock(m_x + i, m_y + j) == '*'){
                        return; //do not rotate
                    }
                }
            }
        }
    }
    m_y--;
    displayPiece(game, well, ' ');
    m_y++;
    displayPiece(game, well, ' ');
    m_y--;
    rotation++; //go to next orientation
    if (rotation == 4)
        rotation = 0;
    displayPiece(game, well, '#');
    m_y++;
}

void Piece::displayPiece(Game* game, Well& well, char c){
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if (orientation[rotation][i][j] == '#'){
                if (getX()+i >= 0 && getX()+ i <= well.wellWidth()+1 && getY() + j <= well.wellHeight() && getY() + j >= 0){
                    if ((well.getBlock(getX() + i, getY() + j) == '$' || well.getBlock(getX() + i, getY() + j) == '*') && (!game->getGameOver())) //if the game is not over
                        continue; //don't override set blocks
                    else
                        well.setBlock(getX() + i, getY() + j, c);
                }
            }
        }
    }
}


void Piece::setBlocksInPlace(Game* game, Screen& screen, Well& well){
    if (getPieceType() == PIECE_FOAM)
        explode(game, screen, well, m_x-1, m_x+3, m_y-1, m_y+3);
    else{
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++){
                if (orientation[rotation][i][j] == '#'){
                    if (getX()+i >= 0 && getX()+ i <= well.wellWidth()+1 && getY() + j <= well.wellHeight() && getY() + j >= 0){
                        well.setBlock(getX() + i, getY() + j, '$'); //set the blocks
                    }
                }
            }
        }
    }
}

void Piece::explode(Game* game, Screen& screen, Well& well, int xLeft, int xRight, int yUp, int yDown){}

////////////////////////////////////////////////////
Piece_I::Piece_I(Game* game, Well& well)
: Piece(game, PIECE_I)
{
    //rotation 0
    setOrientation(0, 0, 1, '#', game, well);
    setOrientation(0, 1, 1, '#', game, well);
    setOrientation(0, 2, 1, '#', game, well);
    setOrientation(0, 3, 1, '#', game, well);
    //rotation 1
    setOrientation(1, 1, 0, '#', game, well);
    setOrientation(1, 1, 1, '#', game, well);
    setOrientation(1, 1, 2, '#', game, well);
    setOrientation(1, 1, 3, '#', game, well);
    //rotation 2
    setOrientation(2, 0, 1, '#', game, well);
    setOrientation(2, 1, 1, '#', game, well);
    setOrientation(2, 2, 1, '#', game, well);
    setOrientation(2, 3, 1, '#', game, well);
    //rotation 3
    setOrientation(3, 1, 0, '#', game, well);
    setOrientation(3, 1, 1, '#', game, well);
    setOrientation(3, 1, 2, '#', game, well);
    setOrientation(3, 1, 3, '#', game, well);
}

////////////////////////////////////////////////////
Piece_O::Piece_O(Game* game, Well& well)
: Piece(game, PIECE_O)
{
    //rotation 0
    setOrientation(0, 0, 0, '#', game, well);
    setOrientation(0, 1, 0, '#', game, well);
    setOrientation(0, 0, 1, '#', game, well);
    setOrientation(0, 1, 1, '#', game, well);
    //rotation 1
    setOrientation(1, 0, 0, '#', game, well);
    setOrientation(1, 1, 0, '#', game, well);
    setOrientation(1, 0, 1, '#', game, well);
    setOrientation(1, 1, 1, '#', game, well);
    //rotation 2
    setOrientation(2, 0, 0, '#', game, well);
    setOrientation(2, 1, 0, '#', game, well);
    setOrientation(2, 0, 1, '#', game, well);
    setOrientation(2, 1, 1, '#', game, well);
    //rotation 3
    setOrientation(3, 0, 0, '#', game, well);
    setOrientation(3, 1, 0, '#', game, well);
    setOrientation(3, 0, 1, '#', game, well);
    setOrientation(3, 1, 1, '#', game, well);
}

void Piece_O::rotate(Game* game, Well& well){}


////////////////////////////////////////////////////
Piece_T::Piece_T(Game* game, Well& well)
: Piece(game, PIECE_T)
{
    //rotation 0
    setOrientation(0, 1, 1, '#', game, well);
    setOrientation(0, 1, 0, '#', game, well);
    setOrientation(0, 0, 1, '#', game, well);
    setOrientation(0, 2, 1, '#', game, well);
    //rotation 1
    setOrientation(1, 1, 1, '#', game, well);
    setOrientation(1, 1, 0, '#', game, well);
    setOrientation(1, 1, 2, '#', game, well);
    setOrientation(1, 2, 1, '#', game, well);
    //rotation 2
    setOrientation(2, 1, 1, '#', game, well);
    setOrientation(2, 2, 1, '#', game, well);
    setOrientation(2, 1, 2, '#', game, well);
    setOrientation(2, 0, 1, '#', game, well);
    //rotation 3
    setOrientation(3, 1, 0, '#', game, well);
    setOrientation(3, 0, 1, '#', game, well);
    setOrientation(3, 1, 2, '#', game, well);
    setOrientation(3, 1, 1, '#', game, well);
}


////////////////////////////////////////////////////
Piece_L::Piece_L(Game* game, Well& well)
: Piece(game, PIECE_L)
{
    //rotation 0
    setOrientation(0, 0, 1, '#', game, well);
    setOrientation(0, 0, 2, '#', game, well);
    setOrientation(0, 1, 1, '#', game, well);
    setOrientation(0, 2, 1, '#', game, well);
    //rotation 1
    setOrientation(1, 1, 0, '#', game, well);
    setOrientation(1, 2, 0, '#', game, well);
    setOrientation(1, 2, 1, '#', game, well);
    setOrientation(1, 2, 2, '#', game, well);
    //rotation 2
    setOrientation(2, 2, 1, '#', game, well);
    setOrientation(2, 2, 2, '#', game, well);
    setOrientation(2, 1, 2, '#', game, well);
    setOrientation(2, 0, 2, '#', game, well);
    //rotation 3
    setOrientation(3, 1, 1, '#', game, well);
    setOrientation(3, 1, 2, '#', game, well);
    setOrientation(3, 1, 3, '#', game, well);
    setOrientation(3, 2, 3, '#', game, well);
}


////////////////////////////////////////////////////
Piece_J::Piece_J(Game* game, Well& well)
: Piece(game, PIECE_J)
{
    //rotation 0
    setOrientation(0, 1, 1, '#', game, well);
    setOrientation(0, 2, 1, '#', game, well);
    setOrientation(0, 3, 1, '#', game, well);
    setOrientation(0, 3, 2, '#', game, well);
    //rotation 1
    setOrientation(1, 2, 1, '#', game, well);
    setOrientation(1, 2, 2, '#', game, well);
    setOrientation(1, 2, 3, '#', game, well);
    setOrientation(1, 1, 3, '#', game, well);
    //rotation 2
    setOrientation(2, 1, 1, '#', game, well);
    setOrientation(2, 1, 2, '#', game, well);
    setOrientation(2, 2, 2, '#', game, well);
    setOrientation(2, 3, 2, '#', game, well);
    //rotation 3
    setOrientation(3, 2, 0, '#', game, well);
    setOrientation(3, 1, 0, '#', game, well);
    setOrientation(3, 1, 1, '#', game, well);
    setOrientation(3, 1, 2, '#', game, well);
}



////////////////////////////////////////////////////
Piece_S::Piece_S(Game* game, Well& well)
: Piece(game, PIECE_S)
{
    //rotation 0
    setOrientation(0, 0, 2, '#', game, well);
    setOrientation(0, 1, 2, '#', game, well);
    setOrientation(0, 1, 1, '#', game, well);
    setOrientation(0, 2, 1, '#', game, well);
    //rotation 1
    setOrientation(1, 1, 0, '#', game, well);
    setOrientation(1, 1, 1, '#', game, well);
    setOrientation(1, 2, 1, '#', game, well);
    setOrientation(1, 2, 2, '#', game, well);
    //rotation 2
    setOrientation(2, 0, 2, '#', game, well);
    setOrientation(2, 1, 2, '#', game, well);
    setOrientation(2, 1, 1, '#', game, well);
    setOrientation(2, 2, 1, '#', game, well);
    //rotation 3
    setOrientation(3, 1, 0, '#', game, well);
    setOrientation(3, 1, 1, '#', game, well);
    setOrientation(3, 2, 1, '#', game, well);
    setOrientation(3, 2, 2, '#', game, well);
}



////////////////////////////////////////////////////
Piece_Z::Piece_Z(Game* game, Well& well)
: Piece(game, PIECE_Z)
{
    //rotation 0
    setOrientation(0, 0, 1, '#', game, well);
    setOrientation(0, 1, 1, '#', game, well);
    setOrientation(0, 1, 2, '#', game, well);
    setOrientation(0, 2, 2, '#', game, well);
    //rotation 1
    setOrientation(1, 2, 0, '#', game, well);
    setOrientation(1, 2, 1, '#', game, well);
    setOrientation(1, 1, 1, '#', game, well);
    setOrientation(1, 1, 2, '#', game, well);
    //rotation 2
    setOrientation(2, 0, 1, '#', game, well);
    setOrientation(2, 1, 1, '#', game, well);
    setOrientation(2, 1, 2, '#', game, well);
    setOrientation(2, 2, 2, '#', game, well);
    //rotation 3
    setOrientation(3, 2, 0, '#', game, well);
    setOrientation(3, 2, 1, '#', game, well);
    setOrientation(3, 1, 1, '#', game, well);
    setOrientation(3, 1, 2, '#', game, well);
}


////////////////////////////////////////////////////
CrazyShape::CrazyShape(Game* game, Well& well)
: Piece(game, PIECE_CRAZY)
{
    //rotation 0
    setOrientation(0, 0, 0, '#', game, well);
    setOrientation(0, 1, 1, '#', game, well);
    setOrientation(0, 2, 1, '#', game, well);
    setOrientation(0, 3, 0, '#', game, well);
    //rotation 1
    setOrientation(1, 3, 0, '#', game, well);
    setOrientation(1, 2, 1, '#', game, well);
    setOrientation(1, 2, 2, '#', game, well);
    setOrientation(1, 3, 3, '#', game, well);
    //rotation 2
    setOrientation(2, 3, 3, '#', game, well);
    setOrientation(2, 2, 2, '#', game, well);
    setOrientation(2, 1, 2, '#', game, well);
    setOrientation(2, 0, 3, '#', game, well);
    //rotation 3
    setOrientation(3, 0, 3, '#', game, well);
    setOrientation(3, 1, 2, '#', game, well);
    setOrientation(3, 1, 1, '#', game, well);
    setOrientation(3, 0, 0, '#', game, well);
}

void CrazyShape::movePiece(Game* game, Well& well, int x){
    displayPiece(game, well, ' ');//erase where it is
    setY(getY()-1);
    displayPiece(game, well, ' ');//erase where it was
    setY(getY()+1);
    setX(getX()-x); //move opposite of input
    displayPiece(game, well, '#'); //reconstruct
}

////////////////////////////////////////////////////
VaporBomb::VaporBomb(Game* game, Well& well)
: Piece(game, PIECE_VAPOR)
{
    setOrientation(0, 2, 0, '#', game, well);
    setOrientation(0, 1, 0, '#', game, well);
    setOrientation(1, 2, 0, '#', game, well);
    setOrientation(1, 1, 0, '#', game, well);
    setOrientation(2, 2, 0, '#', game, well);
    setOrientation(2, 1, 0, '#', game, well);
    setOrientation(3, 2, 0, '#', game, well);
    setOrientation(3, 1, 0, '#', game, well);
}

void VaporBomb::rotate(Game* game, Well& well){}

//check the bounds for each piece below 2, below 1, above 1, and above 2, erasing if valid
void VaporBomb::setBlocksInPlace(Game* game, Screen& screen, Well& well){
    if (getX()+1 >= 0 && getX()+ 1 <= well.wellWidth()+1 && getY() -1 <= well.wellHeight() && getY() -1 >= 0){
        well.setBlock(getX() + 1, getY() - 1, ' ');
    }
    if (getX()+1 >= 0 && getX() + 1 <= well.wellWidth()+1 && getY() - 2 <= well.wellHeight() && getY() - 2 >= 0){
        well.setBlock(getX() + 1, getY() - 2, ' ');
    }
    if (getX()+2 >= 0 && getX()+ 2 <= well.wellWidth()+1 && getY() -1 <= well.wellHeight() && getY() -1 >= 0){
        well.setBlock(getX() + 2, getY() - 1, ' ');
    }
    if (getX()+2 >= 0 && getX() + 2 <= well.wellWidth()+1 && getY() - 2 <= well.wellHeight() && getY() - 2 >= 0){
        well.setBlock(getX() + 2, getY() - 2, ' ');
    }
    if (getX()+1 >= 0 && getX()+ 1 <= well.wellWidth()+1 && getY() + 1 <= well.wellHeight() && getY() + 1 >= 0 && getX()+2 >= 0 && getX()+ 2 <= well.wellWidth()+1 && getY() + 1 <= well.wellHeight() && getY() + 1 >= 0){
        if (well.getBlock(getX() + 1, getY() + 1) == '@' && well.getBlock(getX() + 2, getY() + 1) == '@'){
            return; //don't erase the bottom of the well
        }
        else{
            well.setBlock(getX() + 1, getY() + 1, ' ');
            well.setBlock(getX() + 2, getY() + 1, ' ');
        }
    }
    if (getX()+1 >= 0 && getX()+ 1 <= well.wellWidth()+1 && getY() + 2 <= well.wellHeight() && getY() + 2 >= 0 && getX()+2 >= 0 && getX()+ 2 <= well.wellWidth()+1 && getY() + 2 <= well.wellHeight() && getY() + 2 >= 0){
        if (well.getBlock(getX() + 1, getY() + 2) == '@' && well.getBlock(getX() + 2, getY() + 2) == '@'){
            return;
        }
        else{
            well.setBlock(getX() + 1, getY() + 2, ' ');
            well.setBlock(getX() + 2, getY() + 2, ' ');
        }
    }
}

////////////////////////////////////////////////////
FoamBomb::FoamBomb(Game* game, Well& well)
: Piece(game, PIECE_FOAM)
{
    setOrientation(0, 1, 1, '#', game, well);
    setOrientation(1, 1, 1, '#', game, well);
    setOrientation(2, 1, 1, '#', game, well);
    setOrientation(3, 1, 1, '#', game, well);
}

void FoamBomb::rotate(Game* game, Well& well){}

//same format as the maze recursion from Homework 3
void FoamBomb::explode(Game* game, Screen& screen, Well& well, int xLeft, int xRight, int yUp, int yDown){
    if(getX()+1 == xLeft-1 || getX()+1 == xRight+1 || getY()+1 == yUp-1 || getY()+1 == yDown+1)
        return;
    displayPiece(game, well, '*');
    if ( getX() + 1 >= 0 && getX() + 1 <= well.wellWidth()+1 && getY()+1 >= 0 && getY() +1 <= well.wellHeight()){
        well.setBlock(getX()+1, getY()+1, '*');
    }
    if ( getX() + 1 >= 0 && getX() + 1 <= well.wellWidth()+1 && getY() >= 0 && getY() <= well.wellHeight()){
        if (well.getBlock(getX()+1, getY()) == ' '){ //NORTH
            setY(getY()-1);
            explode(game, screen, well, xLeft, xRight, yUp, yDown);
            setY(getY()+1);
        }
    }
    if ( getX() >= 0 && getX() <= well.wellWidth()+1 && getY() + 1 >= 0 && getY()+1 <= well.wellHeight()){
        if (well.getBlock(getX(), getY()+1) == ' '){ //WEST
            setX(getX()-1);
            explode(game, screen, well, xLeft, xRight, yUp, yDown);
            setX(getX()+1);
        }
    }
    if ( getX() + 1 >= 0 && getX() + 1 <= well.wellWidth()+1 && getY()+2 >= 0 && getY()+2 <= well.wellHeight()){
        if (well.getBlock(getX()+1, getY()+2) == ' '){ //SOUTH
            setY(getY()+1);
            explode(game, screen, well, xLeft, xRight, yUp, yDown);
            setY(getY()-1);
        }
    }
    if ( getX() + 2 >= 0 && getX() + 2 <= well.wellWidth()+1 && getY()+1 >= 0 && getY()+1 <= well.wellHeight()){
        if (well.getBlock(getX()+2, getY()+1) == ' '){ //EAST
            setX(getX()+1);
            explode(game, screen, well, xLeft, xRight, yUp, yDown);
            setX(getX()-1);
        }
    }
    return;
}

