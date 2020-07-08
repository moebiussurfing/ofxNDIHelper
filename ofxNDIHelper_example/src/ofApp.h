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
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

public:
#ifdef USE_ofxNDI
	ofxNDIHelper NDIHelper;
#endif

	//window
#ifdef USE_ofxWindowApp
	ofxWindowApp windowApp;
#endif
};
