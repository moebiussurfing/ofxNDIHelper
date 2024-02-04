#pragma once
#include "ofMain.h"

#define USE_ofxNDI
//-> ifdef directives stuff to help copy paste to your app projects.

#ifdef USE_ofxNDI
	#include "SurfingNDIManager.h"
#endif

#include "ofxGui.h"
#include "ofxSurfingHelpers.h"
#include "ofxWindowApp.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void drawBg();
	void windowResized(int w, int h);
	void exit();

public:
#ifdef USE_ofxNDI
	// NDI
	SurfingNDIManager ndi;
#endif

	//--

	ofxWindowApp windowApp;

	//--

	ofxPanel gui;
	ofParameterGroup params { "ofApp" };
};
