#include "UserInterface.h"
#include <ncurses.h>
#include <string>
using namespace std;

///////////////////////////////////////////////////////////
// Screen -- Visual Output
///////////////////////////////////////////////////////////

class ScreenImpl
{
  public:
    ScreenImpl(int width, int height);
    ~ScreenImpl();
    void clear();
    void gotoXY(int x, int y);
    void printChar(char ch);
    void printString(std::string s);
    void printStringClearLine(std::string s);
    void refresh();

  private:
    int m_width;
    int m_height;
};

ScreenImpl::ScreenImpl(int width, int height)
 : m_width(width), m_height(height)
{
    initscr();
    noecho();
    cbreak();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(0);
}

ScreenImpl::~ScreenImpl()
{
    endwin();
}

void ScreenImpl::clear()
{
    ::clear();
}

void ScreenImpl::gotoXY(int x, int y)
{
    if (x < 0  ||  x >= m_width  ||  y < 0  ||  y >= m_height)
        return;
    move(y, x);
}

void ScreenImpl::printChar(char ch)
{
    addch(ch);
}

void ScreenImpl::printString(std::string s)
{
    addstr(s.c_str());
}

void ScreenImpl::printStringClearLine(std::string s)
{
    if (s.size() > size_t(m_width))
        s.erase(m_width);
    printString(s);
    clrtoeol();
}

void ScreenImpl::refresh()
{
    gotoXY(m_width-1, m_height-1);
    ::refresh();
}

Screen::Screen(int width, int height)
{
    m_impl = new ScreenImpl(width, height);
}

Screen::~Screen()
{
    delete m_impl;
}

void Screen::clear()
{
    m_impl->clear();
}

void Screen::gotoXY(int x, int y)
{
    m_impl->gotoXY(x, y);
}

void Screen::printChar(char ch)
{
    m_impl->printChar(ch);
}

void Screen::printString(std::string s)
{
    m_impl->printString(s);
}

void Screen::printStringClearLine(std::string s)
{
    m_impl->printStringClearLine(s);
}

void Screen::refresh()
{
    m_impl->refresh();
}

///////////////////////////////////////////////////////////
// Functions for Keyboard Input
///////////////////////////////////////////////////////////

  // If the user has hit a key, set ch to that character and return true;
  // otherwise, leave ch unchanged and return false.
bool getCharIfAny(char& ch)
{
    int c = getch();
    switch (c)
    {
      case ERR:        return false;
      case KEY_LEFT:   ch = ARROW_LEFT;  break;
      case KEY_RIGHT:  ch = ARROW_RIGHT; break;
      case KEY_UP:     ch = ARROW_UP;    break;
      case KEY_DOWN:   ch = ARROW_DOWN;  break;
      case 'a':        ch = ARROW_LEFT;  break;
      case 'd':        ch = ARROW_RIGHT; break;
      case 'w':        ch = ARROW_UP;    break;
      case 's':        ch = ARROW_DOWN;  break;
      default:         ch = c;           break;
    }
    return true;
}

void waitForEnter()
{
    nodelay(stdscr, FALSE);
    int ch;
    while (ch = getch(), ch != '\n'  &&  ch != '\r')
        ;
    nodelay(stdscr, TRUE);
}

void discardPendingKeys()
{
    char ch;
    while (getCharIfAny(ch))
        ;
}
