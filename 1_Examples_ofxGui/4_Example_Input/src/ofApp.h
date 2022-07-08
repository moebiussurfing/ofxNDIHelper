#pragma once

#include "ofMain.h"

/*
	This example is a basic NDI Receiver

	1. Start your network NDI senders.
	2. Select device index to pick a sender.
	3. Press key 'd' to debug and show useful info.
	4. Press key 'm' to switch between full screen or mini modes.
	5. Draw and edit your mini preview: size and placement.
	6. Play with Scale Mode slider to see how drawing performs into the view port aka mini preview

	IMPORTANT:
	If you want to use the simpler version, based on ofxGui,
	you must remove the folder ofxNDIHelper/src/ImGui/ from the project!
	There's 4 files inside.
	(If not, will require ofxSurfingImGui, and some other dependencies:
	ofxSurfingImGui, ofxImGui
	Will replace ofxGui with a cool ImGui base GUI workflow. Check:
	https://github.com/moebiussurfing/ofxSurfingImGui)
*/


#include "NDI_input.h" //-> We can use the NDI input class alone.`

#include "ofxGui.h"

class ofApp : public ofBaseApp
{

public:

	void setup();
	void draw();
	void keyPressed(int key);

public:

	NDI_input receiver;
};
