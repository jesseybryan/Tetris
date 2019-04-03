#ifndef PIECE_INCLUDED
#define PIECE_INCLUDED

#include "Well.h"
#include "UserInterface.h"
class Game;

/////////////////////////////////////
enum PieceType {
    PIECE_I, PIECE_L, PIECE_J, PIECE_T, PIECE_O, PIECE_S, PIECE_Z,
    PIECE_VAPOR, PIECE_FOAM, PIECE_CRAZY, NUM_PIECE_TYPES
};
PieceType chooseRandomPieceType();
/////////////////////////////////////

class Piece{
public:
    Piece(Game* game, PieceType pieceType);
    virtual ~Piece();
    //getters
    PieceType getPieceType() const;
    int getX() const;
    int getY() const;
    int getXOffset() const;
    char getOrientation(int r, int x, int y) const;
    int getRotation() const;
    int getPieceWidth() const;
    char getBoundingBoxChar(int x, int y) const;
    int nextRotation();
    //setters
    void setX(int x);
    void setY(int y);
    void setOrientation(int r, int x, int y, char c, Game* game, Well& well);
    //other methods
    virtual void movePiece(Game* game, Well& well, int x);
    virtual void moveToBottom(Screen& screen, Well& well, Game* game);
    virtual void rotate(Game* game, Well& well);
    virtual void displayPiece(Game* game, Well& well, char c);
    virtual void setBlocksInPlace(Game* game, Screen& screen, Well& well);
    virtual void explode(Game* game, Screen& screen, Well& well, int xLeft, int xRight, int yUp, int yDown);
private:
    int m_x;
    int m_y;
    int rotation;
    Game* m_game;
    PieceType m_pieceType;
    char orientation[4][4][4];
};

/////////////////////////////////////
class Piece_I : public Piece{
public:
    Piece_I(Game* game, Well& well);
private:
};

/////////////////////////////////////
class Piece_O : public Piece{
public:
    Piece_O(Game* game, Well& well);
    virtual void rotate(Game* game, Well& well);
private:
};

/////////////////////////////////////
class Piece_T : public Piece{
public:
    Piece_T(Game* game, Well& well);
private:
};

/////////////////////////////////////
class Piece_L : public Piece{
public:
    Piece_L(Game* game, Well& well);
private:
};

/////////////////////////////////////
class Piece_J : public Piece{
public:
    Piece_J(Game* game, Well& well);
private:
};

/////////////////////////////////////
class Piece_S : public Piece{
public:
    Piece_S(Game* game, Well& well);
private:
};

/////////////////////////////////////
class Piece_Z : public Piece{
public:
    Piece_Z(Game* game, Well& well);
private:
};

/////////////////////////////////////
class CrazyShape: public Piece{
public:
    CrazyShape(Game* game, Well& well);
    virtual void movePiece(Game* game, Well& well, int x);
private:
};

/////////////////////////////////////
class VaporBomb : public Piece{
public:
    VaporBomb(Game* game, Well& well);
    virtual void rotate(Game* game, Well& well);
    virtual void setBlocksInPlace(Game* game, Screen& screen, Well& well);
private:
};

/////////////////////////////////////
class FoamBomb : public Piece{
public:
    FoamBomb(Game* game, Well& well);
    virtual void rotate(Game* game, Well& well);
    virtual void explode(Game* game, Screen& screen, Well& well, int xLeft, int xRight, int yUp, int yDown);
private:
};


#endif // PIECE_INCLUDED
