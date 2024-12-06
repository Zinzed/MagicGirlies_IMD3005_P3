#pragma once

#include "ofMain.h"
#include "CVC.h"

class Timer
{
private:
    float m_time;

    ofTrueTypeFont m_timerText;

    int m_minutes;

    ofImage m_timerUI;

public:
    void setText(string textPath, float size);
    void setTimerUI(string textPath);
    void drawText();
    void update();
};
