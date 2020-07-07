#include "ofxNDIHelper.h"

#pragma mark - OF

//--------------------------------------------------------------
ofxNDIHelper::ofxNDIHelper()
{
	//path for settings
	setPathGlobal("ofxNDIHelper/");
	path_Params_Control = "params_Control.xml";

	setActive(true);//add key and mouse listeners
}

//--------------------------------------------------------------
void ofxNDIHelper::beginNDI_IN()
{
}

//--------------------------------------------------------------
void ofxNDIHelper::endNDI_IN()
{
}

//--------------------------------------------------------------
void ofxNDIHelper::beginNDI_OUT()
{
	//Draw graphics into an fbo for the example
	ndiSender_Fbo.begin();
}

//--------------------------------------------------------------
void ofxNDIHelper::endNDI_OUT()
{
	ndiSender_Fbo.end();

	//--

	//Draw the fbo result fitted to the display window
	//ndiSender_Fbo.draw(0, 0, ofGetWidth(), ofGetHeight());

	ndiSender.SendImage(ndiSender_Fbo, false);//send processed
	//ndiSender.SendImage(vidGrabber.getPixels());//send clean webcam


	//--

	//draw too
	ndiSender_Fbo.draw(0, 0);

}

//--------------------------------------------------------------
void ofxNDIHelper::setup()
{
	//log mode
	ofSetLogLevel("ofxNDIHelper", OF_LOG_NOTICE);
	//ofSetLogLevel("ofxNDIHelper", OF_LOG_SILENT);

	//--

	//window
	screenW = ofGetWidth();
	screenH = ofGetHeight();

	//--

	//params control (addon variables)
	Addon_Active.set("ADDON ACTIVE", true);
	Addon_Float.set("ADDON float", 0, 0, 1);
	//group
	params_Addon.setName("ADDON");
	params_Addon.add(Addon_Active);
	params_Addon.add(Addon_Float);

	//callback
	ofAddListener(params_Addon.parameterChangedE(), this, &ofxNDIHelper::Changed_params_Addon);

	//-

	//addon control(internal)

	//params
	MODE_Active.set("ACTIVE", true);
	ENABLE_keys.set("KEYS", true);
	SHOW_Help.set("HELP", false);
	MODE_App.set("APP MODE", 0, 0, NUM_MODES_APP - 1);
	MODE_App_Name.set("", "");
	MODE_App_Name.setSerializable(false);
	ENABLE_Debug.set("DEBUG", true);
	ENABLE_AutoSave.set("AUTO SAVE", true);
	SHOW_Gui.set("GUI", true);
	Gui_Position.set("GUI POSITION",
		glm::vec2(screenW * 0.5, screenH * 0.5),
		glm::vec2(0, 0),
		glm::vec2(screenW, screenH)
	);

	//params control (internal)
	params_Control.setName("INTERNAL");
	params_Control.add(MODE_App);
	params_Control.add(MODE_App_Name);
	params_Control.add(MODE_Active);
	params_Control.add(ENABLE_keys);
	params_Control.add(SHOW_Help);
	params_Control.add(ENABLE_Debug);
	params_Control.add(ENABLE_AutoSave);
	params_Control.add(SHOW_Gui);
	params_Control.add(Gui_Position);

	//callback
	ofAddListener(params_Control.parameterChangedE(), this, &ofxNDIHelper::Changed_params_Control);

	//-

	//all params
	params.setName("ALL PARAMS");
	params.add(params_Addon);
	params.add(params_Control);

	//callback
	ofAddListener(params.parameterChangedE(), this, &ofxNDIHelper::Changed_params);

	//--

	//gui

	//theme
	string str = "fonts/overpass-mono-bold.otf";
	ofxGuiSetFont(path_GLOBAL + str, 9);
	ofxGuiSetDefaultHeight(20);
	ofxGuiSetBorderColor(32);
	ofxGuiSetFillColor(ofColor(48));
	ofxGuiSetTextColor(ofColor::white);
	ofxGuiSetHeaderColor(ofColor(24));
	//ofxGuiSetBackgroundColor(ofColor::black);

	//setup gui
	gui_Control.setup("ofxNDIHelper");
	gui_Control.add(params);//add control (internal) and addon params

	//collapse groups
	auto &g0 = gui_Control.getGroup("ALL PARAMS");//1st level
	auto &g2 = g0.getGroup("ADDON");//2nd level
	auto &g3 = g0.getGroup("INTERNAL");//2nd level
	auto &g31 = g3.getGroup("GUI POSITION");//3nd level
	//g0.minimize();
	//g2.minimize();
	g3.minimize();
	g31.minimize();

	//----

	//sources / modules
#ifdef USE_WEBCAM
	setupWebcam();
#endif

#ifdef USE_ofxNDI_OUT
	setupNDI_OUT();
#endif

#ifdef USE_ofxNDI_IN
	setupNDI_IN();
#endif

	//-----

	//settings
	DISABLE_Callbacks = true;

	//--

	//startup

	ofLogNotice(__FUNCTION__) << "STARTUP INIT";

	DISABLE_Callbacks = false;
	MODE_Active = true;

	loadParams(params_Control, path_GLOBAL + path_Params_Control);

	//set gui position after window setup/resizing
	windowResized(screenW, screenH);

	//-
}

