#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetWindowPosition(-1920 + 55, 60);

	NDIHelper.setup();
}

//--------------------------------------------------------------
void ofApp::update()
{
	NDIHelper.begin_NDI_OUT();
	{
		// Draw Webcam at full screen size
		NDIHelper.draw_Webcam_Full();
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
void ofApp::windowResized(int w, int h) {

	NDIHelper.windowResized(w, h);
}