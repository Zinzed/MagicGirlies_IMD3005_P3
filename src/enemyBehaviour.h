#pragma once
#include "ofMain.h"

class EnemyBehaviour
{
    private:
        ofImage enemies[3];
        bool isDead;

    public:
        void loadEnemies(string enemy1, string enemy2, string enemey3);
        void enemyActions();
        int randNum();
};