//--------------------------------------------------------------
void ofxNDIHelper::update()
{
#ifdef USE_WEBCAM
	vidGrabber.update();
#endif

	//--

//autosave
//ENABLE_AutoSave = false;
	if (ENABLE_AutoSave && ofGetElapsedTimeMillis() - timerLast_Autosave > timeToAutosave)
	{
		DISABLE_Callbacks = true;
		//get gui position before save
		Gui_Position = glm::vec2(gui_Control.getPosition());
		saveParams(params_Control, path_GLOBAL + path_Params_Control);
		DISABLE_Callbacks = false;

		timerLast_Autosave = ofGetElapsedTimeMillis();
		//ofLogNotice(__FUNCTION__) << "Autosaved DONE";
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::drawGui()
{	
//	//1
//#ifdef USE_WEBCAM
//	drawWebcam();
//#endif

	if (SHOW_Gui)
	{
		//edit mode
		if (MODE_App == 1)
		{
			//controlPoints.draw();
		}

		gui_Control.draw();

#ifdef USE_ofxNDI_IN
		// Show what it is receiving
		ShowInfo();
#endif
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::exit()
{
	// The sender must be released 
	// or NDI sender discovery will still find it
	ndiSender.ReleaseSender();

	//get gui position before save
	Gui_Position = glm::vec2(gui_Control.getPosition());

	saveParams(params_Control, path_GLOBAL + path_Params_Control);

}

//--------------------------------------------------------------
ofxNDIHelper::~ofxNDIHelper()
{
	setActive(false);//remove keys and mouse listeners

	//remove params callbacks listeners
	ofRemoveListener(params.parameterChangedE(), this, &ofxNDIHelper::Changed_params);
	ofRemoveListener(params_Control.parameterChangedE(), this, &ofxNDIHelper::Changed_params_Control);
	ofRemoveListener(params_Addon.parameterChangedE(), this, &ofxNDIHelper::Changed_params_Addon);

	//exit();
}

//--------------------------------------------------------------
void ofxNDIHelper::setLogLevel(ofLogLevel level)
{
	ofSetLogLevel("ofxNDIHelper", level);
}


#pragma mark - OF LISTENERS

//--------------------------------------------------------------
void ofxNDIHelper::windowResized(int w, int h)
{
	screenW = w;
	screenH = h;

	//-

	//user gui deopending on window dimensions
	//gui_Control.setPosition(screenW * 0.5 - 200, screenH - 200);
}

//keys
//--------------------------------------------------------------
void ofxNDIHelper::keyPressed(ofKeyEventArgs &eventArgs)
{
	const int &key = eventArgs.key;
	ofLogNotice(__FUNCTION__) << "keyPressed: " << (char)key << " [" << key << "]";

	//modifiers
	bool mod_COMMAND = eventArgs.hasModifier(OF_KEY_COMMAND);
	bool mod_CONTROL = eventArgs.hasModifier(OF_KEY_CONTROL);
	bool mod_ALT = eventArgs.hasModifier(OF_KEY_ALT);
	bool mod_SHIFT = eventArgs.hasModifier(OF_KEY_SHIFT);

	bool debug = false;
	if (debug)
	{
		ofLogNotice(__FUNCTION__) << "mod_COMMAND: " << (mod_COMMAND ? "ON" : "OFF");
		ofLogNotice(__FUNCTION__) << "mod_CONTROL: " << (mod_CONTROL ? "ON" : "OFF");
		ofLogNotice(__FUNCTION__) << "mod_ALT: " << (mod_ALT ? "ON" : "OFF");
		ofLogNotice(__FUNCTION__) << "mod_SHIFT: " << (mod_SHIFT ? "ON" : "OFF");
	}

	//-

	//disabler for all keys. (independent from MODE_Active)
	if (ENABLE_keys)
	{
		//custom
		if (key == ' ')
		{
		}
		else if (key == ' ')
		{
		}

		//custom with modifiers
		if (key == OF_KEY_UP && mod_ALT)
		{
			ofLogNotice(__FUNCTION__) << "";
		}
		else if (key == OF_KEY_UP)
		{
			ofLogNotice(__FUNCTION__) << "";
		}

		//general
		if (key == key_MODE_App)
		{
			int i = MODE_App;
			i++;
			MODE_App = i % NUM_MODES_APP;


		}
		else if (key == 'g')
		{
			SHOW_Gui = !SHOW_Gui;
		}
		else if (key == 'h')
		{
			SHOW_Help = !SHOW_Help;
		}
		else if (key == 'd')
		{
			ENABLE_Debug = !ENABLE_Debug;
		}

		//ndi
#ifdef USE_ofxNDI_IN
		{
			char name[256];
			int index = key - 48;

			int nsenders = ndiReceiver.GetSenderCount();

			if (key == ' ') {
				// List all the senders
				if (nsenders > 0) {
					ofLogNotice(__FUNCTION__) << "Number of NDI senders found: " << nsenders ;
					for (int i = 0; i < nsenders; i++) {
						ndiReceiver.GetSenderName(name, 256, i);
						ofLogNotice(__FUNCTION__) << "    Sender " << i << " [" << name << "]" ;
					}
					if (nsenders > 1)
						ofLogNotice(__FUNCTION__) << "Press key [0] to [" << nsenders - 1 << "] to select a sender" ;
				}
				else
					ofLogNotice(__FUNCTION__) << "No NDI senders found" ;
			}
			else if (nsenders > 0 && index >= 0 && index < nsenders) {
				// Update the receiver with the returned index
				// Returns false if the current sender is selected
				if (ndiReceiver.SetSenderIndex(index))
					ofLogNotice(__FUNCTION__) << "Selected [" << ndiReceiver.GetSenderName(index) << "]" ;
				else
					ofLogNotice(__FUNCTION__) << "Same sender" ;
			}
		}
#endif
	}

	//--

	//key enabler
	if (key == 'k')
	{
		ENABLE_keys = !ENABLE_keys;
		ofLogNotice(__FUNCTION__) << "KEYS: " << (ENABLE_keys ? "ON" : "OFF");

		if (!ENABLE_keys)
		{
			ofLogNotice(__FUNCTION__) << "ALL KEYS DISABLED. PRESS 'k' TO ENABLE GAIN!";
		}
		else
		{
			ofLogNotice(__FUNCTION__) << "KEYS ENABLED BACK";
		}
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::keyReleased(ofKeyEventArgs &eventArgs)
{
	const int &key = eventArgs.key;
	ofLogNotice(__FUNCTION__) << "keyPressed: " << (char)key << " [" << key << "]";

	bool mod_COMMAND = eventArgs.hasModifier(OF_KEY_COMMAND);
	bool mod_CONTROL = eventArgs.hasModifier(OF_KEY_CONTROL);
	bool mod_ALT = eventArgs.hasModifier(OF_KEY_ALT);
	bool mod_SHIFT = eventArgs.hasModifier(OF_KEY_SHIFT);
}

//--------------------------------------------------------------
void ofxNDIHelper::addKeysListeners()
{
	ofAddListener(ofEvents().keyPressed, this, &ofxNDIHelper::keyPressed);
}

//--------------------------------------------------------------
void ofxNDIHelper::removeKeysListeners()
{
	ofRemoveListener(ofEvents().keyPressed, this, &ofxNDIHelper::keyPressed);
}

//mouse
//--------------------------------------------------------------
void ofxNDIHelper::mouseDragged(ofMouseEventArgs &eventArgs)
{
	const int &x = eventArgs.x;
	const int &y = eventArgs.y;
	const int &button = eventArgs.button;
	//ofLogNotice(__FUNCTION__) << "mouseDragged " << x << ", " << y << ", " << button;
}

//--------------------------------------------------------------
void ofxNDIHelper::mousePressed(ofMouseEventArgs &eventArgs)
{
	const int &x = eventArgs.x;
	const int &y = eventArgs.y;
	const int &button = eventArgs.button;
	//ofLogNotice(__FUNCTION__) << "mousePressed " << x << ", " << y << ", " << button;
}

//--------------------------------------------------------------
void ofxNDIHelper::mouseReleased(ofMouseEventArgs &eventArgs)
{
	const int &x = eventArgs.x;
	const int &y = eventArgs.y;
	const int &button = eventArgs.button;
	//ofLogNotice(__FUNCTION__) << "mouseReleased " << x << ", " << y << ", " << button;
}

//--------------------------------------------------------------
void ofxNDIHelper::addMouseListeners()
{
	ofAddListener(ofEvents().mouseDragged, this, &ofxNDIHelper::mouseDragged);
	ofAddListener(ofEvents().mousePressed, this, &ofxNDIHelper::mousePressed);
	ofAddListener(ofEvents().mouseReleased, this, &ofxNDIHelper::mouseReleased);
}

//--------------------------------------------------------------
void ofxNDIHelper::removeMouseListeners()
{
	ofRemoveListener(ofEvents().keyPressed, this, &ofxNDIHelper::keyPressed);
}


#pragma mark - API

//--------------------------------------------------------------
void ofxNDIHelper::setActive(bool b)
{
	//* disables all keys and mouse interaction listeners from the addon

	MODE_Active = b;
	if (!b)
	{
		removeKeysListeners();
		removeMouseListeners();
	}
	else
	{
		addKeysListeners();
		addMouseListeners();
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::setGuiVisible(bool b)
{
	SHOW_Gui = b;
}


#pragma mark - CALLBACKS

//all params
//--------------------------------------------------------------
void ofxNDIHelper::Changed_params(ofAbstractParameter &e)
{
	//if (!DISABLE_Callbacks)
	//{
	//	string name = e.getName();

	//	//exclude debugs
	//	if (name != "exclude"
	//		&& name != "exclude")
	//	{
	//		ofLogNotice(__FUNCTION__) << "Changed_params: " << name << " : " << e;

	//	}

	//	//params
	//	if (name == "")
	//	{
	//	}
	//}
}

//addon engine params
//--------------------------------------------------------------
void ofxNDIHelper::Changed_params_Addon(ofAbstractParameter &e)
{
	if (!DISABLE_Callbacks)
	{
		string name = e.getName();

		//exclude debugs
		if (name != "exclude"
			&& name != "exclude")
		{
			ofLogNotice(__FUNCTION__) << "Changed_params_Addon: " << name << " : " << e;

		}

		//params
		if (name == "")
		{
		}
	}
}

//addon control (internal) params
//--------------------------------------------------------------
void ofxNDIHelper::Changed_params_Control(ofAbstractParameter &e)
{
	if (!DISABLE_Callbacks)
	{
		string name = e.getName();

		//exclude debugs
		if (name != "exclude"
			&& name != "exclude")
		{
			ofLogNotice(__FUNCTION__) << "Changed_params_Control: " << name << " : " << e;

		}

		//control params
		if (name == "")
		{
		}
		else if (name == "APP MODE")
		{
			switch (MODE_App)
			{
			case 0:
				MODE_App_Name = "RUN";
				//setActive(false);
				break;
			case 1:
				MODE_App_Name = "EDIT";
				//setActive(true);
				break;
			default:
				MODE_App_Name = "UNKNOWN";
				break;
			}
		}

		//filter params
		if (name == "GUI POSITION")
		{
			gui_Control.setPosition(Gui_Position.get().x, Gui_Position.get().y);
		}
		else if (name == "ACTIVE")
		{
			setActive(MODE_Active);
		}
		else if (name == "GUI")
		{
		}
		else if (name == "HELP")
		{
		}
		else if (name == "APP MODE")
		{
		}
		else if (name == "DEBUG")
		{
		}
	}

}

//--------------------------------------------------------------
void ofxNDIHelper::setKey_MODE_App(int k)
{
	key_MODE_App = k;
}


#pragma mark - FILE SETTINGS

//--------------------------------------------------------------
void ofxNDIHelper::setPathGlobal(string s)//must call before setup. disabled by default
{
	path_GLOBAL = s;
}

//--------------------------------------------------------------
void ofxNDIHelper::loadParams(ofParameterGroup &g, string path)
{
	ofLogNotice(__FUNCTION__) << "loadParams: " << path;
	ofXml settings;
	settings.load(path);
	ofDeserialize(settings, g);
}

//--------------------------------------------------------------
void ofxNDIHelper::saveParams(ofParameterGroup &g, string path)
{
	ofLogNotice(__FUNCTION__) << "saveParams: " << path;
	ofXml settings;
	ofSerialize(settings, g);
	settings.save(path);
}

#ifdef USE_WEBCAM
//--------------------------------------------------------------
void ofxNDIHelper::setupWebcam() {
	// Set up webcam
	int _d = 0;
	vidGrabber.setDeviceID(_d); // The first webcam
	//vidGrabber.setup();
	vidGrabber.setup(1920, 1080); 
	//vidGrabber.setup(640, 480); 
}

//--------------------------------------------------------------
void ofxNDIHelper::drawWebcam() {
	//float _ratioSource = vidGrabber.getWidth() / vidGrabber.getHeight();
	//float _ratioScreen = ofGetWidth() / (float)ofGetHeight();
	//float _ratio = _ratioScreen / _ratioSource;
	//vidGrabber.draw(0, 0, ofGetHeight() * _ratio, ofGetHeight());
	vidGrabber.draw(0, 0, ofGetWidth(), ofGetHeight());
}
#endif

#ifdef USE_ofxNDI_OUT
//--------------------------------------------------------------
void ofxNDIHelper::setupNDI_OUT() {


	//-

	ofLogNotice(__FUNCTION__) << ndiSender.GetNDIversion() << " (http://ndi.tv/)" ;

	strcpy_s(senderName, 256, "Openframeworks NDI Sender"); // Set the sender name

	// Set the dimensions of the sender output here
	// This is independent of the size of the display window
	senderWidth = 1920;
	senderHeight = 1080;

	// Create an RGBA fbo for collection of data
	ndiSender_Fbo.allocate(senderWidth, senderHeight, GL_RGBA);

	// Optionally set fbo readback using OpenGL pixel buffers
	ndiSender.SetReadback(); // Change to false to compare

	// Set NDI asynchronous sending for best performance
	ndiSender.SetAsync();

	//// Create a new sender - default RGBA for ofPixels
	//ndiSender.CreateSender("Openframeworks NDI webcam", (unsigned int)vidGrabber.getWidth(), (unsigned int)vidGrabber.getHeight());
	//ofLogNotice(__FUNCTION__) << "Created NDI sender [Openframeworks NDI webcam] (" << vidGrabber.getWidth() << "x" << vidGrabber.getHeight() << ")" ;

	// Create a sender with default RGBA output format
	ndiSender.CreateSender(senderName, senderWidth, senderHeight);
	ofLogNotice(__FUNCTION__) << "Created NDI sender [" << senderName << "] (" << senderWidth << "x" << senderHeight << ")" ;
}
#endif


#ifdef USE_ofxNDI_IN

//--------------------------------------------------------------
void ofxNDIHelper::ShowInfo() {

	char str[256];

	int nsenders = ndiReceiver.GetSenderCount();
	if (nsenders > 0) {

		if (ndiReceiver.ReceiverCreated()) {
			if (ndiReceiver.ReceiverConnected()) {
				// Show received sender information and received fps
				sprintf_s(str, 256, "[%s] (%dx%d/%4.2fp) (fps %2.0f)", ndiReceiver.GetSenderName().c_str(), ndiReceiver.GetSenderWidth(), ndiReceiver.GetSenderHeight(), ndiReceiver.GetSenderFps(), ndiReceiver.GetFps());
			}
			else {
				// Nothing received
				sprintf_s(str, 256, "Connecting to [%s]", ndiReceiver.GetSenderName().c_str());
			}
		}
		ofDrawBitmapString(str, 20, 30);

		if (nsenders == 1) {
			ofDrawBitmapString("1 network source", 20, ofGetHeight() - 20);
		}
		else {
			sprintf_s(str, 256, "%d network sources", nsenders);
			ofDrawBitmapString(str, 20, ofGetHeight() - 40);
			ofDrawBitmapString("'SPACE' to list senders", 20, ofGetHeight() - 20);
		}
	}
	else {
		ofDrawBitmapString("Connecting . . .", 20, 30);
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::setupNDI_IN() {

	//ofBackground(0);
	//ofSetColor(255);

	// Set the window title to show that it is a receiver
	ofSetWindowTitle("Openframeworks NDI receiver");

#ifdef _WIN64
	ofLogNotice(__FUNCTION__) << "\nofxNDI example receiver - 64 bit" ;
#else // _WIN64
	ofLogNotice(__FUNCTION__) << "\nofxNDI example receiver - 32 bit" ;
#endif // _WIN64

	ofLogNotice(__FUNCTION__) << ndiReceiver.GetNDIversion() << " (http://ndi.tv/)" ;
	ofLogNotice(__FUNCTION__) << "Press 'SPACE' to list NDI senders" ;

	// ofFbo
	ndiFbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);

	// Clear the fbo so the first frame draw is black
	ndiFbo.begin();
	ofClear(0, 0, 0, 0);
	ndiFbo.end();

	// ofTexture
	ndiTexture.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);

	// ofImage
	ndiImage.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR_ALPHA);

	// ofPixels
	ndiPixels.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR_ALPHA);

	// unsigned char pixels
	ndiChars = new unsigned char[senderWidth*senderHeight * 4];

	// Sender dimensions and fps are not known yet
	senderWidth = (unsigned char)ofGetWidth();
	senderHeight = (unsigned char)ofGetHeight();

}

//--------------------------------------------------------------
void ofxNDIHelper::drawNDI_IN() {

	// Receive ofTexture
	ndiReceiver.ReceiveImage(ndiTexture);
	ndiTexture.draw(0, 0, ofGetWidth(), ofGetHeight());

	// Receive ofFbo
	// ndiReceiver.ReceiveImage(ndiFbo);
	// ndiFbo.draw(0, 0, ofGetWidth(), ofGetHeight());

	// Receive ofImage
	// ndiReceiver.ReceiveImage(ndiImage);
	// ndiImage.draw(0, 0, ofGetWidth(), ofGetHeight());

	// Receive ofPixels
	// ndiReceiver.ReceiveImage(ndiPixels);
	// ndiImage.setFromPixels(ndiPixels);
	// ndiImage.draw(0, 0, ofGetWidth(), ofGetHeight());

	/*
	// Receive unsigned char pixel image
	// ndiChars is the buffer to receive the pixels
	// buffer size must be managed if there is a sender size change
	unsigned int width = (unsigned int)ofGetWidth();
	unsigned int height = (unsigned int)ofGetHeight();
	if (ndiReceiver.ReceiveImage(ndiChars, width, height)) {
		if (width != senderWidth || height != senderHeight) {
			// Update sender dimensions
			senderWidth = width;
			senderHeight = height;
			// Reallocate the receiving buffer
			delete ndiChars;
			ndiChars = new unsigned char[senderWidth*senderHeight * 4];
			// Reallocate the image we use for display
			ndiImage.allocate(senderWidth, senderHeight, OF_IMAGE_COLOR_ALPHA);
		}
		else {
			// Update the display image
			ndiImage.getPixels().setFromExternalPixels(ndiChars, senderWidth, senderHeight, 4);
			ndiImage.update();
		}
	}
	// Draw whether received or not
	ndiImage.draw(0, 0, ofGetWidth(), ofGetHeight());
	*/

	//// Show what it is receiving
	//ShowInfo();

}

#endif