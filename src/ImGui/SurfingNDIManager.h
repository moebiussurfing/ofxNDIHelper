#pragma once

#include "ofxNDIHelper.h"
#include "SurfingNDIGui.h"

class SurfingNDIManager
{
public:

	SurfingNDIManager(){};
	~SurfingNDIManager(){};

	ofxNDIHelper NDIHelper;
	SurfingNDIGui NDIGui;

	ofParameter<bool> bGui{"NDIManager", true};
	ofParameter<bool> bGui_Controls{"Controls", true};

	void setup() {
		NDIHelper.setup();
		NDIHelper.bGui_Internal = false;

		//--

		NDIGui.setup();

		NDIGui.guiManager.addWindowSpecial(NDIHelper.bGui_Controls);
		NDIGui.guiManager.addWindowSpecial(NDIHelper.bGui_Webcam);
		NDIGui.guiManager.addWindowSpecial(NDIHelper.NDI_Input1.bGui);
		NDIGui.guiManager.addWindowSpecial(NDIHelper.NDI_Input2.bGui);
		NDIGui.guiManager.addWindowSpecial(NDIHelper.bGui_NDI_OUT);

		NDIGui.startup();
		
		//--

		NDIGui.guiManager.ClearStyles();
		
		NDIGui.guiManager.AddStyleGroupForBools(NDIHelper.params_Panels, OFX_IM_TOGGLE_ROUNDED_MEDIUM);

		NDIGui.guiManager.AddStyleGroupForBools(NDIHelper.params_Webcam, OFX_IM_TOGGLE_SMALL);
		NDIGui.guiManager.AddStyleGroupForBools(NDIHelper.NDI_Input1.params, OFX_IM_TOGGLE_SMALL);
		NDIGui.guiManager.AddStyleGroupForBools(NDIHelper.NDI_Input2.params, OFX_IM_TOGGLE_SMALL);
		NDIGui.guiManager.AddStyleGroupForBools(NDIHelper.params_NDI_Output, OFX_IM_TOGGLE_SMALL);

		bGui.makeReferenceTo(NDIHelper.bGui);
		bGui_Controls.makeReferenceTo(NDIHelper.bGui_Controls);
	};

	void drawSignals(){
		NDIHelper.draw_NDI_IN_1();
		NDIHelper.draw_NDI_IN_2();
		NDIHelper.draw_Webcam();
	};

	void drawGuiInternal() {
		NDIHelper.draw_Gui();
	};

	void draw() { // all the channels for user preview. take car with overwritten by other draws!
		NDIHelper.draw();
	};

	void drawPreviews() {
		NDIHelper.drawPreviews();
	};

	void drawGui() {
		drawGuiInternal();
		drawImGui();
	};

	void drawImGui() { 
		NDIHelper.draw_Gui(); 

		if (NDIHelper.bGui)
		{
			NDIGui.guiManager.begin();
			{
				float w = 175;
				ImVec2 size_min = ImVec2(w, -1);
				ImVec2 size_max = ImVec2(w, -1);

				// Main
				ImGui::SetNextWindowSizeConstraints(size_min, size_max);
				if (NDIGui.guiManager.beginWindowSpecial(NDIHelper.bGui_Controls)) {
					NDIGui.guiManager.AddGroup(NDIHelper.params_Panels);
					NDIGui.guiManager.endWindowSpecial();
				}

				// Webcam
				ImGui::SetNextWindowSizeConstraints(size_min, size_max);
				if (NDIGui.guiManager.beginWindowSpecial(NDIHelper.bGui_Webcam)) {
					NDIGui.guiManager.AddGroup(NDIHelper.params_Webcam);
					NDIGui.guiManager.endWindowSpecial();
				}

				// In 1
				ImGui::SetNextWindowSizeConstraints(size_min, size_max);
				if (NDIGui.guiManager.beginWindowSpecial(2)) {
				//if (NDIGui.guiManager.beginWindowSpecial(NDIHelper.NDI_Input1.bGui)) {
					ImGui::Text("Hello");
					NDIGui.guiManager.AddGroup(NDIHelper.NDI_Input1.params);
					NDIGui.guiManager.endWindowSpecial();
				}

				// In 2
				ImGui::SetNextWindowSizeConstraints(size_min, size_max);
				if (NDIGui.guiManager.beginWindowSpecial(3)) {
				//if (NDIGui.guiManager.beginWindowSpecial(NDIHelper.NDI_Input2.bGui)) {
					ImGui::Text("Hello");
					NDIGui.guiManager.AddGroup(NDIHelper.NDI_Input2.params);
					NDIGui.guiManager.endWindowSpecial();
				}

				// Out
				ImGui::SetNextWindowSizeConstraints(size_min, size_max);
				if (NDIGui.guiManager.beginWindowSpecial(NDIHelper.bGui_NDI_OUT)) {
					NDIGui.guiManager.AddGroup(NDIHelper.params_NDI_Output);
					NDIGui.guiManager.endWindowSpecial();
				}
			}
			NDIGui.guiManager.end();
		}
	};

	void windowResized(int w, int h)
	{
		NDIHelper.windowResized(w, h);
	};

	void begin_NDI_OUT(){ NDIHelper.begin_NDI_OUT(); };
	void end_NDI_OUT(){ NDIHelper.end_NDI_OUT(); };

};

