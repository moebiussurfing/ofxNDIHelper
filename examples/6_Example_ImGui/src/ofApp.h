#pragma once

#include "ofMain.h"

/*

	This example feeds all the manager signals into the NDI Out.
	The you can pick your network NDI inputs and webcam,
	resize the mini previews and this layout will be sent to the NDI Out as is.

	To Test and understand how it works:

	1. You should install the NDI Tools.
	2. Run an NDI Monitor to preview what is being sent through the output.
	3. You can run an NDI Test Patterns to feed a signal into the NDI add-on.
	4. Have fun with Sources toggles and the add-on GUI to explore combinations,
	while looking to the NDI Out Preview GUI or the NDI Monitor video.

*/


#define USE_ofxNDI //-> ifdef directives stuff to help copy paste to your app projects.

#ifdef USE_ofxNDI
#include "SurfingNDIManager.h"
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
	void exit();

public:

#ifdef USE_ofxNDI
	SurfingNDIManager ndi;
#endif

	//--

	ofxWindowApp windowApp;

	//--

	ofxPanel gui;
	ofParameterGroup params{"ofApp"};
};
