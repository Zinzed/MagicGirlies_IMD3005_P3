#pragma once

#include "ofMain.h"
#include "CVC.h"

class Timer
{
private:
    float m_time;

    ofTrueTypeFont m_timerText;

    int m_minutes;

public:
    void setText(string textPath, float size);
    void drawText();
    void update();
};
