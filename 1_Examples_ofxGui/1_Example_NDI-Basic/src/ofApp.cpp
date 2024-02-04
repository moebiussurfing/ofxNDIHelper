#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	//ofSetWindowPosition(-1920, 25);
	
	string s = "1_Example_NDI-Basic";
	ofSetWindowTitle(s);

	NDIHelper.setup();

	//--

	image.loadImage("assets/image.jpg");

	gui.setup("ofApp");
	gui.add(NDIHelper.bGui);
	gui.add(bDrawImage);
	gui.add(bDrawSource);
	gui.setPosition(10, ofGetHeight() - gui.getHeight() - 40);
}

//--------------------------------------------------------------
void ofApp::update() {
	// Draw inside to feed the NDI Video Output.
	// that's the signal we will send!

	NDIHelper.begin_NDI_OUT();
	{
		if (bDrawSource) {
			// Draw to send to the output
			NDIHelper.draw_NDI_IN_1();
			NDIHelper.draw_NDI_IN_2();
		} else {
			// Draw a simple and animated scene:
			drawScene();
		}
	}
	NDIHelper.end_NDI_OUT();
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofBackground(32);

	// Draw the full Canvas layout,
	// with all the enabled devices/channels draws.
	// Like Preview Monitors and/or Full Screen previews,
	// with help/debug info included.
	NDIHelper.draw();

	//----

	// ui settings
	NDIHelper.draw_Gui();

	//----

	// ofApp
	gui.draw();
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

	NDIHelper.windowResized(w, h);
}

//--

// A simple and animated BG Image scene using an image file:
//--------------------------------------------------------------
void ofApp::drawScene() {
	if (!bDrawImage) {
		ofBackground(32);
		return;
	}

	if (!image.isAllocated()) return;
	ofPushMatrix();
	const float noiseAmnt = 0.07f;
	ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
	float scale = ofMap(ofxSurfingHelpers::Bounce(), 0, 1, 1, 1.08f);
	float noise = ofMap(ofxSurfingHelpers::Noise(), -1, 1, -noiseAmnt, noiseAmnt);
	int xOffset = noise * 500;
	int vOffset = noise * 300;
	ofScale(scale + noise);
	image.draw(xOffset - ofGetWidth() / 2, vOffset - ofGetHeight() / 2, ofGetWidth(), ofGetHeight());
	ofPopMatrix();
};