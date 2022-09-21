
//---

/*

	This add-on stills a bit WIP !!


	TODO:

	+ customize ui widgets better, not using groups!

	+ Layout Canvas: Allow change layers sorting. store an int vector with position of each layer
		now drawing order is: NDI in 1, 2, Webcam and out.

	+ split NDI out to a class like for NDI input

	+ Fix mode scale for Webcam
		+ webcam mode scale move to params like NDI input class

	+ TODO: WIP
	+ should replace by ofxComponent
	+ child_frame: Finish make all transforms proportional to sizes
		+ save load these settings.
		only zoom works.

	+ Real preview size or custom / full size.
	+ Fix resizing exact size of windows to NDI Out or weird margins, real full screen.
		+ e.g. ndi out do not fits exactly the full width frame.

	+ Split Webcam part as a new helper add-on. ?
	+ make cam a class to allow multiple. check MSA cam add-on
	https://github.com/memo/ofxMSAMultiCam

	+ We should use a vector of pointers to allow adding INPUT devices on runtime.

	+ Fix directives+classes to allow use i.e. only the camera or only the Output.

*/


//---


#pragma once

#include "ofMain.h"

//--

#define USE_WEBCAM // Aux camera
#define USE_ofxNDI_OUT // NDI out

//TODO:
// WIP: BUG: can't be removed... breaks camera.
// -> WIP. For transform the Webcam content: 
// zoom and translate. Requires ofxChildFrame.
#define USE_OFX_CHILD_FRAME 

//----

// DEVICES
// 
// 1. Webcam
// 2. 2 x NDI INPUTS
// 3. NDI OUTPUT

//----


//--

// Dependencies

#ifdef USE_ofxNDI_OUT || USE_ofxNDI_IN
#include "ofxNDI.h"
#include "NDI_input.h"
#endif

#include "ofxGui.h"
#include "ofxSurfingBoxInteractive.h"
#include "ofxSurfingBoxHelpText.h"
#include "ofxSurfingHelpers.h"
#include "ofxSurfing_ofxGui.h"
//#include "TextBoxWidget.h"

#ifdef USE_OFX_CHILD_FRAME
#include "ofxChildFrame.h"
#endif

#define CHILD_FRAME_MAX_SCALE 5.f 

//--

class ofxNDIHelper /*: public ofBaseApp*/
{

private:

	float x;
	float y;
	float w;
	float h;
	float wc;
	float hc;

#ifdef USE_OFX_CHILD_FRAME
	ofxChildFrame frame_;
	void mouseDragged(ofMouseEventArgs& mouse);
	void mouseScrolled(ofMouseEventArgs& mouse);
	bool bEnable_ChildFrame = true;
#endif

public:

	ofxNDIHelper();
	~ofxNDIHelper();

	void setup();
	void draw(); // main draw with all the GUI, previews and debug info.
	void drawPreviews(); // draw mini previews
	void draw_Gui();
	void windowResized(int w, int h);

	void doReset_Gui();

private:

	void setup_GuiInternal();
	void setup_Params();
	void startup();
	void update(ofEventArgs& args);
	void exit();
	void draw_InfoDevices();

	bool bLoadedStartupDone = false;

#ifdef USE_ofxNDI_IN

public:

	void draw_NDI_IN_1();
	void draw_NDI_IN_2();

private:

public:

	void draw_NDI_IN_1_MiniPreview();
	void draw_NDI_IN_1_Full();

	void draw_NDI_IN_2_MiniPreview();
	void draw_NDI_IN_2_Full();

#endif

	//--

public:

	void setActive(bool b);
	void setGuiVisible(bool b);
	void setGuiInternalVisible(bool b);
	void setGuiToggleVisible();
	void setPathGlobal(std::string s);
	void setLogLevel(ofLogLevel level);

	//--

public:

	// API

	// Feed the sender aka NDI Out

	void begin_NDI_OUT();
	void end_NDI_OUT();

	//--

public:

	//--------------------------------------------------------------
	void setAutoSave(bool b)
	{
		bAutoSave = b;
	}

	//--

private:

	ofParameterGroup params_AppsSettings;

	//ofParameter<bool> bLockRatio;
	ofParameter<bool> bResetLayout;
	ofParameter<bool> bResetGui;

	bool bDoRestartup = false;

	//--

	ofxSurfingBoxHelpText textBoxWidget;

public:

	ofParameterGroup params_Helper; // For the Gui

	//--

public:

	void doReset_Mini_PreviewsLayout();

private:

	// Default layout
	// Preview viewport sizes

	int padg = 5;
	int pad = 20;
	float wPreview = 320;
	int xPadPreview = 300;
	int yPadPreview = 100;
	float _padx = 11;
	float _pady = -11;
	float _padx2 = 9;
	float _pady2 = 18;
	int x2, y2;
	//float rounded = 2.0;

	bool bLabelsInner = true;
	int padLabel = 3;
	int round = 0;

	// Text Box
	ofTrueTypeFont font;

	//-

private:

	// Updating some params before save will trigs also the group callbacks
	// so we disable this callbacks just in case params updating's are required
	// in this case we will need to update gui position param
	bool bDISABLE_CALLBACKS;

	int screenW, screenH;

	// auto save
	ofParameter<bool> bAutoSave;
	uint64_t timerLast_Autosave = 0;
	int timeToAutosave = 10000;//10 secs

	//-

