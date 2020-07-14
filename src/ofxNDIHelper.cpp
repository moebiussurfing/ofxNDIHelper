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

#ifdef USE_WEBCAM
	path_WebcamSettings = "webcam_Settings.xml";
#endif

	setActive(true);//add key and mouse listeners
}

//feed
//--------------------------------------------------------------
void ofxNDIHelper::begin_NDI_OUT()
{
	if (ENABLE_NDI_Output.get())
	{
		fbo_NDI_Sender.begin();
		ofClear(0, 0, 0, 255);
	}
}
//--------------------------------------------------------------
void ofxNDIHelper::end_NDI_OUT()
{
	if (ENABLE_NDI_Output.get())
	{
		if (SHOW_Help.get()) ofDrawBitmapStringHighlight("NDI OUT", 40, 60);

		fbo_NDI_Sender.end();

		//--

		//do the sending

		ofColor(255);
		ndiSender.SendImage(fbo_NDI_Sender);
		//ndiSender.SendImage(fbo_NDI_Sender, false);//send processed
		//ndiSender.SendImage(vidGrabber.getPixels());//send clean webcam
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::draw_NDI_OUT_Preview()//TODO:mini
{
	if (ENABLE_NDI_Output.get())
	{
		ofPushStyle();
		ofSetColor(255, 255);

		if (!mini_ndiOutput.get())//full size 
		{
			ofRectangle r(0, 0, fbo_NDI_Sender.getWidth(), fbo_NDI_Sender.getHeight());
			r.scaleTo(ofGetWindowRect());
			//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_CENTER);
			//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_STRETCH_TO_FILL);
			//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_FILL);

			fbo_NDI_Sender.draw(r.x, r.y, r.width, r.height);
		}
		else//mini
		{
			int xpad = 10;

			//float xx = xpad;
			float yy = fbo_NDI_Sender.getHeight() * 0.5;
			float ww = fbo_NDI_Sender.getWidth() * 0.25;
			float xx = xpad + 20 + ww;
			float hh = fbo_NDI_Sender.getHeight() * 0.25;

			//TODO:?
			ofRectangle rw = ofGetWindowRect();
			rw.scaleTo(ofRectangle(xx, yy, ww, hh));

			fbo_NDI_Sender.draw(rw.x, rw.y, rw.width, rw.height);

			//bb border
			ofNoFill();
			ofSetColor(0);
			ofSetLineWidth(2.0);
			ofDrawRectangle(rw);

			ofDrawBitmapStringHighlight("NDI OUT", rw.x + 4 - 1, rw.y - 5);
		}

		//fbo_NDI_Sender.draw(0, 0, ofGetWidth(), ofGetHeight());

		ofPopStyle();
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::setup()
{
	//log mode
	ofSetLogLevel("ofxNDIHelper", OF_LOG_NOTICE);

	font.load(OF_TTF_MONO, 10);

	//--

	//window
	screenW = ofGetWidth();
	screenH = ofGetHeight();

	//--

	//params control (addon variables)
	ENABLE_Addon.set("ENABLE", true);
	ENABLE_Edit.set("EDIT", true);

	//ndi in
	ENABLE_NDI_Input.set("ENABLE NDI INPUT", false);
	ENABLE_Draw_NDI_Input.set("DRAW NDI INPUT", true);
	mini_ndiInput.set("MINI INPUT", false);
	index_NDI_Input.set("DEVICE NDI INPUT", 0, 0, 1);
	name_NDI_Input.set("NDI_IN", "ofxNDIHelperIN");

	//ndi out
	ENABLE_NDI_Output.set("ENABLE NDI OUTPUT", false);
	ENABLE_Draw_NDI_Output.set("DRAW NDI OUTPUT", true);
	mini_ndiOutput.set("MINI OUTPUT", false);
	name_NDI_Output.set("NDI_OUT", "ofxNDIHelperOUT");

	//exclude from settings
	name_NDI_Input.setSerializable(false);
	//name_NDI_Output.setSerializable(false);

	//--

	//webcam
#ifdef USE_WEBCAM
	ENABLE_Webcam.set("ENABLE WEBCAM", false);
	ENABLE_Draw_Webcam.set("DRAW WEBCAM", true);
	mini_Webcam.set("MINI WEBCAM", false);
	index_WebcamDevice.set("DEVICE WEBCAM", 0, 0, 1);
	name_Webcam.set("", "");

	ofParameterGroup gWebcam{ "WEBCAM" };
	gWebcam.add(ENABLE_Webcam);
	gWebcam.add(ENABLE_Draw_Webcam);
	gWebcam.add(mini_Webcam);
	gWebcam.add(index_WebcamDevice);
	gWebcam.add(name_Webcam);

	index_WebcamDevice.setSerializable(false);
	name_Webcam.setSerializable(false);
#endif

	//--

	//ndi in
	ofParameterGroup gNdiI{ "NDI INPUT" };
	gNdiI.add(ENABLE_NDI_Input);
	gNdiI.add(ENABLE_Draw_NDI_Input);
	gNdiI.add(mini_ndiInput);
	gNdiI.add(index_NDI_Input);
	gNdiI.add(name_NDI_Input);

	//ndi out
	ofParameterGroup gNdiO{ "NDI OUTPUT" };
	gNdiO.add(ENABLE_NDI_Output);
	gNdiO.add(ENABLE_Draw_NDI_Output);
	gNdiO.add(mini_ndiOutput);
	gNdiO.add(name_NDI_Output);

	SHOW_Help.set("HELP", false);

	//-

	//group
	params_Control.setName("CONTROL");
	params_Control.add(ENABLE_Addon);
	params_Control.add(SHOW_Help);
	params_Control.add(ENABLE_Edit);

	//webcam
#ifdef USE_WEBCAM
	params_Control.add(gWebcam);
#endif
	params_Control.add(gNdiI);
	params_Control.add(gNdiO);

	//callback
	ofAddListener(params_Control.parameterChangedE(), this, &ofxNDIHelper::Changed_params_Control);

	//-

	//addon control(internal)

	//params
	MODE_Active.set("ACTIVE", true);
	ENABLE_keys.set("KEYS", true);
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

	//ofxGui theme
	string str = "overpass-mono-bold.otf";
	string pathFont = "assets/fonts/" + str;
	ofFile file(pathFont);
	//must check this font file is detected
	if (file.exists())
	{
		ofxGuiSetFont(pathFont, 9);
		ofLogNotice(__FUNCTION__) << "LOADED FILE '" << pathFont << "'";
	}
	else
	{
		ofLogError(__FUNCTION__) << "FILE '" << pathFont << "' NOT FOUND!";
	}
	ofxGuiSetDefaultHeight(20);
	ofxGuiSetBorderColor(32);
	ofxGuiSetFillColor(ofColor(48));
	ofxGuiSetTextColor(ofColor::white);
	ofxGuiSetHeaderColor(ofColor(24));

	//--

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

	//--

	//sources / modules
	//must be called before the loading of settings bc how callbacks are running..
#ifdef USE_WEBCAM
	setupWebcam();
#endif

	MODE_Active = true;

#ifdef USE_ofxNDI_IN
	setup_NDI_IN();
#endif

	//----

#ifdef USE_ofxNDI_OUT
	setup_NDI_OUT();
#endif

	//----

	//startup

	ofLogNotice(__FUNCTION__) << "STARTUP";
	DISABLE_Callbacks = false;

	MODE_Active = true;

	//set gui position after window setup/resizing
	windowResized(screenW, screenH);

	//file settings
	loadParams(params_Internal, path_GLOBAL + path_Params_Internal);
	loadParams(params_Control, path_GLOBAL + path_Params_Control);

	//-----

	refresh_NDI_IN();

	//-
}

//--------------------------------------------------------------
void ofxNDIHelper::update()
{

#ifdef USE_WEBCAM
	if (ENABLE_Webcam.get()) {
		//ofBackground(100, 100, 100);

		vidGrabber.update();

		//if (vidGrabber.isFrameNew()) {
		//	//ofPixels & pixels = vidGrabber.getPixels();
		//	//for (size_t i = 0; i < pixels.size(); i++) {
		//	//	//invert the color of the pixel
		//	//	videoInverted[i] = 255 - pixels[i];
		//	//}
		//	////load the inverted pixels
		//	//videoTexture.loadData(videoInverted);
		//}
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
void ofxNDIHelper::draw() {

	//-

	//ndi out
	if (ENABLE_NDI_Output.get() && ENABLE_Draw_NDI_Output.get()) {
		draw_NDI_OUT_Preview();
	}

	//-

	//ndi in
	if (ENABLE_NDI_Input.get() && ENABLE_Draw_NDI_Input.get()) {
		draw_NDI_IN_Preview();
	}

	//-

	//webcam
#ifdef USE_WEBCAM
	if (ENABLE_Webcam.get() && ENABLE_Draw_Webcam.get())
	{
		drawWebcam_Preview();
	}
#endif
}

//--------------------------------------------------------------
void ofxNDIHelper::drawGui()
{
	if (SHOW_Gui)
	{
		//edit mode
		if (MODE_App == 1)
		{
			if (SHOW_Help.get())
			{
				//Show what it is receiving
				drawInfoDevices();
			}
		}

		//-

		//gui panel
		gui_Control.draw();
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::exit()
{
	//webcam
#ifdef USE_WEBCAM
	exitWebcam();
#endif

	//-

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

		//edit mode
		if (MODE_App == 1)
		{
			//----

			//ndi input

#ifdef USE_ofxNDI_IN
			char name[256];
			int index = key - 48;//slect by key numbers

			int _nsendersRemote = ndiReceiver.GetSenderCount();
			index_NDI_Input.setMax(_nsendersRemote - 1);

			if (key == ' ') {
				refresh_NDI_IN();
			}

			//numbers
			else if (_nsendersRemote > 0 && index >= 0 && index < _nsendersRemote) {
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
#endif
			//----

			//webcam

#ifdef USE_WEBCAM
			if (key == 'i') {
				//webcam
				auto _devs = vidGrabber.listDevices();
				index_WebcamDevice++;
				if (index_WebcamDevice.get() > _devs.size() - 1) index_WebcamDevice = 0;
				_dName = _devs[index_WebcamDevice].deviceName;
				name_Webcam = _dName;

				//select cam device
				vidGrabber.close();
				vidGrabber.setDeviceID(index_WebcamDevice.get());
				vidGrabber.setup(1920, 1080);
			}
#endif
			//----
		}
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

		//--

		if (name == "EDIT")
		{
			if (ENABLE_Edit.get())
			{
				MODE_App = 1;//edit

				if (ENABLE_NDI_Input.get()) {
					refresh_NDI_IN();
				}

				//--

#ifdef USE_WEBCAM
				//if (ENABLE_Webcam.get()) {
				//	setupWebcam();
				//}
#endif
				//--
			}
			else
			{
				MODE_App = 0;
			}
		}

		//--

		//webcam
#ifdef USE_WEBCAM
		else if (name == ENABLE_Webcam.getName())
		{
			if (ENABLE_Webcam.get()) restartWebcam();
			else vidGrabber.close();
		}
		else if (name == index_WebcamDevice.getName() && ENABLE_Webcam.get())
		{
			setupWebcam(index_WebcamDevice.get());
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

			//int _nsendersRemote = ndiReceiver.GetSenderCount();
			//index_NDI_Input.setMax(_nsendersRemote - 1);
			//int index = index_NDI_Input.get();
			//char name[256];

			//if (_nsendersRemote > 0 && index >= 0 && index < _nsendersRemote) {
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

	CheckFolder(path_GLOBAL);
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

//--

//webcam
#ifdef USE_WEBCAM

//TODO:
//--------------------------------------------------------------
void ofxNDIHelper::restartWebcam() {
	//must close before reopen
	vidGrabber.close();

	//start device
	//if (ENABLE_Webcam.get()) 
	{
		vidGrabber.setDeviceID(index_WebcamDevice.get());
		//vidGrabber.setDesiredFrameRate(60);
		vidGrabber.setup(1920, 1080);
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::setupWebcam(int index) {
	//will load from index (index_WebcamDevice) not from name 

	//get back a list of devices.
	vidGrabber.setVerbose(true);
	vector<ofVideoDevice> _devs = vidGrabber.listDevices();
	index_WebcamDevice.setMax(_devs.size() - 1);

	//get all dev names
	webcam_InputDevices = "";
	for (size_t i = 0; i < _devs.size(); i++)
	{
		//queue devices names
		webcam_InputDevices += "[" + ofToString(i) + "] " + _devs[i].deviceName;
		if (i != _devs.size() - 1) webcam_InputDevices += "\n";
	}

	//-

	//check valid index and clamp
	bool bError = false;
	if (index_WebcamDevice > _devs.size() - 1) {
		ofLogError(__FUNCTION__) << "CAMERA INDEX OUT OF RANGE";
		index_WebcamDevice = -1;
		name_Webcam = _dName = "UNKNOWN DEVICE";
		bError = true;
	}

	if (!bError) {//valid index

		//must close before reopen
		vidGrabber.close();

		//start device
		//if (ENABLE_Webcam.get()) 
		{
			vidGrabber.setDeviceID(index_WebcamDevice.get());
			//vidGrabber.setDesiredFrameRate(60);
			vidGrabber.setup(1920, 1080);
		}

		//--

		//debug connected
		bool _isConnected = vidGrabber.isInitialized();
		ofLogNotice(__FUNCTION__) << "vidGrabber INITIALIZED: " << (_isConnected ? "TRUE" : "FALSE");
		if (!_isConnected) {
			ofLogError(__FUNCTION__) << "CAN'T INITIALIZE vidGrabber!";
			ofLogError(__FUNCTION__) << "CAMERA DISABLED";
			ENABLE_Webcam = false;
		}

		//--

		if (index_WebcamDevice < _devs.size()) {
			_dName = _devs[index_WebcamDevice].deviceName;
			name_Webcam = _dName;
		}
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::setupWebcam() {
	//will initialized the device from the loaded name from the xml settings, not from the index number!
	//that's because the index of a device could be changed if a new device is connected.

	//get back a list of devices.
	vidGrabber.setVerbose(true);
	vector<ofVideoDevice> _devs = vidGrabber.listDevices();

	//get all dev names
	webcam_InputDevices = "";
	index_WebcamDevice.setMax(_devs.size() - 1);
	ofLogNotice(__FUNCTION__) << "LIST WEBCAM DEVICES:";

	//log list devices
	for (size_t i = 0; i < _devs.size(); i++)
	{
		//queue devices names
		webcam_InputDevices += "[" + ofToString(i) + "] " + _devs[i].deviceName;
		if (i != _devs.size() - 1) webcam_InputDevices += "\n";

		if (_devs[i].bAvailable) {
			//log the device
			ofLogNotice(__FUNCTION__) << _devs[i].id << ": " << _devs[i].deviceName;
		}
		else {
			//log the device and note it as unavailable
			ofLogNotice(__FUNCTION__) << _devs[i].id << ": " << _devs[i].deviceName << " - unavailable ";
		}
	}

	//--

	//load settings file

	ofXml _xml;
	bool _isLoaded;
	_isLoaded = _xml.load(path_GLOBAL + path_WebcamSettings);
	ofDeserialize(_xml, _dName);
	ofLogNotice(__FUNCTION__) << _xml.toString();
	ofLogNotice(__FUNCTION__) << "xml device name:\t" << _dName.get();

	//--

	//start device
	//DISABLE_Callbacks = true;

	index_WebcamDevice = -1;
	if (_isLoaded) {
		for (int i = 0; i < _devs.size(); i++) {
			if (_devs[i].deviceName == _dName.get()) {
				index_WebcamDevice = i;
				ofLogNotice(__FUNCTION__) << "device name:\t" << _dName.get();
				ofLogNotice(__FUNCTION__) << "device index:\t" << index_WebcamDevice;
			}
		}
	}
	if (index_WebcamDevice == -1) {//error. try to load first device...
		index_WebcamDevice = 0;//select cam device

		if (index_WebcamDevice < _devs.size()) {
			_dName = _devs[index_WebcamDevice].deviceName;
			name_Webcam = _devs[index_WebcamDevice].deviceName;
		}
		else {
			ofLogError(__FUNCTION__) << "CAMERA INDEX OUT OF RANGE";
			name_Webcam = _dName = "UNKNOWN DEVICE";
		}
	}

	name_Webcam = _dName;

	//--

	//must close before reopen
	vidGrabber.close();

	//start device
	//if (ENABLE_Webcam.get()) 
	{
		vidGrabber.setDeviceID(index_WebcamDevice.get());
		//vidGrabber.setDesiredFrameRate(60);
		vidGrabber.setup(1920, 1080);
	}

	//--

	////debug connected
	//bool _isConnected = vidGrabber.isInitialized();
	//ofLogNotice(__FUNCTION__) << "vidGrabber INITIALIZED: " << (_isConnected ? "TRUE" : "FALSE");
	//if (!_isConnected) {
	//	ofLogError(__FUNCTION__) << "CAN'T INITIALIZE vidGrabber!";
	//	ofLogError(__FUNCTION__) << "CAMERA DISABLED";
	//	ENABLE_Webcam = false;
	//}
}

//--------------------------------------------------------------
void ofxNDIHelper::drawWebcam_Preview() {
	if (ENABLE_Webcam.get())
	{
		ofPushStyle();
		ofSetColor(255, 255);

		ofRectangle r(0, 0, vidGrabber.getWidth(), vidGrabber.getHeight());

		if (!mini_Webcam.get())//full size 
		{
			r.scaleTo(ofGetWindowRect());
			//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_CENTER);
			//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_STRETCH_TO_FILL);
			//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_FILL);

			vidGrabber.draw(r.x, r.y, r.width, r.height);
		}
		else//mini
		{
			int xpad = 10;

			float xx = xpad;
			float yy = vidGrabber.getHeight() * 0.2;
			float ww = vidGrabber.getWidth() * 0.25;
			float hh = vidGrabber.getHeight() * 0.25;

			ofRectangle rw = ofGetWindowRect();
			rw.scaleTo(ofRectangle(xx, yy, ww, hh));

			vidGrabber.draw(rw.x, rw.y, rw.width, rw.height);

			//bb border
			ofNoFill();
			ofSetColor(0);
			ofSetLineWidth(2.0);
			ofDrawRectangle(rw);

			ofDrawBitmapStringHighlight("WEBCAM: " + _dName.get(), rw.x + 4 - 1, rw.y - 5);
		}

		ofPopStyle();
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::drawWebcamOut() {
	ofPushStyle();
	ofSetColor(255, 255);

	ofRectangle r(0, 0, vidGrabber.getWidth(), vidGrabber.getHeight());
	r.scaleTo(ofGetWindowRect());
	//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_CENTER);
	//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_STRETCH_TO_FILL);
	//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_FILL);

	vidGrabber.draw(r.x, r.y, r.width, r.height);

	ofPopStyle();
}

//--------------------------------------------------------------
void ofxNDIHelper::drawWebcamInfo(int x, int y) {
	string str;
	str = "";

	//display all devices
	//display device name
	//str += ">" + name_Webcam.get();// +"\n";
	str += "" + _dName.get() + " [" + ofToString(index_WebcamDevice.get()) + "]";
	//str += " [" + ofToString(index_WebcamDevice.get()) + "]";
	str += " " + ofToString(vidGrabber.isInitialized() ? "[ON]" : "[OFF]") + "\n";
	str += "\n";
	str += "WEBCAM INPUT DEVICES:\n";
	str += webcam_InputDevices + "\n";
	str += "PRESS i: TO SELECT NEXT DEVICE";
	//str += "\nI: restart device";

	drawTextBoxed(str, x, y);
}

//--------------------------------------------------------------
void ofxNDIHelper::exitWebcam() {
	ofXml _xml;
	ofSerialize(_xml, _dName);
	_xml.save(path_GLOBAL + path_WebcamSettings);
}
#endif

//--

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
	x = 50;
	y = 40;

	int l = 20;
	int i = 0;

	//-

//webcam
#ifdef USE_WEBCAM
	if (ENABLE_Webcam.get())
	{
		drawWebcamInfo(x, y);
	}
#endif

	//--

	//ndi in

	i = 0;
	x += 350;
	if (ENABLE_NDI_Input.get()) {

		//string str = "";
		//str += "NDI INPUT DEVICES:\n";
		//char str[256];
		//int _nsendersRemote = ndiReceiver.GetSenderCount();
		//if (_nsendersRemote > 0) {

		//	if (ndiReceiver.ReceiverCreated()) {
		//		if (ndiReceiver.ReceiverConnected()) {
		//			//Show received sender information and received fps
		//			sprintf_s(str, 256, "[%s] (%dx%d/%4.2fp) (fps %2.0f)", ndiReceiver.GetSenderName().c_str(), 
		//				ndiReceiver.GetSenderWidth(), 
		//				ndiReceiver.GetSenderHeight(), 
		//				ndiReceiver.GetSenderFps(), ndiReceiver.GetFps());
		//		}
		//		else {
		//			//Nothing received
		//			sprintf_s(str, 256, "> %s", ndiReceiver.GetSenderName().c_str());
		//		}
		//	}
		//	ofDrawBitmapStringHighlight(str, x, y + l * i++);
		//	//i++;

		//	if (_nsendersRemote == 1) {
		//		ofDrawBitmapStringHighlight("1 NET SOURCE", x, y + l * i++);
		//	}
		//	else {
		//		sprintf_s(str, 256, "%d NET SOURCES.", _nsendersRemote);
		//		string str2(str);
		//		str2 += " SPACE: LIST SENDERS";
		//		ofDrawBitmapStringHighlight(str2, x, y + l * i++);
		//	}
		//}
		//else {
		//	ofDrawBitmapStringHighlight("CONNECTING...", x, y + l * i++);
		//}

		////ndi input devices list
		//if (NDI_InputDevices.size() > 0) ofDrawBitmapStringHighlight(NDI_InputDevices, x, y + l * i++);


		//--

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

	//ndi out

	i = 0;
	//x += 650;
	x = ofGetWidth() - 300;//right

	if (ENABLE_NDI_Output.get()) {
		//Show what it is sending
		if (ndiSender.SenderCreated()) {
			string str = "";
			str += "NDI OUPUT DEVICE:\n";
			str += "PORT NAME: " + ofToString(senderName) + "\n";
			str += ofToString(senderWidth) + "x" + ofToString(senderHeight) + "\n";
			str += "FPS: " + ofToString((int)ofGetFrameRate()) + " APP / " + ofToString(ndiSender.GetFps()) + " NDI";

			drawTextBoxed(str, x, y);
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
void ofxNDIHelper::draw_NDI_IN_Preview() {
	if (ENABLE_NDI_Input.get()) {

		//Receive ofTexture
		ndiReceiver.ReceiveImage(ndiReceiveTexture);//read to texture

		//-

		////draw
		////ndiReceiveTexture.draw(0, 0, ofGetWidth(), ofGetHeight());
		//if (!mini_ndiInput.get()) {
		//	ndiReceiveTexture.draw(0, 0, ofGetWidth(), ofGetHeight());
		//}
		//else {
		//	ndiReceiveTexture.draw(0, ofGetHeight()*0.55, ofGetWidth()*0.25, ofGetHeight()*0.25);
		//}

		//-

		ofPushStyle();
		ofSetColor(255, 255);

		if (!mini_ndiInput.get())//full size 
		{
			ofRectangle r(0, 0, ndiReceiveTexture.getWidth(), ndiReceiveTexture.getHeight());
		
			r.scaleTo(ofGetWindowRect());
			//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_CENTER);
			//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_STRETCH_TO_FILL);
			//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_FILL);

			ndiReceiveTexture.draw(r.x, r.y, r.width, r.height);
		}
		else//mini
		{
			int xpad = 10;

			float xx = xpad;
			float yy = ndiReceiveTexture.getHeight() * 0.5;
			float ww = ndiReceiveTexture.getWidth() * 0.25;
			float hh = ndiReceiveTexture.getHeight() * 0.25;

			ofRectangle rw = ofGetWindowRect();
			rw.scaleTo(ofRectangle(xx, yy, ww, hh));

			ndiReceiveTexture.draw(rw.x, rw.y, rw.width, rw.height);

			//bb border
			ofNoFill();
			ofSetColor(0);
			ofSetLineWidth(2.0);
			ofDrawRectangle(rw);

			ofDrawBitmapStringHighlight("NDI IN", rw.x + 4 - 1, rw.y - 5);
		}

		ofPopStyle();
	}
}

#endif

