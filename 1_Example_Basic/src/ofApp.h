#pragma once

#include "ofMain.h"

/*

	To Test and understand how it works:

	1. You should install the NDI Tools.
	2. Run an NDI Monitor to preview what is being sent through the output.
	3. You can run an NDI Test Patterns to feed a signal into the NDI Addon.
	4. Have fun with Sources toggles and the addon GUI to explore combinations,
	while looking to the NDI Out Preview GUI or the NDI Monitor video.

*/


#define USE_ofxNDI //-> ifdef directives stuff to help copy paste to other app projects.
#ifdef USE_ofxNDI
#include "ofxNDIHelper.h"
#endif

#include "ofxSurfingHelpers.h"
#include "ofxGui.h"
#include "ofxWindowApp.h"

class ofApp : public ofBaseApp
{

public:

	void setup();
	void update();
	void draw();
	void windowResized(int w, int h);

public:

#ifdef USE_ofxNDI
	ofxNDIHelper NDIHelper;
#endif

	//--

	// window
	ofxWindowApp windowApp;

	//--

	ofxPanel gui;

	// bg image
	ofImage image;
	ofParameter<bool>bDrawImage{ "Image", true };

	ofParameterGroup params{ "Sources To Feed NDI Out" };
	ofParameter<bool>bByPassDrawAddonSources{ "ByPass", false };
	ofParameter<bool>bDraw_Webcam_Mini{ "Cam Mini", true };
	ofParameter<bool>bDraw_Webcam_Full{ "Cam Full", false };
	ofParameter<bool>bDraw_NDI_Input_Mini{ "NDI Mini", true };
	ofParameter<bool>bDraw_NDI_Input_Full{ "NDI Full", false };

	// A simple and animated scene using an image:
	//--------------------------------------------------------------
	void drawScene()
	{
		if (!bDrawImage) {
			ofBackground(32);
			return;
		}

		ofPushMatrix();
		const float noiseAmnt = 0.07f;
		ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
		float scale = ofMap(ofxSurfingHelpers::Bounce(), 0, 1, 1, 1.08f);
		float noise = ofMap(ofxSurfingHelpers::Noise(), -1, 1, -noiseAmnt, noiseAmnt);
		int xOffset = noise * 500;
		int vOffset = noise * 300;
		ofScale(scale + noise);
		image.draw(xOffset - ofGetWidth() / 2, vOffset - ofGetHeight() / 2, ofGetWidth(), ofGetHeight());
		ofPopMatrix();
	};
};
