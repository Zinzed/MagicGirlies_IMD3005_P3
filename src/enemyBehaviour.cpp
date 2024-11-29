#include "enemyBehaviour.h"

void EnemyBehaviour::loadEnemies(string enemy1, string enemy2, string enemy3)
{
    enemies[0].load(enemy1);
    enemies[1].load(enemy2);
    enemies[2].load(enemy3);
}

void EnemyBehaviour::enemyActions()
{

}