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
	
	params.add(bByPassDrawAddonSources);
	params.add(bDraw_Webcam_Mini);
	params.add(bDraw_Webcam_Full);
	params.add(bDraw_NDI_Input_Mini);
	params.add(bDraw_NDI_Input_Full);
	gui.add(params);

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

		if (!bByPassDrawAddonSources) //-> Feed sources from the add-on too.
		{
			// 1. Draw the receiving signal from NDI Input from the add-on:
			
			//-> The Full screen of NDI Input
			if (bDraw_NDI_Input_Full) NDIHelper.draw_NDI_IN_Full();

			//-> The Mini screen of NDI Input
			if (bDraw_NDI_Input_Mini) NDIHelper.draw_NDI_IN_MiniPreview();

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

	Notice that obviously we can draw the above methods here too.
	The above draw are being feeded into the NDI OUTPUT signal!
	
	*/
	
	//if (bDraw_NDI_Input_Full) NDIHelper.draw_NDI_IN_Full();
	//if (bDraw_NDI_Input_Mini) NDIHelper.draw_NDI_IN_MiniPreview();
	//if (bDraw_Webcam_Full) NDIHelper.draw_Webcam_Full();
	//if (bDraw_Webcam_Mini) NDIHelper.draw_Webcam_MiniPreview();

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