#pragma once

#include "ofMain.h"

/*
	This example is an NDI Sender
	Draws an webcam image at full screen into an NDI Out to be broad casted into your network.
	Then can be received into any NDI receiver like an NDI Monitor app.
	Also you will draw the preview webcam for monitoring purposes.

	To Test and understand how the addon works:

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
};
