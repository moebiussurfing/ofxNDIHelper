#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
#ifdef USE_ofxNDI

	NDIHelper.setup();

#endif

	//--

	image.loadImage("assets/image.jpg");

	params.add(bDrawImage);

	ofParameterGroup params_To_NDI_Out{ "TO NDI OUT" };
	params_To_NDI_Out.add(bDontDraw);
	params_To_NDI_Out.add(bDraw_Webcam_Mini);
	params_To_NDI_Out.add(bDraw_Webcam_Full);
	params_To_NDI_Out.add(bDraw_NDI_Input_1_Mini);
	params_To_NDI_Out.add(bDraw_NDI_Input_1_Full);
	params_To_NDI_Out.add(bDraw_NDI_Input_2_Mini);
	params_To_NDI_Out.add(bDraw_NDI_Input_2_Full);
	params.add(params_To_NDI_Out);

	gui.setup("ofApp");
	gui.add(params);

	gui.add(NDIHelper.bGui);
	gui.add(NDIHelper.bGui_Controls);

	// Handle session settings
	ofxSurfingHelpers::loadGroup(params, "ofApp.json");
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
		{
			drawScene();
		}

		//-> Feed sources from the add-on too.
		if (!bDontDraw)
		{
			// 1. Draw the receiving signal from NDI Inputs from the add-on:

			// Input Channel 1
			//-> The Full screen of NDI Input
			if (bDraw_NDI_Input_1_Full) NDIHelper.draw_NDI_IN_1_Full();
			//-> The Mini screen of NDI Input
			if (bDraw_NDI_Input_1_Mini) NDIHelper.draw_NDI_IN_1_MiniPreview();

			// Input Channel 2
			//-> The Full screen of NDI Input
			if (bDraw_NDI_Input_2_Full) NDIHelper.draw_NDI_IN_2_Full();
			//-> The Mini screen of NDI Input
			if (bDraw_NDI_Input_2_Mini) NDIHelper.draw_NDI_IN_2_MiniPreview();

			//--

			// 2. Draw the connected Webcam from the add-on:

			//-> The full preview of the camera.
			if (bDraw_Webcam_Full) NDIHelper.draw_Webcam_Full();

			//-> The mini preview of the camera.
			if (bDraw_Webcam_Mini) NDIHelper.draw_Webcam_MiniPreview();
		}
	}
	NDIHelper.end_NDI_OUT();

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

	NOTICE that, obviously, we can draw the above methods everywhere here too.
	The above draws (on update) are being drawn there
	to be feeded into the NDI OUTPUT signal!

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

//--------------------------------------------------------------
void ofApp::exit()
{
	// handle session settings
	ofxSurfingHelpers::saveGroup(params, "ofApp.json");
}
