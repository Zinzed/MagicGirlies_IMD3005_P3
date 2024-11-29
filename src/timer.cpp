#include "timer.h"

void Timer::update()
{
    m_time = ofGetElapsedTimeMillis() / 1000.0f;
}

void Timer::setText(string textPath, float size)
{
    m_timerText.load(textPath, size);
}

void Timer::drawText()
{
    m_timerText.drawString("time: " + ofToString(m_time), 9 * ofGetWindowWidth() / 10, 80.0f);
}
