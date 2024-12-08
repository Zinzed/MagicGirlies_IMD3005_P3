#pragma once

#include "ofMain.h"
#include "CVC.h"

class Timer
{
private:

    float m_startTime;

    ofTrueTypeFont m_timerText;

    ofImage m_timerUI;

public:
    float m_time;
    bool timerIsRunning;
    bool timeUp; 
    void setText(string textPath, float size);
    void setTimerUI(string textPath);
    void drawText();
    void startTime();
    void update();
};