	void Changed(ofAbstractParameter& e);

#ifdef USE_WEBCAM 
	void Changed_Webcam(ofAbstractParameter& e);
#endif

#ifdef USE_ofxNDI_OUT 
	void Changed_NDI_Out(ofAbstractParameter& e);
#endif

	//-

	// Control Params

private:

	bool bMode_ofxGui = true;
	bool bMode_ImGui = false;

public:

	void setMode_ofxGui() {
		bMode_ofxGui = true;
		bMode_ImGui = false;
	};

	void setMode_ImGui() {
		bMode_ofxGui = false;
		bMode_ImGui = true;
	};

public:

	//ofParameter<bool> bGui_Controls;
	ofParameter<bool> bGui;
	ofParameter<bool> bGui_Internal;//ofxGui
	ofParameter<bool> bGui_Webcam;
#ifdef USE_ofxNDI_IN
	ofParameter<bool> bGui_NDI_IN1;
	ofParameter<bool> bGui_NDI_IN2;
#endif
	ofParameter<bool> bGui_NDI_OUT;

	ofParameterGroup params_Panels;
	ofParameterGroup params_Internal;

private:

	ofParameterGroup params_Callbacks;

	ofParameter<bool> bActive;
	ofParameter<bool> bKeys;
	ofParameter<bool> bDebug;
	ofParameter<bool> bHelp;
	ofParameter<glm::vec2> position_Gui;

	// Gui
	ofxPanel gui_Controls;
	ofxPanel gui_Webcam;
	ofxPanel gui_NDI_Out;

	// Keys
	void keyPressed(ofKeyEventArgs& eventArgs);
	void keyReleased(ofKeyEventArgs& eventArgs);
	void addKeysListeners();
	void removeKeysListeners();

	// Settings
	std::string path_GLOBAL;//this is to folder all files to avoid mixing with other add-ons data
	std::string path_Params_AppSettings;

	void loadSettings();
	void saveSettings();

	//--
	// 
	// 1. Webcam

#ifdef USE_WEBCAM

private:

	ofVideoGrabber webcam_Grabber;
	void setup_Webcam(); // setup webcam from name device nor index
	void setup_Webcam(int index); // setup webcam from device index
	//void exit_Webcam(); // store camera device name to xml
	vector<ofVideoDevice> _devs;

	//CHILD_FRAME
	struct ChildFrame
	{
		ofVec3f anchor;
		ofVec3f translat;
		ofVec3f scale;
	};
	ChildFrame childFrame;
	//char key_ChildFrame = OF_KEY_COMMAND;
	bool bKeyChildFrameState = false;

	//--

	void setup_Webcam_Params();

	void webcam_SaveSettings();
	void webcam_LoadSettings();
	void webcam_Next();

public:

	void doRestart_Webcam(); // restart camera using the current index camera

	void draw_Webcam();//-> for internal use
	void draw_Webcam_MiniPreview(bool bInfo = false);
	void draw_Webcam_Full();

	//private:

	ofParameter<bool> bWebcam_LockRatio;
	ofParameter<bool> bWebcam_Next;
	ofParameter<int> scaleMode_Index;
	ofParameter<string> scaleMode_Name;
	vector<std::string> scaleMode_Names;
	ofParameter<bool> bWebcam_Enable;
	ofParameter<bool> bWebcam_Restart;
	ofParameter<bool> bWebcam_Draw;
	std::string webcam_Names_InputDevices;
	vector<std::string> webcam_Names;
	ofParameter<std::string> webcam_Name_{ "WEBCAM_DEVICE_NAME", "" };
	ofParameter<std::string> webcam_Name; // can be merged both vars?
	ofParameter<bool> bWebcam_DrawMini;
	ofParameter<int> webcam_Index_Device;

private:
	std::string path_WebcamSettings;
	std::string name_WebcamSettings;

	std::string path_rect_Webcam = "Webcam_Mini";
public:
	ofxSurfingBoxInteractive rect_Webcam;

#endif

	//--

	// 2. NDI INPUT

#ifdef USE_ofxNDI_IN

public:

	NDI_input NDI_Input1;
	NDI_input NDI_Input2;

#endif

	//--

//private:
public:

	// 3. NDI OUTPUT

#ifdef USE_ofxNDI_OUT

	ofParameter<bool> bNDI_Output_Enable;
	ofParameter<bool> bNDI_Output_Draw;
	ofParameter<bool> bNDI_Output_Mini;
	ofParameter<std::string> NDI_Output_Name;

public:

	ofxNDIsender NDI_OUT_Sender; // NDI sender object

private:

	void setup_NDI_OUT();
	ofFbo fbo_NDI_Sender; // Fbo used for graphics and sending
	unsigned int senderWidth; // Width of the sender output
	unsigned int senderHeight; // Height of the sender output
	char senderName[256]; // Sender name

	//--

public:

	void draw_NDI_OUT(); // -> for internal use
	void draw_NDI_OUT_MiniPreview(bool bInfo = false);
	void draw_NDI_OUT_Full();

public:

	ofParameterGroup params_Webcam;

private:

	std::string path_rect_NDI_OUT = "NDI_Out";
	std::string name_rect_NDI_OUT = "NDI_Out_Mini";

public:
	ofxSurfingBoxInteractive rect_NDI_OUT;

	ofParameterGroup params_NDI_Output;

#endif//USE_ofxNDI_OUT

};