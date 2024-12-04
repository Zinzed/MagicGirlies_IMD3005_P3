#pragma once

#include "ofMain.h"
#include "timer.h"
#include "enemyBehaviour.h"

using namespace std;

class ofApp : public ofBaseApp{

	public:

		Timer m_timer;
		EnemyBehaviour m_enemies;

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		vector<ofVec2f> starPositions;
		vector<float> starSizes;
		
};
