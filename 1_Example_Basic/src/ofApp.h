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
	void exit();
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
	ofParameter<bool>bDrawImage{ "Image to feed NDI OUT", true };

	ofParameterGroup params{ "EXAMPLE ofxNDIHelper" };
	ofParameter<bool>bDontDraw{ "HIDE ALL", false };
	ofParameter<bool>bDraw_Webcam_Mini{ "Webcam Mini", true };
	ofParameter<bool>bDraw_Webcam_Full{ "Webcam Full", false };
	ofParameter<bool>bDraw_NDI_Input_1_Mini{ "NDI INPUT 1 Mini", true };
	ofParameter<bool>bDraw_NDI_Input_1_Full{ "NDI INPUT 1 Full", false };
	ofParameter<bool>bDraw_NDI_Input_2_Mini{ "NDI INPUT 2 Mini", true };
	ofParameter<bool>bDraw_NDI_Input_2_Full{ "NDI INPUT 2 Full", false };

	//--
	 
	// A simple and animated BG Image scene using an image file:
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
