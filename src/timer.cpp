#include "timer.h"

void Timer::update()
{
    m_time = ofGetElapsedTimeMillis() / 1000.0f;
}

void Timer::setText(string textPath, float size)
{
    m_timerText.load(textPath, size);
}

void Timer::setTimerUI(string textPath)
{
    m_timerUI.load(textPath);
}

void Timer::drawText()
{   
    ofPushMatrix();
    {
        ofScale(0.8f, 0.8f);
        m_timerUI.draw(1000.0f + CVC::APP_WINDOW_WIDTH / 2.0f, 0.0f);

    }ofPopMatrix();
   
    ofSetColor(0);
    m_timerText.drawString(ofToString(m_time), 950.0f + CVC::APP_WINDOW_WIDTH / 2.0f, 100.0f);
}
