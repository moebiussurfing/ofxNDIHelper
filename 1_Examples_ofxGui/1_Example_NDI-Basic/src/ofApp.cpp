#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	//ofSetWindowPosition(-1920, 25);

	NDIHelper.setup();

	//--

	image.loadImage("assets/image.jpg");

	gui.setup("ofApp");
	gui.add(bDrawImage);
	gui.add(NDIHelper.bGui);
	gui.setPosition(10, ofGetHeight() - gui.getHeight() - 40);
}

//--------------------------------------------------------------
void ofApp::update()
{
	// Draw inside to feed the NDI Video Output.
	// that's the signal we will send!

	NDIHelper.begin_NDI_OUT();
	{
		// Draw a simple and animated scene:
		drawScene();
	}
	NDIHelper.end_NDI_OUT();
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofBackground(32);

	// Draw the full Canvas layout, 
	// with all the enabled devices/channels draws.
	// Like Preview Monitors and/or Full Screen previews,
	// with help/debug info included.
	NDIHelper.draw();

	//----

	// Gui Settings
	NDIHelper.draw_Gui();

	//----

	// ofApp Gui
	gui.draw();
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

	NDIHelper.windowResized(w, h);
}