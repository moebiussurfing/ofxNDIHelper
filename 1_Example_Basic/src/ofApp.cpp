#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{

#ifdef USE_ofxNDI
	NDIHelper.setup();
#endif

	//--

	image.loadImage("assets/image.jpg");

	gui.setup("ofApp");
	gui.add(bDrawImage);
	gui.add(bDrawAddonSources);
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
		drawScene();

		if (bDrawAddonSources) //-> Feed sources from the add-on too.
		{
			// 1. Draw the connected Webcam
			NDIHelper.draw_Preview_Webcam(); //-> The mini preview of the camera.
			//NDIHelper.draw_WebcamOut(); //-> Another useful method. The full screen camera.

			// 2. Draw the receiving from NDI Input.
			NDIHelper.draw_Preview_NDI_IN();
		}
	}
	NDIHelper.end_NDI_OUT();

	//----

	NDIHelper.update();

#endif

}

//--------------------------------------------------------------
void ofApp::draw()
{
	drawScene();

	//--

#ifdef USE_ofxNDI

	//----

	// Draw Preview Monitors
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