#pragma once
#include "ofMain.h"

/*
	This example is an NDI Sender
	but you can play too with the other receivers and webCam managers.
	Notice that on this example, this previews will can be sent to the NDI Out!
	Or is allowed sending the image instead.
	Draws an animated background image into an NDI Out to be broad casted into your network.
	Then can be received into any NDI receiver like an NDI Monitor app.

	To Test and understand how the add-on works:

	1. You should install the NDI Tools.
	2. Run an NDI Monitor to preview what is being sent through the output.
	3. You can run an NDI Test Patterns to feed a signal into the NDI add-on.
	4. Have fun with Sources toggles and the add-on GUI to explore combinations,
	while looking to the NDI Out Preview GUI or the NDI Monitor video.

	IMPORTANT:
	If you want to use the simpler version, based on ofxGui,
	you must remove the folder ofxNDIHelper/src/ImGui/ from the project!
	There's 4 files inside. 
	(If not, will require ofxSurfingImGui, and some other dependencies:
	ofxSurfingImGui, ofxImGui 
	Will replace ofxGui with a cool ImGui base GUI workflow. Check:
	https://github.com/moebiussurfing/ofxSurfingImGui)
*/

#include "ofxNDIHelper.h"

#include "ofxGui.h"

//--

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void windowResized(int w, int h);
	void drawScene();

public:
	ofxNDIHelper NDIHelper;

	//--

	ofxPanel gui;

	// Bg image
	ofImage image;
	ofParameter<bool> bDrawImage { "IMG to feed NDI_Out", true };

	ofParameter<bool> bDrawSource { "Source NDI_Inputs/IMG", false };
};
