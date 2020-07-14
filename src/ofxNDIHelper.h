
///---
///
///	TODO:
///	
///	+	looks like sometimes more that expected ndi sources are appearing
///	!	crashes sometimes..
///	+	dynamic resize input/output
///
///---


#pragma once

#include "ofMain.h"

//----

#define USE_WEBCAM		//aux camera
#define USE_ofxNDI_IN	//ndi input
#define USE_ofxNDI_OUT	//ndi out

//----

#ifdef USE_ofxNDI_OUT
#include "ofxNDI.h"
#else
#ifdef USE_ofxNDI_IN
#include "ofxNDI.h"
#endif
#endif

//we can handle many app modes to change behaviour
#define NUM_MODES_APP 2

//dependencies
#include "ofxGui.h"

class ofxNDIHelper : public ofBaseApp
{
public:

	ofTrueTypeFont font;

	//--------------------------------------------------------------
	void drawTextBoxed(string text, int x, int y)
	{
		ofPushStyle();
		int _alpha = 200;//bbox
		int pad = 20;

		if (!font.isLoaded()) {
			ofDrawBitmapStringHighlight(text, x, y);
		}
		else {
			//bbox
			ofSetColor(0, _alpha);
			ofFill();
			ofRectangle _r(font.getStringBoundingBox(text, x, y));
			_r.setWidth(_r.getWidth() + pad);
			_r.setHeight(_r.getHeight() + pad);
			_r.setX(_r.getPosition().x - pad / 2.);
			_r.setY(_r.getPosition().y - pad / 2.);
			ofDrawRectangle(_r);

			//text
			ofSetColor(255, 255);
			ofNoFill();
			font.drawString(text, x, y);
		}

		ofPopStyle();
	}


public:
	//webcam
#ifdef USE_WEBCAM
	ofVideoGrabber vidGrabber;
	void setupWebcam();//setup webcam from name device nor index
	void setupWebcam(int index);//setup webcam from device index
	void restartWebcam();//restart camera using the current index camera
	void exitWebcam();//store camera device name to xml
	void drawWebcam();
	void drawWebcamInfo(int x, int y);
	ofParameter <std::string> _dName{ "WEBCAM_DEVICE_NAME", "" };
	//int _d;
	ofParameter<bool> mini_Webcam;
	ofParameter<int> index_WebcamDevice;
	ofParameter<string> name_Webcam;
	string path_WebcamSettings;
#endif

#ifdef USE_ofxNDI_OUT
public:
	void setup_NDI_OUT();
	ofxNDIsender ndiSender;		//NDI sender object
	ofFbo fbo_NDI_Sender;       //Fbo used for graphics and sending
	unsigned int senderWidth;	//Width of the sender output
	unsigned int senderHeight;	//Height of the sender output
	char senderName[256];		//Sender name
	void begin_NDI_OUT();		//feed the sender
	void end_NDI_OUT();
	void draw_NDI_OUT();
#endif

#ifdef USE_ofxNDI_IN
public:
	void setup_NDI_IN();
	void refresh_NDI_IN();
	void draw_NDI_IN();
	ofxNDIreceiver ndiReceiver;			//NDI receiver
	ofFbo fbo_NDI_Receiver;				//Fbo to receive
	ofTexture ndiReceiveTexture;		//Texture to receive
	ofImage ndiReceiveImage;			//Image to receive
	ofPixels ndiReceivePixels;			//Pixels to receive
	unsigned char *ndiReceiveChars;		//unsigned char image array to receive
	unsigned int receiverWidth;			//sender width and height needed to receive char pixels
	unsigned int receiverHeight;
	void drawInfoDevices();
#endif

	//--

#pragma mark - ADDON ENGINE

public:
	ofParameterGroup params_Control;;
	ofParameter<bool> ENABLE_Addon;
	ofParameter<bool> ENABLE_Edit;
	ofParameter<bool> ENABLE_Webcam;
	ofParameter<bool> ENABLE_Draw_Webcam;

