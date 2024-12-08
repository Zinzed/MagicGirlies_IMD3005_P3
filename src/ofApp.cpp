#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    //set up app
    ofSetWindowShape(CVC::APP_WINDOW_WIDTH, CVC::APP_WINDOW_HEIGHT);
    ofSetFrameRate(CVC::APP_DESIRED_FRAMERATE);

    //set up cam
    //m_videoGrabber.listDevices();     //to find other cameras attached
    m_videoGrabber.setDeviceID(0);      //assuming first and only camera attached
    m_videoGrabber.setDesiredFrameRate(CVC::CAM_DESIRED_FRAMERATE);
    m_videoGrabber.initGrabber(CVC::VIDEO_WIDTH, CVC::VIDEO_HEIGHT);
    m_camResolutionConflict = (((int)m_videoGrabber.getWidth() != CVC::VIDEO_WIDTH) ||
        ((int)m_videoGrabber.getHeight() != CVC::VIDEO_HEIGHT));

    //set up video
    m_videoPlayer.load(CVC::VIDEO_PATH_COLOR);
    m_videoPlayer.setLoopState(OF_LOOP_NORMAL);
    m_videoPlayer.play();

    //want to check to see if video being loaded in matches the video size consts we wish to process at
    m_videoResolutionConflict = (((int)m_videoPlayer.getWidth() != CVC::VIDEO_WIDTH) ||
        ((int)m_videoPlayer.getHeight() != CVC::VIDEO_HEIGHT));

    //allocate CV images
    m_colorImage.allocate(CVC::VIDEO_WIDTH, CVC::VIDEO_HEIGHT);
    m_grayscaleDiffImage.allocate(CVC::VIDEO_WIDTH, CVC::VIDEO_HEIGHT);

    //init vars
    m_threshold = 100;
    m_numContoursConsidered = 1;
    m_minArea = 10.0f;
    m_maxArea = (float)(CVC::VIDEO_WIDTH * CVC::VIDEO_HEIGHT);
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

    //start screen and instructions screen
    m_showStart = true;
    m_showInstruc = false;
    m_showInstrucP2 = false;
    m_showEnd = false;
    m_backgrounds.setBgs("startScreen.png", "instructionsP1.png", "instructionsP2.png", "timesUp.png");

    //start screen button images
    m_startBtn.load("startBefore.png");
    m_startBtn.resize(750, 310);
    m_instrucBtn.load("instructionsBefore.png");

    //instructions pages button images
    m_backBtn.load("back.png");
    m_backBtn.resize(175, 175); 
    m_nextBtn.load("next.png");
    m_nextBtn.resize(200, 200);
    m_startAltBtn.load("startAlt.png");
    m_startAltBtn.resize(175, 175);

    //button bounding boxes
    m_startBB.set(250, 350, 750, 210);
    m_instrucBB.set(250, 630, m_instrucBtn.getWidth(), m_instrucBtn.getHeight() - 100);
    m_nextBB.set(1600, 400, 200, 200);
    m_backBB.set(775, 775, 175, 175);
    m_startAltBB.set(1000, 775, 175, 175);
    m_restartBB.set(875, 450, 175, 175);

    //music 
    m_loopUI = true;
    m_uiMusic.load("uiMusic.mp3");
    m_uiMusic.setLoop(m_loopUI);
    m_uiMusic.setVolume(0.8);
    m_uiMusic.play();

    m_loopGP = true;
    m_gpMusic.load("gameplayMusic.mp3");
    m_gpMusic.setLoop(m_loopGP);
    m_gpMusic.setVolume(0.6);

    //timer
    m_timer.setText("generalText.ttf", 50.0f);
    m_timer.setTimerUI("timer.png");

    //enemies and score
    m_enemies.setup();
    m_enemies.loadEnemies("green-enemy.png", "blue-enemy.png", "pink-enemy.png");
    m_enemies.loadScoreUI("score.png");
    m_enemyDeath.load("enemyDeath.mp3");
    m_enemies.loadScoreText("generalText.ttf", 50.0f);
    m_enemyRespawn.load("respawn.mp3");

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

    //setup Arduino
    // replace the string below with the serial port for your Arduino board
    // you can get this from the Arduino application (Tools Menu -> Port) 
    //m_arduino.connect("COM4", 57600);
    m_arduino.connect(config::ARDUINO_DEVICE_NAME, 57600);

    flexVal = 0.0f;
    mode = 0.0f;		// what colour the led lights are


    // Listen for EInitialized notification. this indicates that
    // the arduino is ready to receive commands and it is safe to
    // call setupArduino()
    ofAddListener(m_arduino.EInitialized, this, &ofApp::setupArduino);

    m_bSetup = false;
    flexMode = false;

}



