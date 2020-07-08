#include "ofxNDIHelper.h"

#pragma mark - OF

//--------------------------------------------------------------
ofxNDIHelper::ofxNDIHelper()
{
	DISABLE_Callbacks = true;

	//path for settings
	//setPathGlobal("ofxNDIHelper/");
	path_GLOBAL = "ofxNDIHelper/";//this is to folder all files to avoid mixing with other addons data

	path_Params_Internal = "params_Internal.xml";
	path_Params_Control = "params_Control.xml";

	setActive(true);//add key and mouse listeners
}

//--------------------------------------------------------------
void ofxNDIHelper::begin_NDI_OUT()
{
	fbo_NDI_Sender.begin();
	ofClear(0, 0, 0, 255);
}

//--------------------------------------------------------------
void ofxNDIHelper::draw_NDI_OUT()
{
	if (ENABLE_NDI_Output.get()) {
		fbo_NDI_Sender.draw(0, 0, ofGetWidth(), ofGetHeight());
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::end_NDI_OUT()
{
	fbo_NDI_Sender.end();

	//--

	//if (ENABLE_NDI_Output.get()) 
	//{
		ofColor(255);
		ndiSender.SendImage(fbo_NDI_Sender);//send processed
		//ndiSender.SendImage(fbo_NDI_Sender, false);//send processed
		//ndiSender.SendImage(vidGrabber.getPixels());//send clean webcam
		//Draw the fbo result fitted to the display window
		//fbo_NDI_Sender.draw(0, 0, ofGetWidth(), ofGetHeight());
	//}

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
	ENABLE_Addon.set("ENABLE", true);
	ENABLE_Edit.set("EDIT", true);

	ENABLE_Webcam.set("ENABLE WEBCAM", false);
	mini_Webcam.set("MINI WEBCAM", false);
	index_WebcamDevice.set("DEVICE WEBCAM", 0, 0, 1);
	name_Webcam.set("", "");

	ENABLE_NDI_Input.set("ENABLE NDI INPUT", false);
	mini_ndiInput.set("MINI INPUT", false);
	index_NDI_Input.set("DEVICE NDI INPUT", 0, 0, 1);
	name_NDI_Input.set("NDI_IN", "ofxNDIHelperIN");

	ENABLE_NDI_Output.set("ENABLE NDI OUTPUT", false);
	name_NDI_Output.set("NDI_OUT", "ofxNDIHelperOUT");

	//exclude from settings
	name_Webcam.setSerializable(false);
	name_NDI_Input.setSerializable(false);
	//name_NDI_Output.setSerializable(false);

	//webcam
	ofParameterGroup gWebcam{ "ENABLE WEBCAM" };
	gWebcam.add(ENABLE_Webcam);
	gWebcam.add(mini_Webcam);
	gWebcam.add(index_WebcamDevice);
	gWebcam.add(name_Webcam);

	//ndi in
	ofParameterGroup gNdiI{ "NDI INPUT" };
	gNdiI.add(ENABLE_NDI_Input);
	gNdiI.add(mini_ndiInput);
	gNdiI.add(index_NDI_Input);
	gNdiI.add(name_NDI_Input);

	//ndi out
	ofParameterGroup gNdiO{ "ENABLE NDI OUTPUT" };
	gNdiO.add(ENABLE_NDI_Output);
	gNdiO.add(name_NDI_Output);

	//-

	//group
	params_Control.setName("CONTROL");
	params_Control.add(ENABLE_Addon);
	params_Control.add(ENABLE_Edit);
	params_Control.add(gWebcam);
	params_Control.add(gNdiI);
	params_Control.add(gNdiO);

	//callback
	ofAddListener(params_Control.parameterChangedE(), this, &ofxNDIHelper::Changed_params_Control);

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
	params_Internal.setName("INTERNAL");
	params_Internal.add(MODE_App);
	params_Internal.add(MODE_App_Name);
	params_Internal.add(MODE_Active);
	params_Internal.add(ENABLE_keys);
	params_Internal.add(SHOW_Help);
	params_Internal.add(ENABLE_Debug);
	params_Internal.add(ENABLE_AutoSave);
	params_Internal.add(SHOW_Gui);
	params_Internal.add(Gui_Position);

	//callback
	ofAddListener(params_Internal.parameterChangedE(), this, &ofxNDIHelper::Changed_params_Internal);

	//-

	//all params
	params.setName("ofxNDIHelper");
	params.add(params_Control);
	params.add(params_Internal);

	//callback
	ofAddListener(params.parameterChangedE(), this, &ofxNDIHelper::Changed_params);

	//--

	//gui

	//theme
	string str = "fonts/overpass-mono-bold.otf";
	ofxGuiSetFont(path_GLOBAL + str, 9);//TODO: allow default OF_TTF

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
	auto &g0 = gui_Control.getGroup("ofxNDIHelper");//1st level
	auto &g2 = g0.getGroup("CONTROL");//2nd level
	auto &g3 = g0.getGroup("INTERNAL");//2nd level
	auto &g31 = g3.getGroup("GUI POSITION");//3nd level
	//g0.minimize();
	//g2.minimize();
	g3.minimize();
	g31.minimize();

	//startup

	ofLogNotice(__FUNCTION__) << "STARTUP INIT";
	DISABLE_Callbacks = false;

	MODE_Active = true;

	loadParams(params_Internal, path_GLOBAL + path_Params_Internal);
	loadParams(params_Control, path_GLOBAL + path_Params_Control);

	//set gui position after window setup/resizing
	windowResized(screenW, screenH);

	//----

	//sources / modules
#ifdef USE_WEBCAM
	setupWebcam();
#endif

#ifdef USE_ofxNDI_OUT
	setup_NDI_OUT();
#endif

#ifdef USE_ofxNDI_IN
	setup_NDI_IN();
#endif

	//-----

	refresh_NDI_IN();

	//-
}

//--------------------------------------------------------------
void ofxNDIHelper::update()
{

#ifdef USE_WEBCAM
	if (ENABLE_Webcam.get()) {
		ofBackground(100, 100, 100);
		vidGrabber.update();

		if (vidGrabber.isFrameNew()) {
			//ofPixels & pixels = vidGrabber.getPixels();
			//for (size_t i = 0; i < pixels.size(); i++) {
			//	//invert the color of the pixel
			//	videoInverted[i] = 255 - pixels[i];
			//}
			////load the inverted pixels
			//videoTexture.loadData(videoInverted);
		}
	}
#endif

	//--

	//autosave
	//ENABLE_AutoSave = false;
	if (ENABLE_AutoSave && ofGetElapsedTimeMillis() - timerLast_Autosave > timeToAutosave)
	{
		ofLogLevel _logPre = ofGetLogLevel();
		ofSetLogLevel(OF_LOG_SILENT);//bypass this logs

		DISABLE_Callbacks = true;

		//get gui position before save
		Gui_Position = glm::vec2(gui_Control.getPosition());
		saveParams(params_Internal, path_GLOBAL + path_Params_Internal);
		saveParams(params_Control, path_GLOBAL + path_Params_Control);

		DISABLE_Callbacks = false;

		timerLast_Autosave = ofGetElapsedTimeMillis();
		ofLogNotice(__FUNCTION__) << "Autosaved DONE";

		ofSetLogLevel(_logPre);
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::drawGui()
{
	if (SHOW_Gui)
	{
		//edit mode
		if (MODE_App == 1)
		{
#ifdef USE_ofxNDI_IN
			//Show what it is receiving
			drawInfoDevices();
#endif
		}

		//-

		//gui panel
		gui_Control.draw();
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::exit()
{
#ifdef USE_ofxNDI_OUT
	//The sender must be released 
	//or NDI sender discovery will still find it
	ndiSender.ReleaseSender();
#endif

	//--

	//get gui position before save
	Gui_Position = glm::vec2(gui_Control.getPosition());

	saveParams(params_Internal, path_GLOBAL + path_Params_Internal);
	saveParams(params_Control, path_GLOBAL + path_Params_Control);
}

//--------------------------------------------------------------
ofxNDIHelper::~ofxNDIHelper()
{
	setActive(false);//remove keys and mouse listeners

	//remove params callbacks listeners
	ofRemoveListener(params.parameterChangedE(), this, &ofxNDIHelper::Changed_params);
	ofRemoveListener(params_Internal.parameterChangedE(), this, &ofxNDIHelper::Changed_params_Internal);
	ofRemoveListener(params_Control.parameterChangedE(), this, &ofxNDIHelper::Changed_params_Control);
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

	//ndiSender.UpdateSender(1920, 1080);//update size

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
		////custom
		//if (key == ' ')
		//{
		//}
		//else if (key == ' ')
		//{
		//}

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

		//----

		//ndi
#ifdef USE_ofxNDI_IN
		//edit mode
		if (MODE_App == 1)
		{
			char name[256];
			int index = key - 48;//slect by key numbers

			int nsenders = ndiReceiver.GetSenderCount();
			index_NDI_Input.setMax(nsenders - 1);

			if (key == ' ') {
				refresh_NDI_IN();
			}

			//numbers
			else if (nsenders > 0 && index >= 0 && index < nsenders) {
				//Update the receiver with the returned index
				//Returns false if the current sender is selected
				if (ndiReceiver.SetSenderIndex(index)) {
					index_NDI_Input = index;
					ofLogNotice(__FUNCTION__) << "Selected [" << ndiReceiver.GetSenderName(index) << "]";
					ndiReceiver.GetSenderName(name, 256, index);
					ofLogNotice(__FUNCTION__) << "    Sender " << index << " [" << name << "]";
					std::string my_string(name);
					name_NDI_Input = my_string;
					ofLogNotice(__FUNCTION__) << "name_NDI_Input: " << name_NDI_Input.get();
				}
				else
					ofLogNotice(__FUNCTION__) << "Stay in same sender";
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

		if (name == "EDIT")
		{
			if (ENABLE_Edit.get())
			{
				MODE_App = 1;//edit

				if (ENABLE_NDI_Input.get()) {
					refresh_NDI_IN();
				}
#ifdef USE_WEBCAM
				if (ENABLE_Webcam.get()) {
					setupWebcam();
				}
#endif
			}
			else
			{
				MODE_App = 0;
			}
		}

		//webcam
#ifdef USE_WEBCAM
		else if (name == "ENABLE WEBCAM" && ENABLE_Webcam.get())
		{
			setupWebcam();
		}
		else if (name == "DEVICE WEBCAM" && ENABLE_Webcam.get())
		{
			setupWebcam();
		}
#endif

		//--

		//ndi in 
		else if (name == "ENABLE NDI INPUT" && ENABLE_NDI_Input.get())
		{
			setup_NDI_IN();
			//refresh_NDI_IN();
		}
		else if (name == "DEVICE NDI INPUT" && ENABLE_NDI_Input.get())
		{
			setup_NDI_IN();
			//refresh_NDI_IN();

			//int nsenders = ndiReceiver.GetSenderCount();
			//index_NDI_Input.setMax(nsenders - 1);
			//int index = index_NDI_Input.get();
			//char name[256];

			//if (nsenders > 0 && index >= 0 && index < nsenders) {
			//	//Update the receiver with the returned index
			//	//Returns false if the current sender is selected
			//	if (ndiReceiver.SetSenderIndex(index)) {
			//		ofLogNotice(__FUNCTION__) << "Selected [" << ndiReceiver.GetSenderName(index) << "]";
			//		ndiReceiver.GetSenderName(name, 256, index);
			//		ofLogNotice(__FUNCTION__) << "    Sender " << index << " [" << name << "]";
			//		//std::string my_string(name);
			//		//name_NDI_Input = my_string;
			//		name_NDI_Input = ndiReceiver.GetSenderName(index);
			//		ofLogNotice(__FUNCTION__) << "name_NDI_Input: " << name_NDI_Input.get();
			//	}
			//	else
			//		ofLogNotice(__FUNCTION__) << "Stay in same sender";
			//}
			//else
			//{
			//	ofLogError(__FUNCTION__) << "NOT ANY NDI SENDERS!";
			//}
		}
		//else if (name == "NDI_IN" && ENABLE_NDI_Input.get())
		//{
		//}

		//--

		//ndi out
		else if (name == "ENABLE NDI OUTPUT")
		{
			if (ENABLE_NDI_Output.get()) {
				//setup_NDI_OUT();
			}
			else
			{
				//ndiSender.ReleaseSender();//?
			}
		}
		else if (name == "NDI_OUT" && ENABLE_NDI_Output.get())
		{
			//setup_NDI_OUT();
		}
	}
}

//addon control (internal) params
//--------------------------------------------------------------
void ofxNDIHelper::Changed_params_Internal(ofAbstractParameter &e)
{
	if (!DISABLE_Callbacks)
	{
		string name = e.getName();

		//exclude debugs
		if (name != "exclude"
			&& name != "exclude")
		{
			ofLogNotice(__FUNCTION__) << "Changed_params_Internal: " << name << " : " << e;

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
//TODO:
//--------------------------------------------------------------
void ofxNDIHelper::setupWebcam() {
	////get back a list of devices.
	//vector<ofVideoDevice> devices = vidGrabber.listDevices();
	//
	//for (size_t i = 0; i < devices.size(); i++) {
	//	if (devices[i].bAvailable) {
	//		//log the device
	//		ofLogNotice() << devices[i].id << ": " << devices[i].deviceName;
	//	}
	//	else {
	//		//log the device and note it as unavailable
	//		ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - unavailable ";
	//	}
	//}

	vidGrabber.setVerbose(true);

	//must close before reopen
	vidGrabber.close();

	auto devs = vidGrabber.listDevices();
	int _last = devs.size() - 1;
	index_WebcamDevice.setMax(_last);

	int index = index_WebcamDevice.get();
	if (index <= _last) {
		name_Webcam = devs[index].deviceName;
	}
	//else {
	//	index = -1;
	//	index_WebcamDevice = index;
	//	name_Webcam = "NOT FOUND!";
	//}

	//get all dev names
	webcam_InputDevices = "";
	for (int i = 0; i < devs.size(); i++) {
		webcam_InputDevices += "[" + ofToString(i) + "] " + devs[i].deviceName;
		if (i != devs.size() - 1) webcam_InputDevices += "\n";
	}

	////init
	//if (devs.size() > 0) 
	//{
	//	if (index < devs.size()) {
	vidGrabber.setDeviceID(index);
	vidGrabber.setDesiredFrameRate(60);
	vidGrabber.setup(1920, 1080);
	//vidGrabber.initGrabber(1920, 1080);

	bool bConnected = vidGrabber.isInitialized();
	ofLogNotice(__FUNCTION__) << "vidGrabber initialized: " << (bConnected ? "TRUE" : "FALSE");
	//ofLogNotice(__FUNCTION__) << "vidGrabber: " << vidGrabber.;

	if (!bConnected) {
		ofLogError(__FUNCTION__) << "CAN'T INITIALIZA vidGrabber!";
		ofLogError(__FUNCTION__) << "CAMERA DISABLED";
		ENABLE_Webcam = false;
	}
	//	}
	//}
	//else
	//{
	//	index = -1;
	//	index_WebcamDevice = index;
	//	name_Webcam = "NOT FOUND!";
	//	ofLogError(__FUNCTION__) << "NOT ANY VIDEOCAM vidGrabber!";
	//}
}
#endif

//--------------------------------------------------------------
void ofxNDIHelper::drawWebcam() {
#ifdef USE_WEBCAM
	if (ENABLE_Webcam.get()) {
		ofSetHexColor(0xffffff);

		////ofSetColor(255, 255);
		ofRectangle videoRect;
		videoRect = ofRectangle(0, 0, vidGrabber.getWidth(), vidGrabber.getHeight());
		if (!mini_Webcam.get()) {
			videoRect.scaleTo(ofGetWindowRect());
		}
		else
		{
			ofRectangle r = ofGetWindowRect();
			videoRect.scaleTo(ofRectangle(0, vidGrabber.getHeight()*0.2, vidGrabber.getWidth()*0.25, vidGrabber.getHeight()*0.25));
		}
		vidGrabber.draw(videoRect.x, videoRect.y, videoRect.width, videoRect.height);

		//vidGrabber.draw(0, 0);
		//videoTexture.draw(20 + camWidth, 20, camWidth, camHeight);
	}
#endif
}

#ifdef USE_ofxNDI_OUT

//--------------------------------------------------------------
void ofxNDIHelper::setup_NDI_OUT() {

	ndiSender.ReleaseSender();//?

	ofLogNotice(__FUNCTION__) << ndiSender.GetNDIversion() << " (http://ndi.tv/)";
	//strcpy_s(senderName, 256, "OF NDI Sender"); // Set the sender name

	const std::string str = name_NDI_Output.get();
	strcpy_s(senderName, 256, str.c_str()); //Set the sender name

	//Set the dimensions of the sender output here
	//This is independent of the size of the display window
	//Sender dimensions and fps are not known yet
	//senderWidth = (unsigned char)ofGetWidth();
	//senderHeight = (unsigned char)ofGetHeight();
	senderWidth = 1920;
	senderHeight = 1080;

	//fbo settings
	ofFbo::Settings settings;
	settings.internalformat = GL_RGBA;
	settings.width = senderWidth;
	settings.height = senderHeight;
	fbo_NDI_Sender.allocate(settings);
	fbo_NDI_Sender.begin();
	ofClear(0, 0, 0, 255);
	fbo_NDI_Sender.end();

	////Create an RGBA fbo for collection of data
	//fbo_NDI_Sender.allocate(senderWidth, senderHeight, GL_RGBA);

	//Optionally set fbo readback using OpenGL pixel buffers
	ndiSender.SetReadback(); //Change to false to compare

	//Set NDI asynchronous sending for best performance
	ndiSender.SetAsync();

	////Create a new sender - default RGBA for ofPixels
	//ndiSender.CreateSender("Openframeworks NDI webcam", (unsigned int)vidGrabber.getWidth(), (unsigned int)vidGrabber.getHeight());
	//ofLogNotice(__FUNCTION__) << "Created NDI sender [Openframeworks NDI webcam] (" << vidGrabber.getWidth() << "x" << vidGrabber.getHeight() << ")" ;

	//Create a sender with default RGBA output format
	ndiSender.CreateSender(senderName, senderWidth, senderHeight);
	//ndiSender.CreateSender("OF TEST", senderWidth, senderHeight);

	ofLogNotice(__FUNCTION__) << "Created NDI sender [" << senderName << "] (" << senderWidth << "x" << senderHeight << ")";
}
#endif


#ifdef USE_ofxNDI_IN

//--------------------------------------------------------------
void ofxNDIHelper::drawInfoDevices() {
	int x, y;
	x = 10;
	y = 20;

	int l = 20;
	int i = 0;

	if (ENABLE_Webcam.get()) {
		ofDrawBitmapStringHighlight("WEBCAM INPUT DEVICES", x, y + l * i++);
		ofDrawBitmapStringHighlight(">" + name_Webcam.get()/* + " [" + ofToString(index_WebcamDevice.get()) + "]"*/, x, y + l * i++);
		ofDrawBitmapStringHighlight(webcam_InputDevices, x, y + l * i++);
	}

	//-

	i = 0;
	x += 250;
	if (ENABLE_NDI_Input.get()) {
		char str[256];

		ofDrawBitmapStringHighlight("NDI INPUT DEVICES", x, y + l * i++);
		int nsenders = ndiReceiver.GetSenderCount();
		if (nsenders > 0) {

			if (ndiReceiver.ReceiverCreated()) {
				if (ndiReceiver.ReceiverConnected()) {
					//Show received sender information and received fps
					sprintf_s(str, 256, "[%s] (%dx%d/%4.2fp) (fps %2.0f)", ndiReceiver.GetSenderName().c_str(), ndiReceiver.GetSenderWidth(), ndiReceiver.GetSenderHeight(), ndiReceiver.GetSenderFps(), ndiReceiver.GetFps());
				}
				else {
					//Nothing received
					sprintf_s(str, 256, "> %s", ndiReceiver.GetSenderName().c_str());
				}
			}
			ofDrawBitmapStringHighlight(str, x, y + l * i++);
			//i++;

			if (nsenders == 1) {
				ofDrawBitmapStringHighlight("1 NET SOURCE", x, y + l * i++);
			}
			else {
				sprintf_s(str, 256, "%d NET SOURCES.", nsenders);
				string str2(str);
				str2 += " SPACE: LIST SENDERS";
				ofDrawBitmapStringHighlight(str2, x, y + l * i++);
			}
		}
		else {
			ofDrawBitmapStringHighlight("CONNECTING...", x, y + l * i++);
		}

		//ndi input devices list
		if (NDI_InputDevices.size() > 0) ofDrawBitmapStringHighlight(NDI_InputDevices, x, y + l * i++);
	}

	//-

	i = 0;
	x += 650;
	if (ENABLE_NDI_Output.get()) {
		//Show what it is sending
		if (ndiSender.SenderCreated()) {
			ofDrawBitmapStringHighlight("NDI OUPUT DEVICE", x, y + l * i++);
			char str[256];
			sprintf_s(str, 256, "SENDING AS : [%s] (%dx%d)", senderName, senderWidth, senderHeight);
			ofDrawBitmapStringHighlight(str, x, y + l * i++);
			//Show fps
			sprintf_s(str, 256, "FPS: %3.3d (%4.2f)", (int)ofGetFrameRate(), ndiSender.GetFps());
			ofDrawBitmapStringHighlight(str, x, y + l * i++);
		}
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::refresh_NDI_IN() {
	////deep setup
	//setup_NDI_IN();

	char name[256];
	int nsenders = ndiReceiver.GetSenderCount();
	index_NDI_Input.setMax(nsenders - 1);

	//List all the senders
	if (nsenders > 0) {
		ofLogNotice(__FUNCTION__) << "Number of NDI senders found: " << nsenders;
		NDI_InputDevices = "";
		for (int i = 0; i < nsenders; i++) {
			string name = ndiReceiver.GetSenderName(i);
			string str;
			str = "[" + ofToString(i) + "] " + name;
			ofLogNotice(__FUNCTION__) << str;
			NDI_InputDevices += str;

			if (i != nsenders - 1) NDI_InputDevices += "\n";
		}
		if (nsenders > 1)
			ofLogNotice(__FUNCTION__) << "Press key [0] to [" << nsenders - 1 << "] to select a sender";
	}
	else
		ofLogNotice(__FUNCTION__) << "No NDI senders found";
}

//--------------------------------------------------------------
void ofxNDIHelper::setup_NDI_IN() {
	//ofSetWindowTitle("ofxNDIHelper");

#ifdef _WIN64
	ofLogNotice(__FUNCTION__) << "\nofxNDI example receiver - 64 bit";
#else //_WIN64
	ofLogNotice(__FUNCTION__) << "\nofxNDI example receiver - 32 bit";
#endif //_WIN64
	ofLogNotice(__FUNCTION__) << ndiReceiver.GetNDIversion() << " (http://ndi.tv/)";
	ofLogNotice(__FUNCTION__) << "Press 'SPACE' to list NDI senders";

	//--

	//TODO:

	//ofFbo
	////receiver dimensions and fps are not known yet
	//receiverWidth = (unsigned char)ofGetWidth();
	//receiverHeight = (unsigned char)ofGetHeight();
	receiverWidth = 1920;
	receiverHeight = 1080;

	fbo_NDI_Receiver.allocate(receiverWidth, receiverHeight, GL_RGBA);

	//Clear the fbo so the first frame draw is black
	fbo_NDI_Receiver.begin();
	ofClear(0, 0, 0, 255);
	fbo_NDI_Receiver.end();

	//ofTexture
	ndiReceiveTexture.allocate(receiverWidth, receiverHeight, GL_RGBA);
	////ofImage
	//ndiReceiveImage.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR_ALPHA);
	////ofPixels
	//ndiReceivePixels.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR_ALPHA);

	//unsigned char pixels
	ndiReceiveChars = new unsigned char[receiverWidth*receiverHeight * 4];

	//index_NDI_Input.setMax(ndiReceiver.GetSenderCount() - 1);



	refresh_NDI_IN();



	int nsenders = ndiReceiver.GetSenderCount();
	index_NDI_Input.setMax(nsenders - 1);
	int index = index_NDI_Input.get();
	char name[256];

	if (nsenders > 0 && index >= 0 && index < nsenders) {
		//Update the receiver with the returned index
		//Returns false if the current sender is selected
		if (ndiReceiver.SetSenderIndex(index)) {
			ofLogNotice(__FUNCTION__) << "Selected [" << ndiReceiver.GetSenderName(index) << "]";
			ndiReceiver.GetSenderName(name, 256, index);
			ofLogNotice(__FUNCTION__) << "    Sender " << index << " [" << name << "]";
			//std::string my_string(name);
			//name_NDI_Input = my_string;
			name_NDI_Input = ndiReceiver.GetSenderName(index);
			ofLogNotice(__FUNCTION__) << "name_NDI_Input: " << name_NDI_Input.get();
		}
		else
			ofLogNotice(__FUNCTION__) << "Stay in same sender";
	}
	else
	{
		ofLogError(__FUNCTION__) << "NOT ANY NDI SENDERS!";
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::draw_NDI_IN() {
	if (ENABLE_NDI_Input) {

		//Receive ofTexture
		ndiReceiver.ReceiveImage(ndiReceiveTexture);

		//draw
		//ndiReceiveTexture.draw(0, 0, ofGetWidth(), ofGetHeight());
		if (!mini_ndiInput.get()) {
			ndiReceiveTexture.draw(0, 0, ofGetWidth(), ofGetHeight());
		}
		else {
			ndiReceiveTexture.draw(0, ofGetHeight()*0.55, ofGetWidth()*0.25, ofGetHeight()*0.25);
		}

		//TODO:
		//Receive ofFbo
		//ndiReceiver.ReceiveImage(fbo_NDI_Receiver);
		//fbo_NDI_Receiver.draw(0, 0, ofGetWidth(), ofGetHeight());

		//Receive ofImage
		//ndiReceiver.ReceiveImage(ndiReceiveImage);
		//ndiReceiveImage.draw(0, 0, ofGetWidth(), ofGetHeight());

		//Receive ofPixels
		//ndiReceiver.ReceiveImage(ndiReceivePixels);
		//ndiReceiveImage.setFromPixels(ndiReceivePixels);
		//ndiReceiveImage.draw(0, 0, ofGetWidth(), ofGetHeight());

		/*
		//Receive unsigned char pixel image
		//ndiReceiveChars is the buffer to receive the pixels
		//buffer size must be managed if there is a sender size change
		unsigned int width = (unsigned int)ofGetWidth();
		unsigned int height = (unsigned int)ofGetHeight();
		if (ndiReceiver.ReceiveImage(ndiReceiveChars, width, height)) {
			if (width != senderWidth || height != senderHeight) {
				//Update sender dimensions
				senderWidth = width;
				senderHeight = height;
				//Reallocate the receiving buffer
				delete ndiReceiveChars;
				ndiReceiveChars = new unsigned char[senderWidth*senderHeight * 4];
				//Reallocate the image we use for display
				ndiReceiveImage.allocate(senderWidth, senderHeight, OF_IMAGE_COLOR_ALPHA);
			}
			else {
				//Update the display image
				ndiReceiveImage.getPixels().setFromExternalPixels(ndiReceiveChars, senderWidth, senderHeight, 4);
				ndiReceiveImage.update();
			}
		}
		//Draw whether received or not
		ndiReceiveImage.draw(0, 0, ofGetWidth(), ofGetHeight());
		*/
	}
}

#endif