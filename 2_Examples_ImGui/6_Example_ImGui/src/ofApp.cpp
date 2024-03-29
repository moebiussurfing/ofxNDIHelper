#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
#ifdef USE_ofxNDI
	// NDI
	ndi.setup();
	ndi.setMode_ImGui();

	//ndi.setGuiInternalVisible(true);//debug
#endif

	//--

	params.add(ndi.bGui);

	gui.setup("ofApp");
	gui.add(params);
	gui.setPosition(40, ofGetHeight() - gui.getHeight() - 40);

	ofxSurfingHelpers::loadGroup(params);
}

//--------------------------------------------------------------
void ofApp::update() {
	// Draw inside to feed the NDI Video Output.
	// that's the signal we will send!

#ifdef USE_ofxNDI
	// NDI
	ndi.begin_NDI_OUT();
	{
		drawBg();

		ndi.drawSignals();
	}
	ndi.end_NDI_OUT();
#endif
}

//--------------------------------------------------------------
void ofApp::drawBg() {
	ofBackground(24);
}

//--------------------------------------------------------------
void ofApp::draw() {
	drawBg();

#ifdef USE_ofxNDI
	// NDI Preview
	ndi.draw();
#endif

	//----

	// Gui

#ifdef USE_ofxNDI
	// NDI Gui
	ndi.drawGui();
#endif

	//----

	// ofApp Gui
	gui.draw();
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
#ifdef USE_ofxNDI
	ndi.windowResized(w, h);
#endif
}

//--------------------------------------------------------------
void ofApp::exit() {
	ofxSurfingHelpers::saveGroup(params);
}