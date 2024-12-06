#pragma once

#include "ofMain.h"

class EnemyBehaviour
{
    private:
        ofImage enemies[3];
        bool isDead;
        int m_score;
        ofImage m_scoreUI;

    public:
        void loadEnemies(string enemy1, string enemy2, string enemey3);
        void loadScoreUI(string textPath);
        void enemyActions();
        void drawScoreUI();
        int randNum();
};