//--------------------------------------------------------------
void ofApp::update() {

    updateArduino();

    

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

            // processColor(m_colorImage);
            AutoTrackColor(m_colorImage, pinkRange, greenRange, blueRange);
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

            //processColor(m_colorImage);
            AutoTrackColor(m_colorImage, pinkRange, greenRange, blueRange);
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
            //ofVec2f wavePosition = ofVec2f(m_contourFinder.blobs[0].centroid.x,
              //  m_contourFinder.blobs[0].centroid.y);
            Wave newWave;
            newWave.position.set(CVC::APP_WINDOW_WIDTH / 2, CVC::APP_WINDOW_HEIGHT / 2);
            newWave.radius = 0;
            newWave.opacity = 255;
            newWave.maxRadius = 300;

            waves.push_back(newWave);
        }

        m_previousContourArea = currentContourArea; // Update the previous area
    }



    // Update waves
    for (int i = waves.size() - 1; i >= 0; --i) {
        waves[i].radius += 5;   //expand radius
        waves[i].opacity -= 5;  //fade opacity
        if (waves[i].opacity < 0) waves[i].opacity = 0;

        //remove "dead" waves
        if (waves[i].radius > waves[i].maxRadius || waves[i].opacity <= 0) {
            waves.erase(waves.begin() + i);
        }
    }

    // update the timer if in gameplay
    if (!m_showStart && !m_showInstruc && !m_showInstrucP2)
    {
        if (!m_timer.timerIsRunning)
        {
            m_timer.startTime();
        }
        m_timer.update();
        //end game if timer is greater than 1 minute
        if (m_timer.m_time >= 60)
        {
            m_showEnd = true;
            m_showStart = false;
            m_showInstruc = false;
            m_showInstrucP2 = false;
        }
    }
    else
    {
        m_timer.timerIsRunning = false;
    }

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

    ofLogNotice() << "r: " << static_cast<int>(std::round(m_trackedColor[2] * 255));
    ofLogNotice() << "g: " << static_cast<int>(std::round(m_trackedColor[2] * 255));
    ofLogNotice() << "b: " << static_cast<int>(std::round(m_trackedColor[2] * 255));

    //gameplay music
    if (m_playGPMus && !m_gpMusic.isPlaying())
    {
        m_gpMusic.play();
    }
}


