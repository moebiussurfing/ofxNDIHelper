#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	//ofSetWindowPosition(-1920 + 55, 60);

	NDIHelper.setup();

	// Force visible GUI panels for our purposes here:
	// Feed the webcam to NDI out.
	// We won't need the other channels. 
	// Then overwrite the previous sessions states, in case they were enabled...
	if (1) {
		NDIHelper.bGui_WebCam = true;
		NDIHelper.bGui_NDI_OUT = true;
		NDIHelper.bGui_NDI_IN1 = false;
		NDIHelper.bGui_NDI_IN2 = false;
	}
}

//--------------------------------------------------------------
void ofApp::update()
{
	NDIHelper.begin_NDI_OUT();
	{
		// Draw Webcam at full screen size
		NDIHelper.draw_WebCam_Full();
	}
	NDIHelper.end_NDI_OUT();
}

//--------------------------------------------------------------
void ofApp::draw()
{
	// Draw Previews
	NDIHelper.draw();

	//----

	// Gui Controls
	NDIHelper.draw_Gui();
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
	NDIHelper.windowResized(w, h);
}
