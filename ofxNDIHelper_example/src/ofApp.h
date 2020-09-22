#pragma once

#include "ofMain.h"

#define USE_ofxNDI
#ifdef USE_ofxNDI
#include "ofxNDIHelper.h"
#endif

//uncomment if you added this addon to handle window settings
#define USE_ofxWindowApp
#ifdef USE_ofxWindowApp
#include "ofxWindowApp.h"
#endif

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();
	void exit();
	void keyPressed(int key);
	void windowResized(int w, int h);

public:
#ifdef USE_ofxNDI
	ofxNDIHelper NDIHelper;
#endif

	//window
#ifdef USE_ofxWindowApp
	ofxWindowApp windowApp;
#endif

	ofImage image;
};
