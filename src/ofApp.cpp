#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    //set up app
    ofSetWindowShape(CVC::APP_WINDOW_WIDTH, CVC::APP_WINDOW_HEIGHT);
    ofSetFrameRate(CVC::APP_DESIRED_FRAMERATE);

    //set up cam
    //m_videoGrabber.listDevices();     //to find other cameras attached
    m_videoGrabber.setDeviceID(0);      //assuming first and only camera attached
    m_videoGrabber.setDesiredFrameRate(CVC::CAM_DESIRED_FRAMERATE);
    m_videoGrabber.initGrabber(CVC::VIDEO_WIDTH, CVC::VIDEO_HEIGHT);
    m_camResolutionConflict = ( ((int)m_videoGrabber.getWidth() != CVC::VIDEO_WIDTH) ||
                                ((int)m_videoGrabber.getHeight() != CVC::VIDEO_HEIGHT));

    //set up video
    m_videoPlayer.load(CVC::VIDEO_PATH_COLOR);
    m_videoPlayer.setLoopState(OF_LOOP_NORMAL);
    m_videoPlayer.play();

    //want to check to see if video being loaded in matches the video size consts we wish to process at
    m_videoResolutionConflict = (   ((int)m_videoPlayer.getWidth() != CVC::VIDEO_WIDTH) || 
                                    ((int)m_videoPlayer.getHeight() != CVC::VIDEO_HEIGHT) );

    //allocate CV images
    m_colorImage.allocate(CVC::VIDEO_WIDTH, CVC::VIDEO_HEIGHT);
    m_grayscaleDiffImage.allocate(CVC::VIDEO_WIDTH, CVC::VIDEO_HEIGHT);

    //init vars
    m_threshold             = 128;
    m_numContoursConsidered = 5;
    m_minArea               = 10.0f;
    m_maxArea               = (float)(CVC::VIDEO_WIDTH * CVC::VIDEO_HEIGHT);
    m_isSwinging = false;
    m_previousContourArea = 0.0f; //previous contour area
    m_farThreshold = 180.0f; //"far" area
    m_closeThreshold = 600.0f; //"close" area

    //sfx
    m_magicSound.load("magic-spell.mp3");

    //wave effect
    waveInterval = 500;  // Time between waves in milliseconds
    lastWaveTime = 0;
    stopWaves = false;
    waveCount = 0;
    maxWaves = 10;

    //timer
    m_timer.setText("generalText.ttf", 50.0f);
    m_timer.setTimerUI("timer.png");

    //enemies
    m_enemies.loadEnemies("green-enemy.png", "blue-enemy.png", "pink-enemy.png");

    //debug view
    m_showDebugView = false;

    //magic wand cursor 
    cursor.load("magicWand.png");
    star.load("star.png");
    m_skyBg.load("sky-bg.png");


    //gui
    m_gui.setup();

    //set up app states
    m_appMode = CVC::APP_MODE::APP_VIDEO;
    m_appModes.push_back("APP_VIDEO");
    m_appModes.push_back("APP_CAM");
}

