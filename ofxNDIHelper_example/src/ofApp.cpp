#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
#ifdef USE_ofxNDI
	NDIHelper.setup();
#endif

#ifdef USE_ofxWindowApp
	windowApp.setFrameRate(60);
	windowApp.setVerticalSync(true);
#endif

	image.loadImage("pattern1.jpg");
}

//--------------------------------------------------------------
void ofApp::update() {

#ifdef USE_ofxNDI
	NDIHelper.begin_NDI_OUT();
	{
		//ofBackground(32);
		image.draw(0, 0, ofGetWidth(), ofGetHeight());

		//layers one upper others:

		//1. webcam
		//NDIHelper.drawWebcamOut();
		NDIHelper.draw_Preview_Webcam();

		//2. ndi input
		NDIHelper.draw_Preview_NDI_IN();
	}
	NDIHelper.end_NDI_OUT();
#endif

	//----

#ifdef USE_ofxNDI
	NDIHelper.update();
#endif

}

//--------------------------------------------------------------
void ofApp::draw() {
	//ofBackground(32);
	image.draw(0, 0, ofGetWidth(), ofGetHeight());

#ifdef USE_ofxNDI

	//----

	//////webcam
	////NDIHelper.draw_Preview_Webcam();

	////draw ndi out
	//NDIHelper.draw_Preview_NDI_OUT();

	//----

	//draw monitor
	NDIHelper.draw();

	//----

	//gui
	NDIHelper.drawGui();

#endif
}

//--------------------------------------------------------------
void ofApp::exit()
{
#ifdef USE_ofxNDI
	NDIHelper.exit();
#endif

#ifdef USE_ofxWindowApp
	windowApp.exit();
#endif
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
#ifdef USE_ofxNDI
	NDIHelper.windowResized(w, h);
#endif
}
