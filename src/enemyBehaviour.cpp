#include "enemyBehaviour.h"

void EnemyBehaviour::loadEnemies(string enemy1, string enemy2, string enemy3)
{
    enemies[0].load(enemy1);
    enemies[1].load(enemy2);
    enemies[2].load(enemy3);
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

int EnemyBehaviour::randNum()
{
    int num;
    srand(time(0)); //makes sure that the number is different everytime
    num = (rand() % (3 - 1)) + 1;
    return num;
}
