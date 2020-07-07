#pragma once

#include "ofMain.h"

//--

//sources
//#define USE_WEBCAM
//#define MODE_DRAW_3D_CUBE//uncomment to show a simple 3d cube or a ofxSceneTEST scene (requires addon)
//#define USE_DRAW_SCENE

//#ifdef USE_DRAW_SCENE
//#include "ofxSceneTEST.h"
//#endif

//--

#define USE_ofxNDI
#ifdef USE_ofxNDI
#include "ofxNDIHelper.h"
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

//#ifdef USE_WEBCAM
//	ofVideoGrabber vidGrabber; // Webcam
	//void drawCamera();
//#endif

};
