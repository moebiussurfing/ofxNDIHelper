#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetWindowPosition(-1920, 25);

	NDIHelper.setup();
}

//--------------------------------------------------------------
void ofApp::update()
{
	NDIHelper.begin_NDI_OUT();
	{
		// Draw Webcam full
		NDIHelper.draw_Webcam_Full();
	}
	NDIHelper.end_NDI_OUT();
}

//--------------------------------------------------------------
void ofApp::draw()
{
	// Draw Webcam preview
	NDIHelper.draw_Webcam_MiniPreview();

	//--

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
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	NDIHelper.mouseDragged(x, y, button);
}