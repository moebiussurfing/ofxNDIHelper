#pragma once

#include "ofMain.h"
#include "ofxSurfingImGui.h"

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

