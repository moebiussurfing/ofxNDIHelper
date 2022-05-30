
//---

/*
	TODO:

	+ improve drawing webcam and ndi workflow
	+ switch to ofxSurfingBox for previews rects
		+ add rect container to center the webcam, zoom, rotate, translate inside!
	+ add full screen, fit, scale fit, half screen etc enum list

	+ Fix resizing exact size of windows to NDI Out or weird margins, real full screen.
	+ NDI output devices port settings as names instead of index port.
	+ We should use a vector of pointers to allow adding INPUT devices on runtime.
	+ Real preview size or custom/fullsize.
	+ Fix directives to allow use i.e. only the camera or only the Output.
		+ Split Webcam part as a new helper addon. ?

*/

//---


#pragma once

#include "ofMain.h"

//--

#define USE_WEBCAM // Aux camera
#define USE_ofxNDI_OUT // NDI out
//#define USE_ofxNDI_IN // NDI input //-> moved to NDI_Input.h!

// fix workaround startup
//#define FIX_WORKAROUND_STARTUP_FREEZE // Sometimes Webcam hangs on startup
// NOTE: if webcam hangs during runtime, you should Disable and Enable again to restart/fix!


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
#include "ofxSurfingHelpers.h"
#include "ofxSurfing_ofxGui.h"
#include "TextBoxWidget.h"

//--

class ofxNDIHelper /*: public ofBaseApp*/
{

public:

	ofxNDIHelper();
	~ofxNDIHelper();

	void setup();
	void draw();
	void draw_Gui();
	void windowResized(int w, int h);

private:

	void setup_Gui();
	void reset_Gui();
	void setup_Params();
	void startup();
	void update(ofEventArgs& args);
	void exit();

	void draw_InfoDevices();

#ifdef USE_ofxNDI_IN

public:
	void draw_NDI_IN_1();
	void draw_NDI_IN_2();

private:
	void draw_NDI_IN_1_MiniPreview();
	void draw_NDI_IN_1_Full();
	void draw_NDI_IN_2_MiniPreview();
	void draw_NDI_IN_2_Full();

#endif

	//--

public:
	
	void setActive(bool b);
	void setGuiVisible(bool b);
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

	//--------------------------------------------------------------
	void startupFix()
	{
		ofLogNotice(__FUNCTION__);

		//bEdit = bEdit_PRE;
	}

	//--

private:

	ofParameterGroup params_AppsSettings;
	ofParameter<bool> bLockRatio;
	ofParameter<bool> bReset;

	bool bDoRestartup = false;

	//--

	TextBoxWidget textBoxWidget;

public:

	ofParameterGroup params_User;//For the Gui

	//--

public:

	void doReset_Mini_Previews();

private:

	// Default layout
	// Preview viewport sizes

	int padg = 5;
	int pad = 20;
	float wPreview = 320;
	int xPadPreview = 300;
	int yPadPreview = 100;
	float _padx = 9;
	float _pady =  - 11;
	float _padx2 = 9;
	float _pady2 = 18;
	int x2, y2;
	float rounded = 2.0;

	// Text Box
	ofTrueTypeFont font;
	ofTrueTypeFont fontBig;

	//-

private:

	// Updating some params before save will trigs also the group callbacks
	// so we disable this callbacks just in case params updating's are required
	// in this case we will need to update gui position param
	bool bDISABLECALLBACKS;

	int screenW, screenH;

	// auto save
	ofParameter<bool> bAutoSave;
	uint64_t timerLast_Autosave = 0;
	int timeToAutosave = 10000;//10 secs

	//-

	void Changed(ofAbstractParameter& e);

	//-

	// Control Params

public:

	ofParameter<bool> bGui_Controls;
	ofParameter<bool> bGui;
	ofParameter<bool> bGui_Webcam;
	ofParameter<bool> bGui_NDI_OUT;

private:

	ofParameterGroup params_Internal;
	ofParameterGroup params_Callbacks;
	ofParameterGroup params_Panels;
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

	ofVideoGrabber vidGrabber;
	void setup_Webcam(); // setup webcam from name device nor index
	void setup_Webcam(int index); // setup webcam from device index
	//void exit_Webcam(); // store camera device name to xml
	vector<ofVideoDevice> _devs;

	//--

	void webcam_SaveSettings();
	void webcam_LoadSettings();

public:

	void doRestart_Webcam(); // restart camera using the current index camera

	void draw_Webcam();//-> for internal use
	void draw_Webcam_MiniPreview(bool bInfo = false);
	void draw_Webcam_Full();

private:

	ofParameter<bool> bWebcam_Enable;
	ofParameter<bool> bWebcam_Draw;
	std::string webcam_Names_InputDevices;
	ofParameter<std::string> webcam_Name_{ "WEBCAM_DEVICE_NAME", "" };
	ofParameter<std::string> webcam_Name; // can be merged both vars?
	ofParameter<bool> bWebcam_Mini;
	ofParameter<int> webcam_Index_Device;
	std::string path_WebcamSettings;

	ofxSurfingBoxInteractive rect_Webcam;
	std::string path_rect_Webcam = "Webcam_Mini";

#endif

	//--

	// 2. NDI INPUT

#ifdef USE_ofxNDI_IN

	NDI_input NDI_Input1;
	NDI_input NDI_Input2;

#endif

	//--

	// 3. NDI OUTPUT

#ifdef USE_ofxNDI_OUT

	ofParameter<bool> bNDI_Output_Enable;
	ofParameter<bool> bNDI_Output_Draw;
	ofParameter<bool> bNDI_Output_Mini;
	ofParameter<std::string> NDI_Output_Name;

private:

	void setup_NDI_OUT();
	ofxNDIsender ndiSender; // NDI sender object
	ofFbo fbo_NDI_Sender; // Fbo used for graphics and sending
	unsigned int senderWidth; // Width of the sender output
	unsigned int senderHeight; // Height of the sender output
	char senderName[256]; // Sender name

	//--

public:

	void draw_NDI_OUT();//-> for internal use
	void draw_NDI_OUT_MiniPreview(bool bInfo = false);
	void draw_NDI_OUT_Full();

private:

	ofParameterGroup params_Webcam;
	ofxSurfingBoxInteractive rect_NDI_OUT;
	std::string path_rect_NDI_OUT = "NDI_Out_Mini";

	ofParameterGroup params_NDI_Output;

#endif//USE_ofxNDI_OUT

};