//--------------------------------------------------------------
void ofApp::update(){
    
    switch (m_appMode) {
        case CVC::APP_MODE::APP_VIDEO: {
            m_videoPlayer.update();

            //only process if new frame
            if (m_videoPlayer.isFrameNew() == true) {
                m_colorImage.setFromPixels(m_videoPlayer.getPixels());

                //resize, if res conflict
                if (m_videoResolutionConflict == true) {
                    m_colorImage.resize(CVC::VIDEO_WIDTH, CVC::VIDEO_HEIGHT);
                }

                processColor(m_colorImage);
            }
        }
            break;
        case CVC::APP_MODE::APP_CAM: {
            if (m_camPaused == false) {
                m_videoGrabber.update();

                if (m_videoGrabber.isFrameNew() == true) {
                    m_colorImage.setFromPixels(m_videoGrabber.getPixels());
                }

                //resize, if res conflict
                if (m_camResolutionConflict == true) {
                    m_colorImage.resize(CVC::VIDEO_WIDTH, CVC::VIDEO_HEIGHT);
                }

                processColor(m_colorImage);
            }
            
        }
            break;
    }

    // Swing detection logic
    if (m_contourFinder.nBlobs > 0) {
        float currentContourArea = m_contourFinder.blobs[0].area; // Use the first blob for simplicity

        if (!m_isSwinging && currentContourArea < m_farThreshold) {
            m_isSwinging = true; // Start the swing
        }
        else if (m_isSwinging && currentContourArea > m_closeThreshold) {
            m_isSwinging = false; // End the swing
            // Trigger the swing event here
            //ofDrawBitmapString("SWING DETECTED", ofGetWindowWidth()/2, ofGetWindowHeight() / 2);
          //  ofLogNotice() << "Swing detected! Playing sound.";
            m_magicSound.play();

            // Create a new wave at the blob's position
            ofVec2f wavePosition = ofVec2f(m_contourFinder.blobs[0].centroid.x,
                m_contourFinder.blobs[0].centroid.y);
            Wave newWave;
            newWave.position = wavePosition;
            newWave.radius = 0;
            newWave.opacity = 255;
            newWave.maxRadius = 300;

            waves.push_back(newWave);
        }

        m_previousContourArea = currentContourArea; // Update the previous area
    }

   

     // Update waves
    for (int i = waves.size() - 1; i >= 0; --i) {
        waves[i].radius += 5;   // Expand radius
        waves[i].opacity -= 5;  // Fade opacity
        if (waves[i].opacity < 0) waves[i].opacity = 0;

        // Remove "dead" waves
        if (waves[i].radius > waves[i].maxRadius || waves[i].opacity <= 0) {
            waves.erase(waves.begin() + i);
        }
    }

    // update the timer
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

    ofPushMatrix();
    {
        ofSetColor(255);
        // ofTransform();
        ofScale(2, 2);
        m_skyBg.draw(0, 0);
        
    }ofPopMatrix();

    // draw timer
    m_timer.drawText();
   

    if (m_showDebugView) {

        //draw videos
        ofSetColor(255, 255, 255);
        m_grayscaleDiffImage.draw(CVC::VIDEO_BORDER_SIZE, CVC::VIDEO_BORDER_SIZE);
        m_colorImage.draw(CVC::VIDEO_WIDTH + CVC::VIDEO_BORDER_SIZE * 2.0f, CVC::VIDEO_BORDER_SIZE);
        //m_magicSparkles.draw(CVC::VIDEO_WIDTH + CVC::VIDEO_BORDER_SIZE * 2.0f, CVC::VIDEO_BORDER_SIZE);


        //draw contours
        static ofVec2f contourCenter;
        static float contourArea;

        ofPushMatrix();
        {
            ofTranslate(CVC::VIDEO_BORDER_SIZE, CVC::VIDEO_BORDER_SIZE);

            //loop through blobs and draw them with some debug info.
            for (int i = 0; i < m_contourFinder.nBlobs; i++) {
                contourCenter.set(m_contourFinder.blobs[i].boundingRect.getCenter().x,
                    m_contourFinder.blobs[i].boundingRect.getCenter().y);
                contourArea = m_contourFinder.blobs[i].area;

                //draw contour
                m_contourFinder.blobs[i].draw();

                //draw center
                ofSetColor(ofColor::coral);
                ofDrawCircle(contourCenter.x, contourCenter.y, 5.0f);

                //draw text info.
                ofDrawBitmapString("Centre: " + ofToString(contourCenter.x) + "," + ofToString(contourCenter.y),
                    m_contourFinder.blobs[i].boundingRect.getMaxX() + CVC::VIDEO_BORDER_SIZE,
                    contourCenter.y);
                ofDrawBitmapString("Area: " + ofToString(contourArea),
                    m_contourFinder.blobs[i].boundingRect.getMaxX() + CVC::VIDEO_BORDER_SIZE,
                    contourCenter.y + 20.0f);
            }
        }
        ofPopMatrix();

        //draw gui
        m_gui.begin();
        {
            ImGui::Text("OpenCV Color Lesson");
            ImGui::SliderInt("Threshold", &m_threshold, 0, 255);
            ImGui::SliderInt("Num of Contours", &m_numContoursConsidered, 0, 30);
            ImGui::SliderFloat("Min. Area", &m_minArea, 0.0f, (float)(CVC::VIDEO_WIDTH * CVC::VIDEO_HEIGHT));
            ImGui::SliderFloat("Max. Area", &m_maxArea, 0.0f, (float)(CVC::VIDEO_WIDTH * CVC::VIDEO_HEIGHT));

            ImGui::Separator(); //you've got to keep them separated

            ImGui::Text("\n Please select app state, thank you! Or not ... in that case, no thank you!");
            static int currentListBoxIndex = 0;
            if (ofxImGui::VectorCombo("App Mode", &currentListBoxIndex, m_appModes)) {
                m_appMode = (CVC::APP_MODE)currentListBoxIndex;
            }
            ImGui::ColorEdit3("Selected Color", (float*)m_trackedColor);
            ImGui::Text("\n Instructions: \npress spacebar to toggle pause \nright-click on video to select colour");
        }
        m_gui.end();
    }

    //CODE FOR CURSOR WAND STARS
        //sets the max desired size
    star.resize(50, 50);

    //draws the starst
    for (int i = 0; i < starPositions.size(); i++) {

        float size = starSizes[i];

        ofPushMatrix();
        ofSetColor(255);
        ofTranslate(starPositions[i]);
        ofScale(size / 100.0, size / 100.0);
        star.draw(-star.getWidth() / 2, -star.getHeight() / 2);

        ofPopMatrix();
    }

    cursor.draw(ofGetMouseX() - 50, ofGetMouseY() - 60);
    cursor.resize(100, 100);

    // Draw waves
    ofPushMatrix();

    for (const Wave& wave : waves) {

        //ofSetColor(255, 255, 255, wave.opacity);  // White with fading opacity
        //ofSetColor(0, 255, 0, wave.opacity); //green 
       // ofSetColor(0, 0, 255, wave.opacity); //blue
        ofSetColor(255, 105, 180, wave.opacity); //pink
        ofNoFill();
        ofSetLineWidth(5);

        // Draw multiple rings around the wave's position
        int numRings = 5; // Number of rings
        float spacing = 50; // Space between rings
        for (int i = 0; i < numRings; ++i) {
            float ringRadius = wave.radius + (i * spacing);
            if (ringRadius <= wave.maxRadius) {
                ofDrawCircle(wave.position, ringRadius);
            }
        } 
    }

    ofPopMatrix();


    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    //each key on a keyboard ha sa keycode associated with it
    //32 = spacebar
    if (key == 32) {
        switch (m_appMode) {
        case CVC::APP_MODE::APP_VIDEO: {
            //set the opposite of what pause state it alrready is
            m_videoPlayer.setPaused(!m_videoPlayer.isPaused());
        }
            break;
        case CVC::APP_MODE::APP_CAM: {
            m_camPaused = !m_camPaused;
        }
           break;
        }
    }

    if (key == 's')
    {
        ofLogNotice() << "Swing detected! Playing sound.";
        m_magicSound.play();
    }
    if (key == 'd') {
        m_showDebugView = !m_showDebugView; //toggle the debug visibility
    }
}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    //let's do this in a bit 
    if (button == OF_MOUSE_BUTTON_RIGHT) {
        //select the colour from the image
        //we CAN't SELECT color from outside the image (may get an indes out of array error)
        ofRectangle videoRect = ofRectangle( CVC::VIDEO_WIDTH + CVC::VIDEO_BORDER_SIZE * 2,
                                             CVC::VIDEO_BORDER_SIZE,
                                             CVC::VIDEO_WIDTH,
                                             CVC::VIDEO_HEIGHT );

        //step 1: clamping screenspace to image space
        int convertX = ofClamp(x, videoRect.getMinX(), videoRect.getMaxX());
        int convertY = ofClamp(y, videoRect.getMinY(), videoRect.getMaxY());

        //step2: mapping screenspace to image space (by shift coordinate origin
        convertX -= videoRect.getMinX();
        convertY -= videoRect.getMinY();

        //get the color using x + y * width function ...
        const int index = (convertX + convertY * m_colorImage.getWidth()) * m_colorImage.getPixels().getNumChannels();
        m_trackedColor[0] = m_colorImage.getPixels()[index + 0] / 255.0f;       //red, divide by 255.0f as m_trackedColor is a float color [0.0f, 1.0f]
        m_trackedColor[1] = m_colorImage.getPixels()[index + 1] / 255.0f;       //green
        m_trackedColor[2] = m_colorImage.getPixels()[index + 2] / 255.0f;       //blue
    }
}

