
//---

/*
	TODO:

	+ Make a class with the NDI INPUT to add multiple channels. Start with 2 INPUT channels.
	+ Fix resizing windows to NDI Out or weird margins, real full screen.
	+ Fix directives to allow use i.e. only the camera or only the Output.
	+ Split Webcam part as a new helper addon. ?
	+ NDI input/output devices port settings as names instead of index port.

*/

//---


#pragma once

#include "ofMain.h"

//--

#define USE_WEBCAM // Aux camera
#define USE_ofxNDI_IN // NDI input
#define USE_ofxNDI_OUT // NDI out

// fix workaround startup
//#define FIX_WORKAROUND_STARTUP_FREEZE // Sometimes Webcam hangs on startup
// NOTE: if webcam hangs during runtime, you should Disable and Enable again to restart/fix!


//--

// Dependencies

#ifdef USE_ofxNDI_OUT || USE_ofxNDI_IN
#include "ofxNDI.h"
#endif

#include "ofxGui.h"
#include "ofxInteractiveRect.h"
#include "ofxSurfingHelpers.h"
#include "ofxSurfing_ofxGui.h"
#include "TextBoxWidget.h"

//--

class ofxNDIHelper : public ofBaseApp
{

public:

	ofxNDIHelper();
	~ofxNDIHelper();

	void setup();
	void setup_Params();
	void startup();
	void update(ofEventArgs & args);
	void draw();
	void exit();
	void draw_Gui();
	void windowResized(int w, int h);

	//--

	void setActive(bool b);
	void setGuiVisible(bool b);
	void setGuiToggleVisible();
	void setPathGlobal(std::string s);
	void setLogLevel(ofLogLevel level);

	//--

public:

	// API

	// feed the sender
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
	void fixStartup()
	{
		bEdit = bEdit_PRE;

		NDI_Input_Index = NDI_Input_Index;//retrig
	}
	//--

private:

	ofParameterGroup params_AppsSettings;
	ofParameter<bool> bEdit;
	ofParameter<bool> bLockRatio;
	ofParameter<bool> bReset;
	bool bEdit_PRE;
	bool bDoRestartup = false;

	//--

	TextBoxWidget textBoxWidget;

public:

	ofParameterGroup params_User;//For the Gui

	//--

public:

	void doReset_Mini_Previews();
	
private:

	// default layout
	// preview viewport sizes
	int xPadPreview = 300;
	int yPadPreview = 100;
	int pad = 20;
	float wPreview = 320;
	float _padx = 9;
	float _pady = 9 - pad;
	float rounded = 2.0;
	float _padx2 = 9;
	float _pady2 = 18;
	int x2, y2;

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

	// autosave
	ofParameter<bool> bAutoSave;
	uint64_t timerLast_Autosave = 0;
	int timeToAutosave = 10000;//10 secs

	//-

	void Changed_Params_AppSettings(ofAbstractParameter &e);

	//-

	// Control Params

public:

	ofParameter<bool> bGui_Controls;
	ofParameter<bool> bGui;

private:

	ofParameterGroup params_Internal;
	ofParameter<bool> bActive;
	ofParameter<bool> bKeys;
	ofParameter<bool> bDebug;
	ofParameter<bool> bHelp;
	ofParameter<glm::vec2> position_Gui;

	// gui advanced/add-on
	ofxPanel gui_User;

	// keys
	void keyPressed(ofKeyEventArgs &eventArgs);
	void keyReleased(ofKeyEventArgs &eventArgs);
	void addKeysListeners();
	void removeKeysListeners();

	// settings
	std::string path_GLOBAL;//this is to folder all files to avoid mixing with other add-ons data
	std::string path_Params_AppSettings;

	void loadParams(ofParameterGroup &g, std::string path);
	void saveParams(ofParameterGroup &g, std::string path);

	//----

	// DEVICES
	// 1. Webcam
	// 2. NDI INPUT
	// 3. NDI OUTPUT

private:

	// 1. Webcam

#ifdef USE_WEBCAM

	ofVideoGrabber vidGrabber;
	void setup_Webcam(); // setup webcam from name device nor index
	void setup_Webcam(int index); // setup webcam from device index
	void exit_Webcam(); // store camera device name to xml

public:

	void doRestart_Webcam(); // restart camera using the current index camera

	void draw_Webcam();//-> for internal use
	void draw_Webcam_MiniPreview(bool bInfo = false);
	void draw_Webcam_Full();

private:

	ofParameter<bool> bWebcam;
	ofParameter<bool> bWebcam_Draw;
	std::string webcam_Names_InputDevices;
	ofParameter<std::string> webcam_Name_{ "WEBCAM_DEVICE_NAME", "" };
	ofParameter<std::string> webcam_Name; // can be merged both vars?
	ofParameter<bool> bWebcam_Mini;
	ofParameter<int> webcam_Index_Device;
	std::string path_WebcamSettings;

	ofxInteractiveRect rect_Webcam = { "rect_Webcam" };
	std::string path_rect_Webcam = "_Webcam_Mini";
#endif//USE_WEBCAM

	//--

	// 2. NDI INPUT

#ifdef USE_ofxNDI_IN

	ofParameter<bool> bNDI_Input;
	ofParameter<bool> bNDI_Input_Scan;
	ofParameter<bool> bNDI_Input_Draw;
	ofParameter<bool> bNDI_Input_Mini;
	ofParameter<int> NDI_Input_Index;
	ofParameter<std::string> NDI_Input_Name;
	std::string NDI_INPUT_Names_Devices;

public:

	void draw_NDI_IN();
	void draw_NDI_IN_MiniPreview(bool bInfo = false);
	void draw_NDI_IN_Full();

	void doRefresh_NDI_IN();

private:

	void setup_NDI_IN();
	void draw_InfoDevices();
	ofxNDIreceiver ndiReceiver; // NDI receiver
	ofFbo fbo_NDI_Receiver; // Fbo to receive
	ofTexture ndiReceiveTexture; // Texture to receive
	ofImage ndiReceiveImage; // Image to receive
	ofPixels ndiReceivePixels; // Pixels to receive
	unsigned char *ndiReceiveChars; // unsigned char image array to receive
	unsigned int receiverWidth; // sender width and height needed to receive char pixels
	unsigned int receiverHeight;

	ofxInteractiveRect rect_NDI_IN = { "rect_NDI_IN" };
	std::string path_rect_NDI_IN = "_NDI_In_Mini";

	ofParameterGroup params_NDI_Input{ "NDI INPUT" };

#endif//USE_ofxNDI_IN

	//--

	// 3. NDI OUTPUT

#ifdef USE_ofxNDI_OUT

	ofParameter<bool> bNDI_Output;
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

	ofParameterGroup params_Webcam{ "WEBCAM" };
	ofxInteractiveRect rect_NDI_OUT = { "rect_NDI_OUT" };
	std::string path_rect_NDI_OUT = "_NDI_Out_Mini";

	ofParameterGroup params_NDI_Output{ "NDI OUTPUT" };

#endif//USE_ofxNDI_OUT
};