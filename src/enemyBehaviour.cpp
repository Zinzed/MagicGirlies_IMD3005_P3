#include "enemyBehaviour.h"
#include "CVC.h"

void EnemyBehaviour::setup()
{
    isDead = false;

    m_score = 0;

    srand(time(0));
    m_num = (rand() % (4 - 1)) + 1;
}

void EnemyBehaviour::loadEnemies(string enemy1, string enemy2, string enemy3)
{
    enemies[0].load(enemy1);
    enemies[1].load(enemy2);
    enemies[2].load(enemy3);
}

void EnemyBehaviour::loadScoreText(string textPath, int size)
{
    m_scoreText.load(textPath, size);
}

void EnemyBehaviour::loadScoreUI(string textPath)
{
    m_scoreUI.load(textPath);
}

void EnemyBehaviour::drawScore()
{
    ofSetColor(0);
    m_scoreText.drawString(ofToString(m_score), 1010.0f + CVC::APP_WINDOW_WIDTH / 2.0f, 210.0f);
}

void EnemyBehaviour::enemyActions()
{
    if (!isDead)
    {
        if (m_num == 1)
        {
            enemies[0].draw(CVC::APP_WINDOW_WIDTH / 2, CVC::APP_WINDOW_HEIGHT / 4);
        }
        if (m_num == 2)
        {
            enemies[1].draw(CVC::APP_WINDOW_WIDTH / 2, CVC::APP_WINDOW_HEIGHT / 4);
        }
        if (m_num == 3)
        {
            enemies[2].draw(CVC::APP_WINDOW_WIDTH / 2, CVC::APP_WINDOW_HEIGHT / 4);
        }
    }

}

void EnemyBehaviour::drawScoreUI()
{
    ofScale(0.8f, 0.8f);
    m_scoreUI.draw(1050.0f + CVC::APP_WINDOW_WIDTH / 2.0f, 150.0f);
}

void EnemyBehaviour::generateNew()
{
    m_num = (rand() % (4 - 1)) + 1;
}
