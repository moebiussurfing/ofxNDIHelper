#pragma once

#include "ofMain.h"

#include "ofxSurfingImGui.h"
//IMPORTANT:
//If you want to use the simpler version, based on ofxGui,
//you must remove the folder ofxNDIHelper/src/ImGui/ from the project!
//There's 4 files inside.
//(If not, will require ofxSurfingImGui, and some other dependencies:
//ofxSurfingImGui, ofxImGui
//Will replace ofxGui with a cool ImGui base GUI workflow. Check:
//https://github.com/moebiussurfing/ofxSurfingImGui)
 
class SurfingNDIGui
{
public:

	SurfingNDIGui();
	~SurfingNDIGui();

	void setup();
	void startup();
	void draw();

	ofxSurfing_ImGui_Manager guiManager;
};

