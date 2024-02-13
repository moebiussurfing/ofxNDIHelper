#pragma once

/*


*/

//--

#define DEFAULT_STARTUP_WAITING_TIME 60 // in frames. 60 = 1 sec

#define USE_ofxNDI_IN

//--

#ifdef USE_ofxNDI_IN

#include "ofMain.h"

#include "ofxNDI.h"
#include "ofxGui.h"
#include "ofxSurfingBoxInteractive.h"
#include "ofxSurfingHelpers.h"
#include "ofxSurfing_ofxGui.h"

#define DEVICES_BY_NAME_INSTEAD_OF_BY_INDEX

class NDI_input
{

public:
	
	bool bLoadedStartupDone = false; // to hide all and waiting startup done to start drawing.
	bool bFoundSendersDone = false; 
	bool bStartupDelayedDone = false;

public:

	NDI_input();
	~NDI_input();

	void setup(string name = "InputToOF");
	void setup_Params();
	void update(ofEventArgs& args); 
	void draw();
	void drawGui();
	void windowResized(int w, int h);
	void exit();
	
private:
	
	void startup();
	void startupDelayed();
	
	void updateWorkaround();//workaround to make sure that settings are loaded fine.

public:

	ofParameterGroup params;//all

	//--------------------------------------------------------------
	ofParameterGroup& getParameters() {
		return params;
	}

	ofParameterGroup params_Settings;
	ofParameterGroup params_Control;

private:

	bool bLabelsInner = true;
	int padLabel = 3;
	int round = 0;

	bool bEnable_PRE = false;


	void Changed(ofAbstractParameter& e);

public:

	void doReset_Mini_PreviewsSize();

	ofParameter<bool> bGui;
	ofParameter<bool> bGui_Preview;
	ofParameter<bool> bGui_Internal;

	ofParameter<bool> bDebug;

	ofParameter<bool> bNext;

	vector<std::string> namesDevices;
	ofParameter<int> scaleMode_Index;
	vector<std::string> scaleMode_Names;

	ofParameter<bool> bLockAspect;

	ofParameter<bool> bReset;
private:

	ofParameter<glm::vec2> position_Gui;

	
	ofParameter<string> scaleMode_Name;
	ofScaleMode scaleMode;
	

	ofParameter<void> bLoad;

public:

	void setDebug(bool b) { bDebug = b; }
	void setMini(bool b) { bDrawMini = b; }
	void setToggleDebug() { bDebug = !bDebug; }
	void setToggleMini() { bDrawMini = !bDrawMini; } // Legacy API
	void setToggleFullScreen() { setToggleMini(); }

private:

	// Text Box
	ofTrueTypeFont font;
	//ofTrueTypeFont fontBig;

	bool bDISABLE_CALLBACKS = true;

public:

	ofxPanel gui_Control;

private:
	
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
	float _padx = 11;
	float _pady = -11;
	float _padx2 = 9;
	float _pady2 = 18;
	int x2, y2;

	void loadSettings();
	void saveSettings();

public:
	
	//--------------------------------------------------------------
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
	//--------------------------------------------------------------
	void setPositionPreview(glm::vec2 pos)
	{
		rect_NDI_IN.setPosition(pos.x, pos.y);
	}

	//--------------------------------------------------------------
	ofRectangle getPreviewRect() {
		return rect_NDI_IN.getRectangle();
	}

	//--

//private:
public:

	// 2. NDI INPUT

	ofParameter<bool> bEnable;
	ofParameter<bool> bScan;
	ofParameter<bool> bDraw;
	ofParameter<bool> bDrawMini;
	ofParameter<int> indexDevice;
	ofParameter<std::string> nameDevice;
	std::string NDI_INPUT_Names_Devices = "NO DEVICES FOUND";

public:

	void draw_Main();
	void draw_MiniPreview();
	void draw_FullScreen();

	void doListDevices();
	//void doRefresh_NDI_IN();
	
	void doNext(); 
	void doScan(); // scan network NDI devices!

	ofxSurfingBoxInteractive rect_NDI_IN;

private:

	int nsenders = 0;
	//bool bEnable_PRE;//workaround

	void setup_ByIndex(int _indexDevice);
	void setup_ByName(string _nameDevice);
	void setup_Fbo();
	void setup_ByIndex();

	void draw_InfoDevices();

	ofxNDIreceiver NDI_Receiver; // NDI receiver
	ofFbo fbo_NDI_Receiver; // Fbo to receive
	ofTexture tex_NDI_Receiver; // Texture to receive
	
	//ofImage ndiReceiveImage; // Image to receive
	//ofPixels ndiReceivePixels; // Pixels to receive
	//unsigned char* ndiReceiveChars; // unsigned char image array to receive
	unsigned int wReceiver; // sender width and height needed to receive char pixels
	unsigned int hReceiver;


	//--

	//TODO:
	//--------------------------------------------------------------
	void doFixer()
	{
		int nsenders = NDI_Receiver.GetSenderCount();
		indexDevice.setMax(nsenders - 1);

		// List all the senders
		if (nsenders > 0)
		{
			ofLogNotice(__FUNCTION__) << "Number of NDI senders found: " << nsenders;

			NDI_INPUT_Names_Devices = "";
			for (int i = 0; i < nsenders; i++)
			{
				string name = NDI_Receiver.GetSenderName(i);
				string str = ofToString(i) + " " + name;
				ofLogNotice(__FUNCTION__) << str;

				NDI_INPUT_Names_Devices += str;
				NDI_INPUT_Names_Devices += "\n";
			}
		}
	}
};

#endif