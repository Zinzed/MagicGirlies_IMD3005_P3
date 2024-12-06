#include "enemyBehaviour.h"
#include "CVC.h"

void EnemyBehaviour::loadEnemies(string enemy1, string enemy2, string enemy3)
{
    enemies[0].load(enemy1);
    enemies[1].load(enemy2);
    enemies[2].load(enemy3);
}

void EnemyBehaviour::loadScoreUI(string textPath)
{
    m_scoreUI.load(textPath);
}

void EnemyBehaviour::enemyActions()
{
    int num = randNum();

    if (!isDead)
    {
        if (num == 1)
        {
            enemies[0].draw(100, 100);
        }
        if (num == 2)
        {
            enemies[1].draw(100, 100);
        }
        if (num == 3)
        {
            enemies[2].draw(100, 100);
        }
    }

    //if (enemy is killed)
    //{
    //      isDead == true;
    //      score++;
    //    
    //}
}

void EnemyBehaviour::drawScoreUI()
{
    ofScale(0.8f, 0.8f);
    m_scoreUI.draw(1050.0f + CVC::APP_WINDOW_WIDTH / 2.0f, 150.0f);
}

int EnemyBehaviour::randNum()
{
    int num;
    srand(time(0)); //makes sure that the number is different everytime
    num = (rand() % (4 - 1)) + 1;
    return num;
}
