#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxImGui.h"
#include "ImHelpers.h"		//this gives us some extra ofxImGui features (needed for drop-down menus)
#include "timer.h"
#include "enemyBehaviour.h"
#include "CVC.h"
#include "enemyBehaviour.h"
#include "ofEvents.h"


using namespace std;

namespace config {
	static const std::string ARDUINO_DEVICE_NAME = "COM3";		//WIN
};



class ofApp : public ofBaseApp {
public:

	//arduino stuff
	//flex sensors
	static const int PIN_ANALOG_INPUT = 0;
	//static const int PIN_PWM_OUTPUT = 9;
	static const int THRESHOLD = 4;
	//rgb led
	static const int PIN_RED = 6;
	static const int PIN_GREEN = 5;
	static const int PIN_BLUE = 3;
	int PIN_BUTTON = 2;


	Timer m_timer;
	EnemyBehaviour m_enemies;


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

	//struct for tracked color range
	struct ColorRange {
		int minR, maxR;
		int minG, maxG;
		int minB, maxB;
	};

	// Struct to represent a wave with no methods
	struct Wave {
		ofVec2f position; // Position of the wave
		float radius;     // Current radius of the wave
		float opacity;    // Current opacity of the wave
		float maxRadius;  // Maximum radius before the wave is "dead"
		ofColor color;	//color of the wave 
	};

	//define color ranges
	ColorRange pinkRange = { 187, 249, 45, 199, 114, 244 };
	ColorRange greenRange = { 0, 92, 191, 231, 88, 149 };
	ColorRange blueRange = { 37, 119, 54, 166, 196, 247 };

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

	void AutoTrackColor(ofxCvColorImage& image, ColorRange cr1, ColorRange cr2, ColorRange cr3);
	void drawWaves(const Wave& wave);

	bool m_showDebugView;


	vector<ofVec2f> starPositions;
	vector<float> starSizes;


	bool flexMode;


	//arduino stuff

	ofArduino				m_arduino;
	ofTrueTypeFont          m_font;
	int					    flexVal;	//sensor value
	int						mode;		// what colour the led lights are
	bool					m_bSetup;   //is Arduinio initialized yet
	void setupArduino(const int& _version);
	void updateArduino();
	void analogPinChanged(const int& pinNum);
	void digitalPinChanged(const int& pinNum);

	ofImage blueEnemy;


};
