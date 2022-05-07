#pragma once

#include "ofMain.h"

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

	// bg image
	ofImage image;
	ofParameter<bool>bDrawImage{ "Image", true };

	ofxPanel gui;

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