	ofParameter<bool> ENABLE_NDI_Input;
	ofParameter<bool> ENABLE_Draw_NDI_Input;
	ofParameter<bool> mini_ndiInput;
	ofParameter<int> index_NDI_Input;
	ofParameter<string> name_NDI_Input;

	ofParameter<bool> ENABLE_NDI_Output;
	ofParameter<bool> ENABLE_Draw_NDI_Output;
	ofParameter<string> name_NDI_Output;

	string NDI_InputDevices;
	string webcam_InputDevices;

public:
	ofParameterGroup params;

	//----

#pragma mark - OF

	ofxNDIHelper();
	~ofxNDIHelper();

	void setup();
	void update();
	void draw();
	void exit();

	void drawGui();

	//public
	void windowResized(int w, int h);

#pragma mark - API

	void setActive(bool b);
	void setGuiVisible(bool b);
	void setPathGlobal(string s);//must call before setup. disabled by default
	void setLogLevel(ofLogLevel level);
	void setAutoSave(bool b)
	{
		ENABLE_AutoSave = b;
	}

	void setKey_MODE_App(int k);

	//-

private:

	//updating some params before save will trigs also the group callbacks
	//so we disable this callbacks just in case params updatings are required
	//in this case we will need to update gui position param
	bool DISABLE_Callbacks;

	int key_MODE_App = OF_KEY_TAB;//default key to switch MODE_App
	int screenW, screenH;

	//autosave
	ofParameter<bool> ENABLE_AutoSave;
	uint64_t timerLast_Autosave = 0;
	int timeToAutosave = 5000;

	//-

	void Changed_params_Control(ofAbstractParameter &e);

	//-

#pragma mark - ADDON TEMPLATE STUFF

#pragma mark - CONTROL PARAMS

	//control params
	ofParameterGroup params_Internal;
	ofParameter<bool> MODE_Active;
	ofParameter<bool> ENABLE_keys;
	ofParameter<bool> ENABLE_Debug;
	ofParameter<bool> SHOW_Gui;
	ofParameter<glm::vec2> Gui_Position;
	ofParameter<bool> SHOW_Help;
	ofParameter<int> MODE_App;
	ofParameter<string> MODE_App_Name;
	ofxPanel gui_Control;

#pragma mark - CALLBACKS

	void Changed_params_Internal(ofAbstractParameter &e);
	void Changed_params(ofAbstractParameter &e);

#pragma mark - OF LISTENERS

	//keys
	void keyPressed(ofKeyEventArgs &eventArgs);
	void keyReleased(ofKeyEventArgs &eventArgs);
	void addKeysListeners();
	void removeKeysListeners();

	//mouse
	void mouseDragged(ofMouseEventArgs &eventArgs);
	void mousePressed(ofMouseEventArgs &eventArgs);
	void mouseReleased(ofMouseEventArgs &eventArgs);
	void addMouseListeners();
	void removeMouseListeners();

#pragma mark - FILE SETTINGS
	//settings
	string path_GLOBAL;//this is to folder all files to avoid mixing with other addons data
	string path_Params_Internal;
	string path_Params_Control;
	void loadParams(ofParameterGroup &g, string path);
	void saveParams(ofParameterGroup &g, string path);

	//--

private:
	//--------------------------------------------------------------
	void CheckFolder(string _path)
	{
		ofLogNotice(__FUNCTION__) << _path;

		ofDirectory dataDirectory(ofToDataPath(_path, true));

		//check if folder path exist
		if (!dataDirectory.isDirectory())
		{
			ofLogError(__FUNCTION__) << "FOLDER NOT FOUND! TRYING TO CREATE...";

			//try to create folder
			bool b = dataDirectory.createDirectory(ofToDataPath(_path, true));

			//debug if creation has been succeded
			if (b) ofLogNotice(__FUNCTION__) << "CREATED '" << _path << "'  SUCCESSFULLY!";
			else ofLogError(__FUNCTION__) << "UNABLE TO CREATE '" << _path << "' FOLDER!";
		}
		else
		{
			ofLogNotice(__FUNCTION__) << "OK! LOCATED FOLDER: '" << _path << "'";//nothing to do
		}
	}
};