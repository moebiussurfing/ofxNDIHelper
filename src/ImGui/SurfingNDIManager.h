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
		bUsingFullScreen = true;
		NDIHelper.draw_NDI_IN_1_Full();
		NDIHelper.draw_NDI_IN_2_Full();
		NDIHelper.draw_Webcam_Full();
	};

	bool bUsingFullScreen = false;

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
					//NDIGui.ui.AddSpacing();
					//NDIGui.ui.AddGroup(NDIHelper.params_Panels, SurfingGuiGroupStyle_NoHeader);

					//TODO:
					NDIGui.ui.Add(NDIGui.ui.bMinimize, OFX_IM_TOGGLE_ROUNDED_MINI);
					NDIGui.ui.AddSpacingSeparated();

					if (NDIGui.ui.bMinimize) {
						if (NDIHelper.bWebcam_Enable) NDIGui.ui.Add(NDIHelper.bGui_Webcam, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
						if (NDIHelper.NDI_Input1.bEnable) NDIGui.ui.Add(NDIHelper.bGui_NDI_IN1, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
						if (NDIHelper.NDI_Input2.bEnable) NDIGui.ui.Add(NDIHelper.bGui_NDI_IN2, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
						if (NDIHelper.bNDI_Output_Enable) NDIGui.ui.Add(NDIHelper.bGui_NDI_OUT, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
					}
					else {
						NDIGui.ui.Add(NDIHelper.bGui_Webcam, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
						NDIGui.ui.Add(NDIHelper.bGui_NDI_IN1, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
						NDIGui.ui.Add(NDIHelper.bGui_NDI_IN2, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
						NDIGui.ui.Add(NDIHelper.bGui_NDI_OUT, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
					}

					NDIGui.ui.EndWindowSpecial();
				}

				// Webcam
				if (NDIHelper.bGui_Webcam || !NDIGui.ui.bMinimize)
				{
					if (NDIHelper.bGui_Webcam) ImGui::SetNextWindowSizeConstraints(size_min, size_max);
					if (NDIGui.ui.BeginWindowSpecial(NDIHelper.bGui_Webcam))
					{
						//NDIGui.ui.AddGroup(NDIHelper.params_Webcam, flags);

						NDIGui.ui.Add(NDIHelper.bWebcam_Enable, OFX_IM_TOGGLE);
						NDIGui.ui.Add(NDIHelper.bWebcam_Draw, OFX_IM_TOGGLE_ROUNDED);
						if (!NDIGui.ui.bMinimize)
							if (NDIHelper.bWebcam_Draw && !bUsingFullScreen) 
								NDIGui.ui.Add(NDIHelper.bWebcam_DrawMini, OFX_IM_TOGGLE_ROUNDED_MINI);
						//NDIGui.ui.Add(NDIHelper.bWebcam_Next, OFX_IM_BUTTON_SMALL);
						//NDIGui.ui.Add(NDIHelper.webcam_Index_Device, OFX_IM_STEPPER);
						//NDIGui.ui.Add(NDIHelper.webcam_Name, OFX_IM_TEXT_DISPLAY);
						//NDIGui.ui.Add(NDIHelper.scaleMode_Name);
						ofxImGuiSurfing::AddCombo(NDIHelper.webcam_Index_Device, NDIHelper.webcam_Names);
						ofxImGuiSurfing::AddCombo(NDIHelper.scaleMode_Index, NDIHelper.scaleMode_Names);

						if (!NDIGui.ui.bMinimize) {
							NDIGui.ui.Add(NDIHelper.bWebcam_LockRatio, OFX_IM_TOGGLE_ROUNDED_MINI);
							NDIGui.ui.Add(NDIHelper.rect_Webcam.bEdit, OFX_IM_TOGGLE);
							NDIGui.ui.Add(NDIHelper.bWebcam_Restart, OFX_IM_BUTTON_SMALL_BORDER_BLINK);
						}

						NDIGui.ui.EndWindowSpecial();
					}
				}

				// Out
				if (NDIHelper.bGui_NDI_OUT || !NDIGui.ui.bMinimize)
				{
					if (NDIHelper.bGui_NDI_OUT) ImGui::SetNextWindowSizeConstraints(size_min, size_max);
					if (NDIGui.ui.BeginWindowSpecial(NDIHelper.bGui_NDI_OUT))
					{
						//NDIGui.ui.AddGroup(NDIHelper.params_NDI_Output, flags);

						NDIGui.ui.Add(NDIHelper.bNDI_Output_Enable, OFX_IM_TOGGLE);
						NDIGui.ui.Add(NDIHelper.bNDI_Output_Draw, OFX_IM_TOGGLE_ROUNDED);
						if (NDIHelper.bNDI_Output_Draw && !bUsingFullScreen)
							NDIGui.ui.Add(NDIHelper.bNDI_Output_Mini, OFX_IM_TOGGLE_ROUNDED_MINI);

						if (!NDIGui.ui.bMinimize) {
							NDIGui.ui.Add(NDIHelper.rect_NDI_OUT.bEdit, OFX_IM_TOGGLE);
						}

						NDIGui.ui.EndWindowSpecial();
					}
				}

				// In 1
				if (NDIHelper.bGui_NDI_IN1 || !NDIGui.ui.bMinimize)
				{
					if (NDIHelper.bGui_NDI_IN1) ImGui::SetNextWindowSizeConstraints(size_min, size_max);
					if (NDIGui.ui.BeginWindowSpecial(NDIHelper.bGui_NDI_IN1))
					{
						//NDIGui.ui.AddGroup(NDIHelper.NDI_Input1.params, flags);

						NDIGui.ui.Add(NDIHelper.NDI_Input1.bEnable, OFX_IM_TOGGLE);
						NDIGui.ui.Add(NDIHelper.NDI_Input1.bDraw, OFX_IM_TOGGLE_ROUNDED);
						if (!NDIGui.ui.bMinimize)
							if (NDIHelper.NDI_Input1.bDraw && !bUsingFullScreen)
								NDIGui.ui.Add(NDIHelper.NDI_Input1.bDrawMini, OFX_IM_TOGGLE_ROUNDED_MINI);
						//NDIGui.ui.Add(NDIHelper.NDI_Input1.bNext, OFX_IM_BUTTON_SMALL);

						if (!NDIGui.ui.bMinimize)
							NDIGui.ui.Add(NDIHelper.NDI_Input1.bScan, OFX_IM_TOGGLE_SMALL);

						ofxImGuiSurfing::AddCombo(NDIHelper.NDI_Input1.indexDevice, NDIHelper.NDI_Input1.namesDevices);
						ofxImGuiSurfing::AddCombo(NDIHelper.NDI_Input1.scaleMode_Index, NDIHelper.NDI_Input1.scaleMode_Names);
						if (!NDIGui.ui.bMinimize) {
							NDIGui.ui.Add(NDIHelper.NDI_Input1.bLockRatio, OFX_IM_TOGGLE_ROUNDED_MINI);
							NDIGui.ui.Add(NDIHelper.NDI_Input1.rect_NDI_IN.bEdit, OFX_IM_TOGGLE);
							NDIGui.ui.Add(NDIHelper.NDI_Input1.bReset, OFX_IM_BUTTON_SMALL_BORDER_BLINK);
						}

						NDIGui.ui.EndWindowSpecial();
					}
				}

				// In 2
				if (NDIHelper.bGui_NDI_IN2 || !NDIGui.ui.bMinimize)
				{
					if (NDIHelper.bGui_NDI_IN2) ImGui::SetNextWindowSizeConstraints(size_min, size_max);
					if (NDIGui.ui.BeginWindowSpecial(NDIHelper.bGui_NDI_IN2))
					{
						//NDIGui.ui.AddGroup(NDIHelper.NDI_Input1.params, flags);

						NDIGui.ui.Add(NDIHelper.NDI_Input2.bEnable, OFX_IM_TOGGLE);
						NDIGui.ui.Add(NDIHelper.NDI_Input2.bDraw, OFX_IM_TOGGLE_ROUNDED);
						if (!NDIGui.ui.bMinimize)
							if (NDIHelper.NDI_Input2.bDraw && !bUsingFullScreen) 
								NDIGui.ui.Add(NDIHelper.NDI_Input2.bDrawMini, OFX_IM_TOGGLE_ROUNDED_MINI);
						//NDIGui.ui.Add(NDIHelper.NDI_Input2.bNext, OFX_IM_BUTTON_SMALL);

						if (!NDIGui.ui.bMinimize)
							NDIGui.ui.Add(NDIHelper.NDI_Input2.bScan, OFX_IM_TOGGLE_SMALL);

						ofxImGuiSurfing::AddCombo(NDIHelper.NDI_Input2.indexDevice, NDIHelper.NDI_Input2.namesDevices);
						ofxImGuiSurfing::AddCombo(NDIHelper.NDI_Input2.scaleMode_Index, NDIHelper.NDI_Input2.scaleMode_Names);
						if (!NDIGui.ui.bMinimize) {
							NDIGui.ui.Add(NDIHelper.NDI_Input2.bLockRatio, OFX_IM_TOGGLE_ROUNDED_MINI);
							NDIGui.ui.Add(NDIHelper.NDI_Input2.rect_NDI_IN.bEdit, OFX_IM_TOGGLE);
							NDIGui.ui.Add(NDIHelper.NDI_Input2.bReset, OFX_IM_BUTTON_SMALL_BORDER_BLINK);
						}

						NDIGui.ui.EndWindowSpecial();
					}
				}

				//--

				//if (NDIGui.ui.BeginWindow("NDIHelper")) {
				//	NDIGui.ui.Add(NDIHelper.bGui_Webcam, OFX_IM_TOGGLE);
				//	NDIGui.ui.Add(NDIHelper.bGui_NDI_IN2, OFX_IM_TOGGLE);
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

