#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	// NDI
#ifdef USE_ofxNDI

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
void ofApp::update()
{
	// Draw inside to feed the NDI Video Output.
	// that's the signal we will send!
	
	// NDI
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
	ofBackground(24);
}

//--------------------------------------------------------------
void ofApp::draw()
{
	drawBg();

	// NDI Preview
#ifdef USE_ofxNDI

	ndi.draw();

#endif

	// Gui

	//----

	// NDI Gui
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