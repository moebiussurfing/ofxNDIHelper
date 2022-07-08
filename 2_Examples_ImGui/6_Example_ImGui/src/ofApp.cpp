#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{

#ifdef USE_ofxNDI

	ndi.setup();
	ndi.setGuiInternalVisible(true);

#endif

	//--

	params.add(ndi.bGui);
	params.add(ndi.bGui_Controls);

	gui.setup("ofApp");
	gui.add(params);
	gui.setPosition(40, ofGetHeight() - gui.getHeight() - 40);

	ofxSurfingHelpers::loadGroup(params);
}

//--------------------------------------------------------------
void ofApp::update()
{
	// Draw inside to feed the NDI Video Output.
	// that's the signal we will send!

#ifdef USE_ofxNDI

	ndi.begin_NDI_OUT();
	{
		drawBg();

		ndi.drawSignals();
	}
	ndi.end_NDI_OUT();

#endif
}

//--------------------------------------------------------------
void ofApp::drawBg()
{
	ofBackground(20);
	//ofBackground(ofColor::orange);
}

//--------------------------------------------------------------
void ofApp::draw()
{
	drawBg();

#ifdef USE_ofxNDI

	ndi.draw();

#endif

	// Gui

	//----

#ifdef USE_ofxNDI

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
void ofApp::exit()
{
	ofxSurfingHelpers::saveGroup(params);
}