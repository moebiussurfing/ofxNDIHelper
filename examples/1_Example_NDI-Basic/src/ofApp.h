#pragma once

#include "ofMain.h"

/*
	This example is an NDI Sender
	but you can play too with the other receivers and webcam managers.
	Notice that on this example, this previews will not been sended to the NDI Out!
	Only the image scene.

	Draws an animated background image into an NDI Out to be broad casted into your network.
	Then can be received into any NDI receiver like an NDI Monitor app.

	To Test and understand how the add-on works:

	1. You should install the NDI Tools.
	2. Run an NDI Monitor to preview what is being sent through the output.
	3. You can run an NDI Test Patterns to feed a signal into the NDI Addon.
	4. Have fun with Sources toggles and the addon GUI to explore combinations,
	while looking to the NDI Out Preview GUI or the NDI Monitor video.

*/


#include "ofxNDIHelper.h"

#include "ofxGui.h"

class ofApp : public ofBaseApp
{

public:

	void setup();
	void update();
	void draw();
	void windowResized(int w, int h);

public:

	ofxNDIHelper NDIHelper;

	//--

	ofxPanel gui;

	// Bg image
	ofImage image;
	ofParameter<bool>bDrawImage{ "Image to feed NDI OUT", true };

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
