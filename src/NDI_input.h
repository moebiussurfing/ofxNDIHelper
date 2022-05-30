#pragma once

#define USE_ofxNDI_IN

//--

#ifdef USE_ofxNDI_IN

#include "ofMain.h"
#include "ofxNDI.h"
#include "ofxGui.h"
#include "ofxSurfingBoxInteractive.h"
#include "ofxSurfingHelpers.h"


#define DEVICES_BY_NAME_INSTEAD_OF_BY_INDEX

class NDI_input
{

public:

	NDI_input();
	~NDI_input();

	void setup(string name = "myInput");
	void setup_Params();
	void draw();
	void drawGui();
	void windowResized(int w, int h);
	void exit();
	void startup();

	ofParameterGroup params;

private:

	ofParameterGroup params_Control;

	void Changed(ofAbstractParameter& e);
	void doReset_Mini_Previews();

public:

	ofParameter<bool> bGui_Preview;
	ofParameter<bool> bGui_Internal;

	//void setEdit(bool b) { bEdit = b; }

public:

	ofParameter<glm::vec2> position_Gui;

	ofParameter<bool> bLockRatio;
	ofParameter<bool> bDebug;
	//ofParameter<bool> bEdit;
	ofParameter<bool> bReset;

private:

	// Text Box
	ofTrueTypeFont font;
	ofTrueTypeFont fontBig;

	bool bDISABLECALLBACKS = true;

	ofxPanel gui_Control;

	std::string name;

	std::string path_GLOBAL = "/";//this is to folder all files to avoid mixing with other add-ons data
	std::string path_Params_AppSettings;
	std::string path_rect_NDI_IN;

	int screenW, screenH;

	int xPadPreview = 300;
	int yPadPreview = 100;
	float rounded = 2.0;
	int pad = 20;
	float wPreview = 320;
	float _padx = 9;
	float _pady = -11;
	float _padx2 = 9;
	float _pady2 = 18;
	int x2, y2;

	void loadSettings();
	void saveSettings();

public:
	
	void setName(string s)
	{
		name = s;
	}

	//--------------------------------------------------------------
	void setPathGlobal(string s) // Path for settings file. Must call before setup.
	{
		path_GLOBAL = s;

		ofxSurfingHelpers::CheckFolder(path_GLOBAL);

		//path_rect_NDI_IN = "NDI_IN_" + name + "_Mini";
		//rect_NDI_IN.setPathGlobal(path_GLOBAL + path_rect_NDI_IN);
	}
	//--------------------------------------------------------------
	void setPositionGui(glm::vec2 pos)
	{
		gui_Control.setPosition(pos.x, pos.y);
	}

	//--

private:

	// 2. NDI INPUT

	ofParameter<bool> bEnable;
	ofParameter<bool> bScan;
	ofParameter<bool> bDraw;
	ofParameter<bool> bDrawMini;
	ofParameter<int> indexDevice;
	ofParameter<std::string> nameDevice;
	std::string NDI_INPUT_Names_Devices;

public:

	void draw_NDI_IN();
	void draw_NDI_IN_MiniPreview(bool bInfo = false);
	void draw_NDI_IN_Full();

	void doRefresh_NDI_IN();
	
	void doScan(); // scan network NDI devices!

private:


	void setup_NDI_IN_ByIndex(int deviceIndex);
	void setup_NDI_IN_ByName(string deviceName);
	void setup_NDI_IN();
	void draw_InfoDevices();
	ofxNDIreceiver ndiReceiver; // NDI receiver
	ofFbo fbo_NDI_Receiver; // Fbo to receive
	ofTexture ndiReceiveTexture; // Texture to receive
	ofImage ndiReceiveImage; // Image to receive
	ofPixels ndiReceivePixels; // Pixels to receive
	unsigned char* ndiReceiveChars; // unsigned char image array to receive
	unsigned int receiverWidth; // sender width and height needed to receive char pixels
	unsigned int receiverHeight;

	ofxSurfingBoxInteractive rect_NDI_IN;

	ofParameterGroup params_NDI_Input{ "NDI INPUT" };

};

#endif