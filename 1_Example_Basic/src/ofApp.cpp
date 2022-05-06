#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
#ifdef USE_ofxNDI
	NDIHelper.setup();
#endif

	image.loadImage("assets/image.jpg");

	gui.setup("ofApp");
	gui.add(bDrawImage);
	gui.add(NDIHelper.bGui);
	gui.add(NDIHelper.bGui_Controls);
}

//--------------------------------------------------------------
void ofApp::update()
{

#ifdef USE_ofxNDI

	// Draw inside to feed the NDI Video Output.
	// that's the signal we will send!

	NDIHelper.begin_NDI_OUT();
	{
		// Layers one upper others:

		// 0. Draw a simple and animated scene:
		if (bDrawImage) drawScene();
		else ofBackground(32);

		// 1. Draw the Webcam
		//NDIHelper.draw_WebcamOut();
		NDIHelper.draw_Preview_Webcam();

		// 2. Draw the NDI Input
		NDIHelper.draw_Preview_NDI_IN();
	}
	NDIHelper.end_NDI_OUT();

	//----

	NDIHelper.update();
#endif

}

//--------------------------------------------------------------
void ofApp::draw()
{
	// 0. Draw a simple and animated scene:
	if (bDrawImage) drawScene();
	else ofBackground(32);

	//--

#ifdef USE_ofxNDI

	//----

	// Draw monitor
	NDIHelper.draw();

	//----

	// Gui
	NDIHelper.draw_Gui();

	//----

/*
	// Other useful methods:
	// Preview Webcam
	NDIHelper.draw_Preview_Webcam();

	// Preview NDI Out
	NDIHelper.draw_Preview_NDI_OUT();
*/

#endif

// ofApp Gui
	gui.draw();
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
#ifdef USE_ofxNDI
	NDIHelper.windowResized(w, h);
#endif
}