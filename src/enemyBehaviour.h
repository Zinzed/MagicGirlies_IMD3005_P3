#pragma once

#include "ofMain.h"

class EnemyBehaviour
{
    private:
        ofImage enemies[3];
        ofImage m_scoreUI;
        int m_num;
        ofTrueTypeFont m_scoreText;

    public:
        bool isDead;
        int m_score;
        void setup();
        void loadEnemies(string enemy1, string enemy2, string enemey3);
        void loadScoreText(string textPath, int size);
        void loadScoreUI(string textPath);
        void drawScore();
        void enemyActions();
        void drawScoreUI();
        void generateNew();
};
