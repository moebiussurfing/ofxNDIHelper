#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofEnableAlphaBlending();

#ifdef USE_ofxNDI

	NDIHelper.setup();

#endif

	params.add(scene.bGui);
	params.add(NDIHelper.bGui);

	gui.setup("ofApp");
	gui.add(params);

	ofxSurfingHelpers::loadGroup(params);
}

//--------------------------------------------------------------
void ofApp::update()
{

#ifdef USE_ofxNDI

	// Draw inside to feed the NDI Video Output.
	// that's the signal we will send!

	NDIHelper.begin_NDI_OUT();
	{
		// Draw independent channels 
		// to use as layers to mix/blend with your own code
		//scene.drawBackground();
		//scene.drawChannel1();
		//scene.drawChannel2();

		// Draw all channels 
		// one upon other without any blending
		scene.drawAll();
	}
	NDIHelper.end_NDI_OUT();

#endif

}

//--------------------------------------------------------------
void ofApp::draw()
{
	scene.drawAll();

	//--

#ifdef USE_ofxNDI

	// Draw Preview Monitors
	NDIHelper.draw();

	//----

	// Gui
	NDIHelper.draw_Gui();

#endif

	//----

	// Scene Gui
	scene.drawGui();

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