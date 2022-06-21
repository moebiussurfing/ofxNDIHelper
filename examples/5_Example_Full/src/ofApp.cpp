#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{

#ifdef USE_ofxNDI

	NDIHelper.setup();

#endif

	//--

	params.add(NDIHelper.bGui);
	params.add(NDIHelper.bGui_Controls);

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

	NDIHelper.begin_NDI_OUT();
	{
		ofBackground(ofColor::orange);

		// Feed the NDI Helper Previews:
		{
			NDIHelper.draw_NDI_IN_1();
			NDIHelper.draw_NDI_IN_2();
			NDIHelper.draw_Webcam();
		}
	}

	NDIHelper.end_NDI_OUT();

#endif

}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofBackground(ofColor::orange);

#ifdef USE_ofxNDI

	// Draw Preview Monitors
	NDIHelper.draw();

	//----

	// Gui
	NDIHelper.draw_Gui();

#endif

	//----

	// ofApp Gui
	gui.draw();
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

#ifdef USE_ofxNDI

	NDIHelper.windowResized(w, h);

#endif

}

//--------------------------------------------------------------
void ofApp::exit()
{
	ofxSurfingHelpers::saveGroup(params);
}