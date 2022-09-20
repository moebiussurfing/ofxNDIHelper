#pragma once

#include "ofxNDIHelper.h"
#include "SurfingNDIGui.h"

class SurfingNDIManager
{
public:

	SurfingNDIManager() {};
	~SurfingNDIManager() {};

	ofxNDIHelper NDIHelper;
	SurfingNDIGui NDIGui;

	ofParameter<bool> bGui{ "NDIManager", true };
	//ofParameter<bool> bGui_Controls{"Controls", true};

public:

	void setMode_ofxGui() {
		NDIHelper.setMode_ofxGui();
	};

	void setMode_ImGui() {
		NDIHelper.setMode_ImGui();
	};

public:

	void setGuiInternalVisible(bool b)//debug
	{
		NDIHelper.bGui_Internal = b;
	}

	void setup() {
		NDIHelper.setup();
		NDIHelper.bGui_Internal = false;

		//--

		NDIGui.setup();

		// Panels
		NDIGui.ui.addWindowSpecial(NDIHelper.bGui);
		NDIGui.ui.addWindowSpecial(NDIHelper.bGui_Webcam);
		NDIGui.ui.addWindowSpecial(NDIHelper.bGui_NDI_IN1);
		NDIGui.ui.addWindowSpecial(NDIHelper.bGui_NDI_IN2);
		NDIGui.ui.addWindowSpecial(NDIHelper.bGui_NDI_OUT);

		NDIGui.startup();

		//--

		// Styles

		NDIGui.ui.ClearStyles();

		NDIGui.ui.AddStyleGroupForBools(NDIHelper.params_Panels, OFX_IM_TOGGLE_ROUNDED_MEDIUM);

		NDIGui.ui.AddStyleGroupForBools(NDIHelper.params_Webcam, OFX_IM_TOGGLE_SMALL);
		NDIGui.ui.AddStyleGroupForBools(NDIHelper.NDI_Input1.params, OFX_IM_TOGGLE_SMALL);
		NDIGui.ui.AddStyleGroupForBools(NDIHelper.NDI_Input2.params, OFX_IM_TOGGLE_SMALL);
		NDIGui.ui.AddStyleGroupForBools(NDIHelper.params_NDI_Output, OFX_IM_TOGGLE_SMALL);

		bGui.makeReferenceTo(NDIHelper.bGui);
	};

	// all the channels for user preview. take care with overwritten or to drawing twice by other draws!
	void draw() {
		NDIHelper.draw();
	};

	// all enabled channels drawn on the full layout canvas
	void drawSignals() {
		NDIHelper.draw_NDI_IN_1();
		NDIHelper.draw_NDI_IN_2();
		NDIHelper.draw_Webcam();
	};

	// all enabled channels drawn at full screen size
	void drawSignalsFullScreen() {
		NDIHelper.draw_NDI_IN_1_Full();
		NDIHelper.draw_NDI_IN_2_Full();
		NDIHelper.draw_Webcam_Full();
	};

	// each channel drawn at full screen size
	void draw_NDI_IN_1_Full() {
		NDIHelper.draw_NDI_IN_1_Full();
	};
	void draw_NDI_IN_2_Full() {
		NDIHelper.draw_NDI_IN_2_Full();
	};
	void draw_Webcam_Full() {
		NDIHelper.draw_Webcam_Full();
	};

	// each channel drawn at mini from canvas layout or full screen size
	void draw_NDI_IN_1() {
		NDIHelper.draw_NDI_IN_1();
	};
	void draw_NDI_IN_2() {
		NDIHelper.draw_NDI_IN_2();
	};

	void draw_Webcam() {
		NDIHelper.draw_Webcam();
	};

	void drawGuiInternal() {
		NDIHelper.draw_Gui();
	};

	void drawPreviews() {
		NDIHelper.drawPreviews();
	};

	void drawGui()
	{
		drawGuiInternal();

		drawImGui();
	};

