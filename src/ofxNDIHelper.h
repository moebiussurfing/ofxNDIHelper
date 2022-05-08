
//---

/*

	TODO:

	+ 
	+ split webcam part as a new helper addon.
	+ ndi input port settings as names not index port.

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

//--

#ifdef USE_ofxNDI_OUT || USE_ofxNDI_IN
#include "ofxNDI.h"
#endif

// dependencies
#include "ofxGui.h"
#include "ofxInteractiveRect.h"
#include "ofxSurfingHelpers.h"
#include "ofxSurfing_ofxGui.h"

//--

class ofxNDIHelper : public ofBaseApp
{

public:

	ofxNDIHelper();
	~ofxNDIHelper();

	void setup();
	void setup_Params();
	void startup();
	void update();
	void draw();
	void exit();
	void draw_Gui();
	void windowResized(int w, int h);

	void setActive(bool b);
	void setGuiVisible(bool b);
	void setGuiToggleVisible();
	void setPathGlobal(std::string s);
	void setLogLevel(ofLogLevel level);
	//--------------------------------------------------------------
	void setAutoSave(bool b)
	{
		bAutoSave = b;
	}

	//--

private:

	ofParameterGroup params_Webcam{ "WEBCAM" };
	ofParameterGroup params_NDI_Input{ "NDI INPUT" };
	ofParameterGroup params_NDI_Output{ "NDI OUTPUT" };

	//--------------------------------------------------------------
	void fixStartup()
	{
		bEdit = bEdit_PRE;

		NDI_Input_Index = NDI_Input_Index;
	}

private:

	// webcam

#ifdef USE_WEBCAM
	ofVideoGrabber vidGrabber;
	void setup_Webcam();//setup webcam from name device nor index
	void setup_Webcam(int index);//setup webcam from device index
	void restart_Webcam();//restart camera using the current index camera
	void exit_Webcam();//store camera device name to xml

public:

	void draw_Webcam_MiniPreview(bool bInfo = false);
	void draw_Webcam_Full();
	
private:
	
	ofParameter <std::string> _webcam_Name{ "WEBCAM_DEVICE_NAME", "" };
	ofParameter<std::string> webcam_Name;//can be merged both vars?
	ofParameter<bool> webcam_Mini;
	ofParameter<int> webcam_Index_Device;
	std::string path_WebcamSettings;
#endif

#ifdef USE_ofxNDI_OUT
private:

	void setup_NDI_OUT();
	ofxNDIsender ndiSender;		//NDI sender object
	ofFbo fbo_NDI_Sender;       //Fbo used for graphics and sending
	unsigned int senderWidth;	//Width of the sender output
	unsigned int senderHeight;	//Height of the sender output
	char senderName[256];		//Sender name

public:

	// API

	void begin_NDI_OUT(); // feed the sender
	void end_NDI_OUT();

	//--

//private:
public:

	void draw_NDI_OUT();

	void draw_NDI_OUT_MiniPreview(bool bInfo = false);
	void draw_NDI_OUT_Full();

#endif

	//--

#ifdef USE_ofxNDI_IN

//private:
public:

	void draw_NDI_IN();

	void draw_NDI_IN_MiniPreview(bool bInfo = false);
	void draw_NDI_IN_Full();

private:

	void setup_NDI_IN();
	void refresh_NDI_IN();
	ofxNDIreceiver ndiReceiver;			//NDI receiver
	ofFbo fbo_NDI_Receiver;				//Fbo to receive
	ofTexture ndiReceiveTexture;		//Texture to receive
	ofImage ndiReceiveImage;			//Image to receive
	ofPixels ndiReceivePixels;			//Pixels to receive
	unsigned char *ndiReceiveChars;		//unsigned char image array to receive
	unsigned int receiverWidth;			//sender width and height needed to receive char pixels
	unsigned int receiverHeight;
	void draw_InfoDevices();

#endif

	//--

private:

	ofParameterGroup params_AppsSettings;

	ofParameter<bool> bEdit;
	bool bEdit_PRE;
	ofParameter<bool> bWebcam;
	ofParameter<bool> bWebcam_Draw;

	ofParameter<bool> bNDI_Input;
	ofParameter<bool> bNDI_Input_Scan;
	ofParameter<bool> bNDI_Input_Draw;
	ofParameter<bool> bNDI_Input_Mini;
	ofParameter<int> NDI_Input_Index;
	ofParameter<std::string> NDI_Input_Name;

	ofParameter<bool> bNDI_Output;
	ofParameter<bool> bNDI_Output_Draw;
	ofParameter<bool> bNDI_Output_Mini;
	ofParameter<std::string> NDI_Output_Name;

	std::string name_NDI_InputDevices;
	std::string name_Webcam_InputDevices;
	bool bDoRestartup = false;

public:

	ofParameterGroup params_User;

	//--

private:

	// mini preview rectangles positions and sizes
	ofxInteractiveRect rect_NDI_IN = { "rect_NDI_IN" };
	ofxInteractiveRect rect_NDI_OUT = { "rect_NDI_OUT" };
	ofxInteractiveRect rect_Webcam = { "rect_Webcam" };
	std::string path_rect_NDI_IN = "_NDI_In_Mini";
	std::string path_rect_NDI_OUT = "_NDI_Out_Mini";
	std::string path_rect_Webcam = "_Webcam_Mini";
	ofParameter<bool> bLockRatio;
	ofParameter<bool> bReset;

	void reset_Mini_Previews();

	// default layout
	int xPadPreview = 300;
	int yPadPreview = 50;
	float wPreview = 320;
	//preview viewport size

	// Text box
	ofTrueTypeFont font;
	ofTrueTypeFont fontBig;
	float rounded = 2.0;
	int pad = 20;
	
	float _padx = 9;
	float _pady = 9 - pad;

	//-

private:

	// updating some params before save will trigs also the group callbacks
	// so we disable this callbacks just in case params updating's are required
	// in this case we will need to update gui position param
	bool bDISABLECALLBACKS;

	//int key_MODE_App = OF_KEY_TAB;//default key to switch MODE_App
	int screenW, screenH;

	// autosave
	ofParameter<bool> bAutoSave;
	uint64_t timerLast_Autosave = 0;
	int timeToAutosave = 5000;

	//-

	void Changed_Params_AppSettings(ofAbstractParameter &e);

	//-

	// control params

public:

	ofParameter<bool> bGui_Controls;
	ofParameter<bool> bGui;

private:

	ofParameterGroup params_Internal;
	ofParameter<bool> bActive;
	ofParameter<bool> bKeys;
	ofParameter<bool> bDebug;
	ofParameter<glm::vec2> position_Gui;
	ofParameter<bool> bHelp;

	//ofParameter<int> MODE_App;
	//ofParameter<std::string> MODE_App_Name;

	ofxPanel gui_User;

	// keys
	void keyPressed(ofKeyEventArgs &eventArgs);
	void keyReleased(ofKeyEventArgs &eventArgs);
	void addKeysListeners();
	void removeKeysListeners();

	// mouse
	void mouseDragged(ofMouseEventArgs &eventArgs);
	void mousePressed(ofMouseEventArgs &eventArgs);
	void mouseReleased(ofMouseEventArgs &eventArgs);
	void addMouseListeners();
	void removeMouseListeners();

	// settings
	std::string path_GLOBAL;//this is to folder all files to avoid mixing with other addons data
	std::string path_Params_AppSettings;

	void loadParams(ofParameterGroup &g, std::string path);
	void saveParams(ofParameterGroup &g, std::string path);
};