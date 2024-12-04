#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxImGui.h"
#include "ImHelpers.h"		//this gives us some extra ofxImGui features (needed for drop-down menus)
#include "timer.h"
#include "enemyBehaviour.h"
#include "CVC.h"

using namespace std;




class ofApp : public ofBaseApp{
	public:
		
		Timer m_timer;


		ofVideoPlayer m_videoPlayer;
		ofVideoGrabber m_videoGrabber;
		bool m_videoResolutionConflict;		//do we need to re-size our video before we process?
		bool m_camResolutionConflict;		//do we need to re-size our video before we process?
		bool m_camPaused;					//pausing our camera feed
		
		ofxCvContourFinder m_contourFinder;

		//CV images
		ofxCvColorImage		m_colorImage;
		ofxCvGrayscaleImage	m_grayscaleDiffImage;

		//GUI
		ofxImGui::Gui		m_gui;
		int					m_threshold;	//how closely does the colour have to match to track?
		int					m_numContoursConsidered;
		float				m_minArea;
		float				m_maxArea;

		//we want to select which color we are tracking within the video
		float m_trackedColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };	//ofxImgui requires color in this form

		//app modes
		CVC::APP_MODE m_appMode;				//current app state
		std::vector<std::string> m_appModes;	//"list"/dynamic array of app states

		bool m_isSwinging; //to detect swinging motion 
		float m_previousContourArea; //previous contour area
		float m_farThreshold; //"far" area
		float m_closeThreshold; //"close" area

		ofSoundPlayer m_magicSound;

		ofImage cursor;
		ofImage star;
		ofImage m_skyBg;

		// Struct to represent a wave with no methods
		struct Wave {
			ofVec2f position; // Position of the wave
			float radius;     // Current radius of the wave
			float opacity;    // Current opacity of the wave
			float maxRadius;  // Maximum radius before the wave is "dead"
		};

		// Wave management
		vector<Wave> waves;
		float waveInterval;   // Time interval between waves
		float lastWaveTime;   // Timestamp of the last wave
		bool stopWaves;       // Flag to stop adding waves
		int waveCount;        // Current number of waves created
		int maxWaves;         // Maximum number of waves allowed

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void mousePressed(int x, int y, int button);

		//we will do all color CV processing here
		//we are passing a reference here ...
		void processColor(ofxCvColorImage& image);

		bool m_showDebugView;

		
		vector<ofVec2f> starPositions;
		vector<float> starSizes;
};
