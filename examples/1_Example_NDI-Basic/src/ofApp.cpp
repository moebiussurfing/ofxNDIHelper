#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{

	NDIHelper.setup();

	//--

	image.loadImage("assets/image.jpg");


	gui.setup("ofApp");
	gui.add(bDrawImage);
	gui.add(NDIHelper.bGui);
	gui.add(NDIHelper.bGui_Controls);

}

//--------------------------------------------------------------
void ofApp::update()
{

	// Draw inside to feed the NDI Video Output.
	// that's the signal we will send!

	NDIHelper.begin_NDI_OUT();
	{
		// Layers one upper others:

		// 0. Draw a simple and animated scene:
		drawScene();
	}
	NDIHelper.end_NDI_OUT();

}

//--------------------------------------------------------------
void ofApp::draw()
{

	// Draw Preview Monitors
	NDIHelper.draw();

	//----

	// Gui
	NDIHelper.draw_Gui();

	//----

	// ofApp Gui
	gui.draw();
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

	NDIHelper.windowResized(w, h);
}