void ofApp::processColor(ofxCvColorImage& image) {
    const int numChannelsPerPixel   = image.getPixels().getNumChannels(); //shouild be 3 for red, green, blue channels
    const int numChannels           = CVC::VIDEO_WIDTH * CVC::VIDEO_HEIGHT * numChannelsPerPixel;
    float pixel[3]                  = { 0.0f, 0.0f, 0.0f };

    //loop through color image pixels
    //check if pixel color matches our tracked color
    //if true, set our diff image pixel color to white
    //if false, set our diff image pixel color to black
    for (int i = 0; i < numChannels; i+=numChannelsPerPixel) {
        pixel[0] = image.getPixels()[i + 0];        //red
        pixel[1] = image.getPixels()[i + 1];        //green
        pixel[2] = image.getPixels()[i + 2];        //blue

        //check if tracked color matches pixel color
        //m_trackedColor is a float color [0.0, 1.0] so we need to convert back to a byte color [0,255]
        if (    (abs(pixel[0] - m_trackedColor[0] * 255.0f) < m_threshold) &&
                (abs(pixel[1] - m_trackedColor[1] * 255.0f) < m_threshold) &&
                (abs(pixel[2] - m_trackedColor[2] * 255.0f) < m_threshold)) {
            
            //COLOR THE SAME _ YAAYYYYYYY!!
            m_grayscaleDiffImage.getPixels()[i / numChannelsPerPixel] = 255;
        }
        else {
            //NOT THE SAME - BOOO
            m_grayscaleDiffImage.getPixels()[i / numChannelsPerPixel] = 0;
        }
    }

    //update cv image
    m_grayscaleDiffImage.flagImageChanged();        //tells the compy that the image has changed on the CPU RAM and should be moved to GPU RAM
    
    //find contours/blobs
    m_contourFinder.findContours(m_grayscaleDiffImage, m_minArea, m_maxArea, m_numContoursConsidered, false, true);
}
