#ifndef USERINTERFACE_INCLUDED
#define USERINTERFACE_INCLUDED

#include <string>

const char ARROW_UP    = '8';
const char ARROW_DOWN  = '2';
const char ARROW_LEFT  = '4';
const char ARROW_RIGHT = '6';

///////////////////////////////////////////////////////////
// Screen -- Visual Output
///////////////////////////////////////////////////////////

class ScreenImpl;

class Screen
{
  public:
    Screen(int width, int height);
    ~Screen();
    void clear();
    void gotoXY(int x, int y);
    void printChar(char ch);
    void printString(std::string s);
    void printStringClearLine(std::string s);
    void refresh();

  private:
    ScreenImpl* m_impl;
};

///////////////////////////////////////////////////////////
// Functions for Keyboard Input
///////////////////////////////////////////////////////////

bool getCharIfAny(char& ch);
void waitForEnter();
void discardPendingKeys();

///////////////////////////////////////////////////////////
// Class for Timing
///////////////////////////////////////////////////////////

//========================================================================
// Timer t;                 // create and start a timer
// t.start();               // restart the timer
// double d = t.elapsed();  // milliseconds since timer was last started
//========================================================================

#include <chrono>

class Timer
{
  public:
    Timer()
    {
        start();
    }
    void start()
    {
        m_time = std::chrono::high_resolution_clock::now();
    }
    double elapsed() const
    {   
        std::chrono::duration<double,std::milli> diff =
                          std::chrono::high_resolution_clock::now() - m_time;
        return diff.count();
    }
  private:
    std::chrono::high_resolution_clock::time_point m_time;
};

#endif // USERINTERFACE_INCLUDED
