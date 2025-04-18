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
	gui.setPosition(40, ofGetHeight() - gui.getHeight() - 40);

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
		// Draw Test Scene
		scene.drawAll();

		//--

		// Feed the NDI Helper Previews too:
		NDIHelper.draw_NDI_IN_1();
		NDIHelper.draw_NDI_IN_2();
		NDIHelper.draw_WebCam();
	}
	NDIHelper.end_NDI_OUT();

#endif

}

//--------------------------------------------------------------
void ofApp::draw()
{
	// Draw Test Scene
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
void ofApp::windowResized(int w, int h)
{
#ifdef USE_ofxNDI

	NDIHelper.windowResized(w, h);

#endif
}

//--------------------------------------------------------------
void ofApp::exit()
{
	ofxSurfingHelpers::saveGroup(params);
}
