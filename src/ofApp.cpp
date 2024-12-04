#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    m_timer.setText("generalText.ttf", 50.0f);
    cursor.load("magicWand.png");
    star.load("star.png");
    m_enemies.loadEnemies("green-enemy.png", "blue-enemy.png", "pink-enemy.png");
}

//--------------------------------------------------------------
void ofApp::update(){

    m_timer.update();

    //CODE FOR CURSOR WAND STARS
    //generates the random positions
    float x = ofGetMouseX() + ofRandom(-50, 50);
    float y = ofGetMouseY() + ofRandom(-50, 50);
    //generates the rndm sizes
    float size = ofRandom(10, 50);
    
    //puts the new position and sie of the stars inside the vectors
    starPositions.push_back(ofVec2f(x, y));
    starSizes.push_back(size);
    
    //the 10 determines how many stars you want at a time for more particules do 50 
    if (starPositions.size() > 10) {
        //removes the position and size of oldest star added in the vecor
        //deleting it basically
        starPositions.erase(starPositions.begin());
        starSizes.erase(starSizes.begin());
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofSetColor(255);
    m_timer.drawText();

    //CODE FOR CURSOR WAND STARS
        //sets the max desired size
    star.resize(50, 50);
    
    //draws the starst
    for (int i = 0; i < starPositions.size(); i++) {
    
        float size = starSizes[i];
    
       ofPushMatrix();
    
        ofTranslate(starPositions[i]);
        ofScale(size / 100.0, size / 100.0); 
        star.draw(-star.getWidth() / 2, -star.getHeight() / 2);
    
       ofPopMatrix();
    }
    
    cursor.draw(ofGetMouseX() - 50, ofGetMouseY() - 60);
    cursor.resize(100, 100);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