//--------------------------------------------------------------
void ofApp::draw() {

    if (m_showStart && !m_showInstruc && !m_showInstrucP2 && !m_showEnd)
    {
        ofSetColor(255);
        m_backgrounds.drawStart();
        m_startBtn.draw(250, 350);
        m_instrucBtn.draw(270, 600);
    }
    else if (m_showInstruc && !m_showStart && !m_showInstrucP2 && !m_showEnd)
    {   
        ofSetColor(255);
        m_backgrounds.drawInstrucP1();
        m_nextBtn.draw(1600, 400);
    }
    if (!m_showStart && !m_showInstruc && m_showInstrucP2 && !m_showEnd)
    {
        m_backgrounds.drawInstrucP2();
        m_backBtn.draw(775, 775);
        m_startAltBtn.draw(1000, 775);

    }

    if (!m_showStart && !m_showInstruc && !m_showInstrucP2 && !m_showEnd)
    {
        m_loopUI = false;
        m_uiMusic.stop();

        ofPushMatrix();
        {
            ofSetColor(255);
            // ofTransform();
            ofScale(2, 2);
            m_skyBg.draw(0, 0);
        }ofPopMatrix();

        //draw score
        ofPushMatrix();
        {
            m_enemies.drawScoreUI();

        }ofPopMatrix();

        ofPushMatrix();
        {
            m_enemies.drawScore();

        }ofPopMatrix();

        // draw timer
        ofPushMatrix();
        {
            ofSetColor(255);
            m_timer.drawText();

        }ofPopMatrix();

        ofPushMatrix();
        {
            ofSetColor(255);
            m_enemies.enemyActions();

        }ofPopMatrix();

        if (m_showDebugView) {
    
            //draw videos
            ofSetColor(255, 255, 255);
            m_grayscaleDiffImage.draw(CVC::VIDEO_BORDER_SIZE, CVC::VIDEO_BORDER_SIZE);
            m_colorImage.draw(CVC::VIDEO_WIDTH + CVC::VIDEO_BORDER_SIZE * 2.0f, CVC::VIDEO_BORDER_SIZE);
    
    
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

    if (!m_showStart && !m_showInstruc && !m_showInstrucP2 && m_showEnd)
    {
        m_gpMusic.stop();

        m_backgrounds.drawEnd();

        ofPushMatrix();
        {
            m_enemies.drawScoreUI();

        }ofPopMatrix();

        ofPushMatrix();
        {
            m_enemies.drawScore();

        }ofPopMatrix();

    }
    
        //CODE FOR CURSOR WAND STARS
            //sets the max desired size
        star.resize(50, 50);
    
        //draws the stars
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
    
        //draw the waves
    
        ofPushMatrix();
    
        for (const Wave& wave : waves) {
    
            drawWaves(wave);
        }
    
        ofPopMatrix();
    
        //enemy showing with flex sensor
        // Check if the flex sensor value exceeds the threshold
        if (flexVal > 2) {
            flexMode = true;
        }
        else if (flexVal == 0) {
            flexMode = false;
        }
    
        if (flexMode == true) {
            m_enemies.enemyActions();
        }
    
        ofSetColor(255);
        ofDrawBitmapString("Flex Value: " + ofToString(flexVal), 20, 20);
    
        }
        
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
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

    //this will be the wand logic
    if (key == 'k')
    {
        m_enemyDeath.play();
        m_enemies.isDead = true;
        m_enemies.m_score++;
    }
    //this will be the flex sensor logic
    if (key == 'f')
    {
        m_enemyRespawn.play();
        m_enemies.generateNew();
        m_enemies.isDead = false;
    }
}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    //let's do this in a bit 
    if (button == OF_MOUSE_BUTTON_RIGHT) {
        //select the colour from the image
        //we CAN't SELECT color from outside the image (may get an indes out of array error)
        ofRectangle videoRect = ofRectangle(CVC::VIDEO_WIDTH + CVC::VIDEO_BORDER_SIZE * 2,
            CVC::VIDEO_BORDER_SIZE,
            CVC::VIDEO_WIDTH,
            CVC::VIDEO_HEIGHT);

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

void ofApp::AutoTrackColor(ofxCvColorImage& image, ColorRange cr1, ColorRange cr2, ColorRange cr3) {
    const int numChannelsPerPixel = image.getPixels().getNumChannels(); // should be 3 for red, green, blue channels
    const int numChannels = CVC::VIDEO_WIDTH * CVC::VIDEO_HEIGHT * numChannelsPerPixel;
    float pixel[3] = { 0.0f, 0.0f, 0.0f };

    ofPixels& pixels = image.getPixels();
    ofImage binaryMask;
    binaryMask.allocate(image.getWidth(), image.getHeight(), OF_IMAGE_GRAYSCALE);

    // loop through the image pixels
    for (int i = 0; i < numChannels; i += numChannelsPerPixel) {
        pixel[0] = pixels[i + 0];        // red
        pixel[1] = pixels[i + 1];        // green
        pixel[2] = pixels[i + 2];        // blue

        //check if the pixel matches any of the defined color ranges
        bool matchesPink = pixel[0] >= cr1.minR && pixel[0] <= cr1.maxR &&
            pixel[1] >= cr1.minG && pixel[1] <= cr1.maxG &&
            pixel[2] >= cr1.minB && pixel[2] <= cr1.maxB;

        bool matchesGreen = pixel[0] >= cr2.minR && pixel[0] <= cr2.maxR &&
            pixel[1] >= cr2.minG && pixel[1] <= cr2.maxG &&
            pixel[2] >= cr2.minB && pixel[2] <= cr2.maxB;

        bool matchesBlue = pixel[0] >= cr3.minR && pixel[0] <= cr3.maxR &&
            pixel[1] >= cr3.minG && pixel[1] <= cr3.maxG &&
            pixel[2] >= cr3.minB && pixel[2] <= cr3.maxB;

        //if the pixel matches any of the color ranges, set corresponding grayscale value to white
        if (matchesPink || matchesGreen || matchesBlue) {
            m_grayscaleDiffImage.getPixels()[i / numChannelsPerPixel] = 255;
            //if (matchesPink) {
            //    m_trackedColor[0] = pixel[0] / 255.0f;  // red
            //    m_trackedColor[1] = pixel[1] / 255.0f;  // green
            //    m_trackedColor[2] = pixel[2] / 255.0f;  // blue
            //}
            //else if (matchesGreen) {
            //    m_trackedColor[0] = pixel[0] / 255.0f;  // red
            //    m_trackedColor[1] = pixel[1] / 255.0f;  // green
            //    m_trackedColor[2] = pixel[2] / 255.0f;  // blue
            //}
            //else if (matchesBlue) {
            //    m_trackedColor[0] = pixel[0] / 255.0f;  // red
            //    m_trackedColor[1] = pixel[1] / 255.0f;  // green
            //    m_trackedColor[2] = pixel[2] / 255.0f;  // blue
            //}
        }
        else {
            m_grayscaleDiffImage.getPixels()[i / numChannelsPerPixel] = 0;
        }
    }

    // update the grayscale image
    m_grayscaleDiffImage.flagImageChanged(); // marks the image as changed for further processing

    //find contours/blobs
    m_contourFinder.findContours(m_grayscaleDiffImage, m_minArea, m_maxArea, m_numContoursConsidered, false, true);

    //button logic
    if (m_startBB.inside(x, y) || m_startAltBB.inside(x, y))
    {
        m_buttonClick.play();
        m_showStart = false;
        m_showInstruc = false;
        m_showInstrucP2 = false;
        m_playGPMus = true;
        m_showEnd = false;
    }
    else if (m_instrucBB.inside(x, y) || m_backBB.inside(x, y))
    {
        m_buttonClick.play();
        m_showInstruc = true;
        m_showStart = false;
        m_showInstrucP2 = false;
        m_showEnd = false;
    }
    else if (m_nextBB.inside(x, y))
    {
        m_buttonClick.play();
        m_showInstruc = false;
        m_showStart = false;
        m_showInstrucP2 = true;
        m_showEnd = false;
    }
    else if (m_restartBB.inside(x, y))
    {
        m_buttonClick.play();
        m_showInstruc = false;
        m_showStart = true;
        m_showInstrucP2 = false;
        m_showEnd = false;
    }
}

void ofApp::drawWaves(const Wave& wave) {


    ofNoFill();
    ofSetLineWidth(5);

    int red = static_cast<int>(std::round(m_trackedColor[0] * 255));
    int green = static_cast<int>(std::round(m_trackedColor[1] * 255));
    int blue = static_cast<int>(std::round(m_trackedColor[2] * 255));

    //check ranges for pink, green, and blue
    //if (red >= 187 && red <= 249 &&
    //    green >= 45 && green <= 199 &&
    //    blue >= 114 && blue <= 244) {
    //    ofSetColor(255, 105, 180, wave.opacity); // pink
    //}
    //else if (red >= 0 && red <= 92 &&
    //    green >= 191 && green <= 231 &&
    //    blue >= 88 && blue <= 149) {
    //    ofSetColor(0, 255, 0, wave.opacity); // green
    //}
    //else if (red >= 37 && red <= 119 &&
    //    green >= 54 && green <= 166 &&
    //    blue >= 196 && blue <= 247) {
    //    ofSetColor(0, 0, 255, wave.opacity); //blue
    //}
    //else {
    ofSetColor(255, 255, 255, wave.opacity); //default to white
    //}


    //draw the wave circle
   // ofDrawCircle(wave.position, wave.radius);
    //draw multiple rings around the wave's position
    int numRings = 5; // number of rings
    float spacing = 50; //space between rings
    for (int i = 0; i < numRings; ++i) {
        float ringRadius = wave.radius + (i * spacing);
        if (ringRadius <= wave.maxRadius) {
            ofDrawCircle(wave.position, ringRadius);
        }

    }
}


//--------------------------------------------------------------
void ofApp::setupArduino(const int& _version)
{

    m_bSetup = true;

    // remove listener because we don't need it anymore
    ofRemoveListener(m_arduino.EInitialized, this, &ofApp::setupArduino);

    // print firmware name and version to the console
    ofLogNotice() << m_arduino.getFirmwareName();
    ofLogNotice() << "firmata v" << m_arduino.getMajorFirmwareVersion() << "." << m_arduino.getMinorFirmwareVersion();

    //analog input
    m_arduino.sendAnalogPinReporting(PIN_ANALOG_INPUT, ARD_ANALOG);

    //digital input
    m_arduino.sendDigitalPinMode(PIN_BUTTON, ARD_INPUT);

    // Set pin modes digital ouput
    m_arduino.sendDigitalPinMode(PIN_RED, ARD_PWM); // Red
    m_arduino.sendDigitalPinMode(PIN_GREEN, ARD_PWM); // Green
    m_arduino.sendDigitalPinMode(PIN_BLUE, ARD_PWM); // Blue

    m_arduino.sendPwm(PIN_RED, 0);
    m_arduino.sendPwm(PIN_GREEN, 0);
    m_arduino.sendPwm(PIN_BLUE, 0);

   // ofAddListener(m_arduino.EDigitalPinChanged, this, &ofApp::digitalPinChanged);

    //PMW/digital output
    

    ofAddListener(m_arduino.EDigitalPinChanged, this, &ofApp::digitalPinChanged);
    ofAddListener(m_arduino.EAnalogPinChanged, this, &ofApp::analogPinChanged);

    //initialization
   
}


void ofApp::updateArduino() {

    m_arduino.update();

    
}

void ofApp::analogPinChanged(const int& pinNum) {
    cout << "analog pin: " + ofToString(pinNum) + " : " + ofToString(m_arduino.getAnalog(pinNum)) << std::endl;
    if (pinNum == PIN_ANALOG_INPUT) {

        //get analog value
        flexVal = m_arduino.getAnalog(pinNum);
        flexVal = (int)ofMap(flexVal, 0, 1023, 0, 255);


    }
}

void ofApp::digitalPinChanged(const int& pinNum) {
     
    cout << "digital pin: " + ofToString(pinNum) + " : " + ofToString(m_arduino.getDigital(pinNum)) << std::endl;
   // mode = m_arduino.getDigital(pinNum);

    switch (mode) {
        //off
    case 0:
        m_arduino.sendPwm(PIN_RED, 0); //red
        m_arduino.sendPwm(PIN_GREEN, 0); //green
        m_arduino.sendPwm(PIN_BLUE, 0); //blue
        break;
        //green
    case 1:
        m_arduino.sendPwm(PIN_RED, 0);
        m_arduino.sendPwm(PIN_GREEN, 255);
        m_arduino.sendPwm(PIN_BLUE, 0);
        break;
        // blue
    case 2:
        m_arduino.sendPwm(PIN_RED, 0);
        m_arduino.sendPwm(PIN_GREEN, 0);
        m_arduino.sendPwm(PIN_BLUE, 255);
        break;
        //pink
    case 3:
        m_arduino.sendPwm(PIN_RED, 255);
        m_arduino.sendPwm(PIN_GREEN, 20);
        m_arduino.sendPwm(PIN_BLUE, 50);
        break;
    default:
        m_arduino.sendPwm(PIN_RED, 0);
        m_arduino.sendPwm(PIN_GREEN, 0);
        m_arduino.sendPwm(PIN_BLUE, 0);
        break;
    }
   


    if (pinNum == PIN_BUTTON ) {//&& m_arduino.getDigital(pinNum) == ARD_LOW) {
        mode++;
        if (mode > 3) mode = 0;

    }
}