	void drawImGui() {
		NDIHelper.draw_Gui();

		if (NDIHelper.bGui)
		{
			NDIGui.ui.Begin();
			{
				float w = 165;
				ImVec2 size_min = ImVec2(w, -1);
				ImVec2 size_max = ImVec2(w, -1);

				SurfingGuiGroupStyle flags = SurfingGuiGroupStyle_Collapsed;

				// Main
				if (NDIHelper.bGui) ImGui::SetNextWindowSizeConstraints(size_min, size_max);
				if (NDIGui.ui.BeginWindowSpecial(NDIHelper.bGui)) 
				{
					NDIGui.ui.AddSpacing();

					//TODO:
					//NDIGui.ui.Add(NDIGui.ui.bMinimize);
					NDIGui.ui.AddGroup(NDIHelper.params_Panels);

					NDIGui.ui.EndWindowSpecial();
				}

				// Webcam
				if (NDIHelper.bGui_Webcam || !NDIGui.ui.bMinimize)
				{
					if (NDIHelper.bGui_Webcam) ImGui::SetNextWindowSizeConstraints(size_min, size_max);
					if (NDIGui.ui.BeginWindowSpecial(NDIHelper.bGui_Webcam)) {
						NDIGui.ui.AddGroup(NDIHelper.params_Webcam, flags);

						NDIGui.ui.EndWindowSpecial();
					}
				}

				// In 1
				if (NDIHelper.bGui_NDI_IN1 || !NDIGui.ui.bMinimize)
				{
					if (NDIHelper.bGui_NDI_IN1) ImGui::SetNextWindowSizeConstraints(size_min, size_max);
					//if (NDIGui.ui.BeginWindowSpecial(2)) {
					if (NDIGui.ui.BeginWindowSpecial(NDIHelper.bGui_NDI_IN1)) {
						//ImGui::Text("Hello");
						NDIGui.ui.AddGroup(NDIHelper.NDI_Input1.params, flags);

						NDIGui.ui.EndWindowSpecial();
					}
				}

				// In 2
				if (NDIHelper.bGui_NDI_IN2 || !NDIGui.ui.bMinimize)
				{
					if (NDIHelper.bGui_NDI_IN2) ImGui::SetNextWindowSizeConstraints(size_min, size_max);
					//if (NDIGui.ui.BeginWindowSpecial(3)) {
					if (NDIGui.ui.BeginWindowSpecial(NDIHelper.bGui_NDI_IN2)) {
						//ImGui::Text("Hello");
						NDIGui.ui.AddGroup(NDIHelper.NDI_Input2.params, flags);

						NDIGui.ui.EndWindowSpecial();
					}
				}

				// Out
				if (NDIHelper.bGui_NDI_OUT || !NDIGui.ui.bMinimize)
				{
					if (NDIHelper.bGui_NDI_OUT) ImGui::SetNextWindowSizeConstraints(size_min, size_max);
					if (NDIGui.ui.BeginWindowSpecial(NDIHelper.bGui_NDI_OUT)) {
						NDIGui.ui.AddGroup(NDIHelper.params_NDI_Output, flags);

						NDIGui.ui.EndWindowSpecial();
					}
				}

				//--

				//if (NDIGui.ui.BeginWindow("NDIHelper")) {
				//	NDIGui.ui.Add(NDIHelper.bGui_Webcam, OFX_IM_TOGGLE);
				//	NDIGui.ui.Add(NDIHelper.bGui_NDI_IN1, OFX_IM_TOGGLE);
				//	NDIGui.ui.Add(NDIHelper.bGui_NDI_IN2, OFX_IM_TOGGLE);
				//	NDIGui.ui.Add(NDIHelper.bGui_NDI_OUT, OFX_IM_TOGGLE);
				//	NDIGui.ui.EndWindow();
				//}
			}
			NDIGui.ui.End();
		}
	};

	void windowResized(int w, int h)
	{
		NDIHelper.windowResized(w, h);
	};

	void begin_NDI_OUT() { NDIHelper.begin_NDI_OUT(); };
	void end_NDI_OUT() { NDIHelper.end_NDI_OUT(); };

};

