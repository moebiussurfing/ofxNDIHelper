#include "ofxNDIHelper.h"

//--------------------------------------------------------------
ofxNDIHelper::ofxNDIHelper()
{
	bDISABLECALLBACKS = true;

	// path for settings
	// this is to folder all files to avoid mixing with other add-ons data
	setPathGlobal("ofxNDIHelper/");

	path_Params_AppSettings = "Settings_App.xml";

#ifdef USE_WEBCAM
	path_WebcamSettings = "Settings_Webcam.xml";
#endif

	setActive(true);// add key and mouse listeners
}

// feed
//--------------------------------------------------------------
void ofxNDIHelper::begin_NDI_OUT()
{
	if (bNDI_Output.get())
	{
		fbo_NDI_Sender.begin();
		ofClear(0, 0, 0, 255);
	}
}
//--------------------------------------------------------------
void ofxNDIHelper::end_NDI_OUT()
{
	if (bNDI_Output.get())
	{
		if (bHelp.get()) ofDrawBitmapStringHighlight("NDI OUT", 40, 60);

		fbo_NDI_Sender.end();

		//--

		// do the sending

		ofColor(255);
		ndiSender.SendImage(fbo_NDI_Sender);
		//ndiSender.SendImage(fbo_NDI_Sender, false);// send processed
		//ndiSender.SendImage(vidGrabber.getPixels());// send clean webcam
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::draw_Preview_NDI_OUT()// TODO: mini
{
	if (bNDI_Output.get())
	{
		ofPushStyle();
		ofSetColor(255, 255);

		if (!bNDI_Output_Mini.get())// full size 
		{
			ofRectangle r(0, 0, fbo_NDI_Sender.getWidth(), fbo_NDI_Sender.getHeight());
			r.scaleTo(ofGetWindowRect());
			//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_CENTER);
			//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_STRETCH_TO_FILL);
			//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_FILL);

			fbo_NDI_Sender.draw(r.x, r.y, r.width, r.height);
		}
		else// mini
		{
			if (bLockRatio.get()) {
				float _ratio = fbo_NDI_Sender.getHeight() / fbo_NDI_Sender.getWidth();
				rectNdiOut.height = rectNdiOut.width * _ratio;
			}
			fbo_NDI_Sender.draw(rectNdiOut.x, rectNdiOut.y, rectNdiOut.width, rectNdiOut.height);
			rectNdiOut.draw();
			// bb border
			ofNoFill();
			ofSetColor(0);
			ofSetLineWidth(2.0);
			ofDrawRectangle(rectNdiOut);
			// info
			if (bHelp) ofDrawBitmapStringHighlight("NDI OUT", rectNdiOut.x + 4 - 1, rectNdiOut.y - 5);
		}

		//fbo_NDI_Sender.draw(0, 0, ofGetWidth(), ofGetHeight());

		ofPopStyle();
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::setup_Params()
{
	bEdit.set("EDIT", true);
	bLockRatio.set("LOCK ASPECT RATIO", true);
	bReset.set("RESET", false);
	bReset.setSerializable(false);

	// NDI in

	bNDI_Input.set("NDI INPUT ENABLE", false);
	bDraw_NDI_Input.set("NDI INPUT DRAW", true);
	bNDI_Input_Mini.set("NDI INPUT MINI", true);
	NDI_Input_Index.set("NDI INPUT DEVICE", 0, 0, 1);
	name_NDI_Input.set("IN", "ofxNDIHelperIN");

	// NDI out

	bNDI_Output.set("NDI OUTPUT ENABLE", false);
	bDraw_NDI_Output.set("NDI OUTPUT DRAW", true);
	bNDI_Output_Mini.set("NDI OUTPUT MINI", true);
	name_NDI_Output.set("OUT", "ofxNDIHelperOUT");

	// exclude from settings
	//?
	//name_NDI_Input.setSerializable(false);
	//name_NDI_Output.setSerializable(false);

	//--

	// Webcam

#ifdef USE_WEBCAM
	bWebcam.set("WEBCAM ENABLE", false);
	bDraw_Webcam.set("WEBCAM DRAW", true);
	mini_Webcam.set("WEBCAM MINI", true);
	index_WebcamDevice.set("WEBCAM DEVICE", 0, 0, 1);
	name_Webcam.set("", "");

	params_Webcam.add(bWebcam);
	params_Webcam.add(bDraw_Webcam);
	params_Webcam.add(mini_Webcam);
	params_Webcam.add(index_WebcamDevice);
	params_Webcam.add(name_Webcam);

	index_WebcamDevice.setSerializable(false);
	name_Webcam.setSerializable(false);
#endif

	//--

	// NDI in

	params_NDI_Input.add(bNDI_Input);
	params_NDI_Input.add(bDraw_NDI_Input);
	params_NDI_Input.add(bNDI_Input_Mini);
	params_NDI_Input.add(NDI_Input_Index);
	params_NDI_Input.add(name_NDI_Input);

	// NDI out

	params_NDI_Output.add(bNDI_Output);
	params_NDI_Output.add(bDraw_NDI_Output);
	params_NDI_Output.add(bNDI_Output_Mini);
	params_NDI_Output.add(name_NDI_Output);

	bHelp.set("HELP", true);

	rectNdiOut.enableEdit();
	rectNdiOut.setRect(200, 200, 200, 400);

	//-

	// group

	// Gui params

	params_User.setName("NDI HELPER");
	params_User.add(bEdit);
	params_User.add(bHelp);
	params_User.add(bDebug);
	params_User.add(bLockRatio);
	params_User.add(bReset);

	// Webcam

#ifdef USE_WEBCAM
	params_User.add(params_Webcam);
#endif
	params_User.add(params_NDI_Input);
	params_User.add(params_NDI_Output);

	//-

	params_AppsSettings.add(params_User);

	//-

	// queue more into params_AppsSettings

	// internal

	bGui.set("GUI", true);
	bGui_Controls.set("CONTROLS", true);
	bActive.set("ACTIVE", true);
	bKeys.set("KEYS", true);
	bDebug.set("DEBUG", true);
	bAutoSave.set("AUTO SAVE", true);
	position_Gui.set("GUI POSITION",
		glm::vec2(screenW * 0.5, screenH * 0.5),
		glm::vec2(0, 0),
		glm::vec2(screenW, screenH)
	);
	//MODE_App.set("APP MODE", 0, 0, NUM_MODES_APP - 1);
	//MODE_App_Name.set("", "");
	//MODE_App_Name.setSerializable(false);

	// params internal

	params_Internal.setName("INTERNAL");
	params_Internal.add(bActive);
	params_Internal.add(bKeys);
	params_Internal.add(bHelp);
	params_Internal.add(bDebug);
	params_Internal.add(bAutoSave);
	params_Internal.add(bGui_Controls);
	params_Internal.add(position_Gui);
	//params_Internal.add(MODE_App);
	//params_Internal.add(MODE_App_Name);

	// group all
	params_AppsSettings.add(params_Internal);

	//-

	// callback

	ofAddListener(params_AppsSettings.parameterChangedE(), this, &ofxNDIHelper::Changed_Params_AppSettings);
}

//--------------------------------------------------------------
void ofxNDIHelper::setup()
{
	// log mode
	ofSetLogLevel("ofxNDIHelper", OF_LOG_NOTICE);

	// gui help display font
	//string _str = "overpass-mono-bold.otf";
	string _str = "telegrama_render.otf";
	string _pathFont = "assets/fonts/" + _str;
	float _size = 7;
	bool b = font.load(_pathFont, _size);
	if (!b) b = font.load(OF_TTF_MONO, _size);

	//--

	// window
	screenW = ofGetWidth();
	screenH = ofGetHeight();

	//--

	setup_Params();

	//--

	// gui

	// ofxGui theme
	//string strG = "overpass-mono-bold.otf";
	string strG = "telegrama_render.otf";
	string pathFontG = "assets/fonts/" + _str;

	ofxSurfingHelpers::setThemeSurfing_ofxGui(pathFontG, 7);

	//--

	// setup gui

	gui_User.setup("ofxNDIHelper");
	gui_User.add(params_User);

	// collapse groups
	auto &g0 = gui_User.getGroup(params_User.getName());// 1st level
	auto &g1 = g0.getGroup(params_Webcam.getName());
	auto &g2 = g0.getGroup(params_NDI_Input.getName());
	auto &g3 = g0.getGroup(params_NDI_Output.getName());
	g1.minimize();
	g2.minimize();
	g3.minimize();

	//--

	// sources / modules
	// must be called before the loading of settings bc how callbacks are running..
#ifdef USE_WEBCAM
	setup_Webcam();
#endif

	bActive = true;

#ifdef USE_ofxNDI_IN
	setup_NDI_IN();
#endif

	//--

#ifdef USE_ofxNDI_OUT
	setup_NDI_OUT();
#endif

	//--

	startup();
}

//--------------------------------------------------------------
void ofxNDIHelper::startup()
{
	ofLogNotice(__FUNCTION__);

	//----

	// startup

	bDISABLECALLBACKS = false;

	bActive = true;

	// set gui position after window setup/resizing
	windowResized(screenW, screenH);

	position_Gui.set(glm::vec2(ofGetWidth() - 210, 10));

	// file settings
	loadParams(params_AppsSettings, path_GLOBAL + path_Params_AppSettings);

	//-----

	refresh_NDI_IN();

	//-

	// workaround
	if (bWebcam.get()) {
		bDoRestartup = true;
		bWebcam = false;
		vidGrabber.close();
	}

	// mini preview rectangles positions and sizes
	reset_Mini_Previews();
	rectNdiIn.loadSettings(path_rectNdiIn, path_GLOBAL, false);
	rectNdiOut.loadSettings(path_rectNdiOut, path_GLOBAL, false);
	rectWebcam.loadSettings(path_rectWebcam, path_GLOBAL, false);
}

//--------------------------------------------------------------
void ofxNDIHelper::update()
{
	//-

#ifdef USE_WEBCAM

	// workaround
	// restart camera after startup to make sure will be initialized fine.
	float _timeWait = 5.0;
	if (bDoRestartup && (ofGetFrameNum() == (int)(_timeWait * 60))) {// in x seconds at 60fps
		//vidGrabber.close();
		restart_Webcam();
		bWebcam = true;
	}

	//-

	if (bWebcam.get())
	{
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

	// autosave
	// bAutoSave = false;
	if (bAutoSave && ofGetElapsedTimeMillis() - timerLast_Autosave > timeToAutosave)
	{
		ofLogLevel _logPre = ofGetLogLevel();
		ofSetLogLevel(OF_LOG_SILENT);// bypass this logs

		bDISABLECALLBACKS = true;

		// get gui position before save
		position_Gui = glm::vec2(gui_User.getPosition());
		saveParams(params_AppsSettings, path_GLOBAL + path_Params_AppSettings);

		bDISABLECALLBACKS = false;

		timerLast_Autosave = ofGetElapsedTimeMillis();
		ofLogNotice(__FUNCTION__) << "Autosaved DONE";

		ofSetLogLevel(_logPre);
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::draw() {
	if (!bGui) return;

	//-

	// NDI out
	if (bNDI_Output.get() && bDraw_NDI_Output.get()) {
		draw_Preview_NDI_OUT();
	}

	//-

	// NDI in
	if (bNDI_Input.get() && bDraw_NDI_Input.get()) {
		draw_Preview_NDI_IN();
	}

	//-

	// Webcam
#ifdef USE_WEBCAM
	if (bWebcam.get() && bDraw_Webcam.get())
	{
		draw_Preview_Webcam();
	}
#endif
}

//--------------------------------------------------------------
void ofxNDIHelper::draw_Gui()
{
	if (!bGui) return;

	if (bGui_Controls)
	{
		// edit mode
		//if (MODE_App == 1)

		if (bHelp)
		{
			if (bHelp.get())
			{
				// Show what it is receiving
				draw_InfoDevices();
			}
		}

		//-

		// gui panel
		gui_User.draw();
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::exit()
{
	// Webcam

#ifdef USE_WEBCAM
	exit_Webcam();
#endif

	//-

#ifdef USE_ofxNDI_OUT
	// The sender must be released 
	// or NDI sender discovery will still find it
	ndiSender.ReleaseSender();
#endif

	//--

	// get gui position before save
	position_Gui = glm::vec2(gui_User.getPosition());

	saveParams(params_AppsSettings, path_GLOBAL + path_Params_AppSettings);

	// viewports
	rectNdiIn.saveSettings(path_rectNdiIn, path_GLOBAL, false);
	rectNdiOut.saveSettings(path_rectNdiOut, path_GLOBAL, false);
	rectWebcam.saveSettings(path_rectWebcam, path_GLOBAL, false);
}

//--------------------------------------------------------------
ofxNDIHelper::~ofxNDIHelper()
{
	setActive(false);// remove keys and mouse listeners

	// remove params callbacks listeners
	ofRemoveListener(params_AppsSettings.parameterChangedE(), this, &ofxNDIHelper::Changed_Params_AppSettings);

	exit();
}

//--------------------------------------------------------------
void ofxNDIHelper::setLogLevel(ofLogLevel level)
{
	ofSetLogLevel("ofxNDIHelper", level);
}


//--------------------------------------------------------------
void ofxNDIHelper::windowResized(int w, int h)
{
	screenW = w;
	screenH = h;

	//ndiSender.UpdateSender(1920, 1080);//update size
}

// keys
//--------------------------------------------------------------
void ofxNDIHelper::keyPressed(ofKeyEventArgs &eventArgs)
{
	const int &key = eventArgs.key;
	ofLogNotice(__FUNCTION__) << "keyPressed: " << (char)key << " [" << key << "]";

	// modifiers
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

	// disabler for all keys. (independent from bActive)
	if (bKeys)
	{
		////custom
		//if (key == ' ')
		//{
		//}
		//else if (key == ' ')
		//{
		//}

		////custom with modifiers
		//if (key == OF_KEY_UP && mod_ALT)
		//{
		//	ofLogNotice(__FUNCTION__) << "KEY UP";
		//}
		//else if (key == OF_KEY_UP)
		//{
		//	ofLogNotice(__FUNCTION__) << "";
		//}

		// general

		if (0) {}

		//if (key == key_MODE_App)
		//{
		//	int i = MODE_App;
		//	i++;
		//	MODE_App = i % NUM_MODES_APP;
		//}

		else if (key == 'G')
		{
			bGui_Controls = !bGui_Controls;
		}
		else if (key == 'H')
		{
			bHelp = !bHelp;
		}
		//else if (key == 'D')
		//{
		//	bDebug = !bDebug;
		//}

		//----

		// edit mode
		//if (MODE_App == 1)
		if (bEdit)
		{
			//----

			// NDI input

#ifdef USE_ofxNDI_IN
			char name[256];
			int index = key - 48;// select by key numbers

			int _nsendersRemote = ndiReceiver.GetSenderCount();
			NDI_Input_Index.setMax(_nsendersRemote - 1);

			if (key == ' ') {
				refresh_NDI_IN();
			}

			// numbers
			else if (_nsendersRemote > 0 && index >= 0 && index < _nsendersRemote) {
				// Update the receiver with the returned index
				// Returns false if the current sender is selected
				if (ndiReceiver.SetSenderIndex(index)) {
					NDI_Input_Index = index;
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

			// Webcam

#ifdef USE_WEBCAM
			if (key == 'I') {
				// Webcam
				auto _devs = vidGrabber.listDevices();
				index_WebcamDevice++;
				if (index_WebcamDevice.get() > _devs.size() - 1) index_WebcamDevice = 0;
				_dName = _devs[index_WebcamDevice].deviceName;
				name_Webcam = _dName;

				// select cam device
				vidGrabber.close();
				vidGrabber.setDeviceID(index_WebcamDevice.get());
				vidGrabber.setup(1920, 1080);
			}
#endif
			//----
		}
	}

	//--

	// key enabler
	if (key == 'K')
	{
		bKeys = !bKeys;
		ofLogNotice(__FUNCTION__) << "KEYS: " << (bKeys ? "ON" : "OFF");

		if (!bKeys)
		{
			ofLogNotice(__FUNCTION__) << "ALL KEYS DISABLED. PRESS 'k' TO ENABLE GAIN!";
		}
		else
		{
			ofLogNotice(__FUNCTION__) << "KEYS ENABLED BACK";
		}
	}

	//---
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

// mouse
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


//--------------------------------------------------------------
void ofxNDIHelper::setActive(bool b)
{
	// * disables all keys and mouse interaction listeners from the addon

	bActive = b;
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
	bGui_Controls = b;
}

//--------------------------------------------------------------
void ofxNDIHelper::setGuiToggleVisible()
{
	bGui_Controls = !bGui_Controls;
}

// addon engine params
//--------------------------------------------------------------
void ofxNDIHelper::Changed_Params_AppSettings(ofAbstractParameter &e)
{
	if (!bDISABLECALLBACKS)
	{
		string name = e.getName();

		// exclude debugs

		if (name != "exclude"
			&& name != "exclude")
		{
			ofLogNotice(__FUNCTION__) << name << " : " << e;
		}

		//--

		if (0) {}

		else if (name == bEdit.getName())
		{
			if (bEdit.get())
			{
				//MODE_App = 1;// edit

				if (bNDI_Input.get()) {
					refresh_NDI_IN();
				}

				rectNdiIn.enableEdit();
				rectNdiOut.enableEdit();
				rectWebcam.enableEdit();

				//--

#ifdef USE_WEBCAM
				//if (bWebcam.get()) {
				//	setup_Webcam();
				//}
#endif
				//--
			}
			else
			{
				//MODE_App = 0;

				rectNdiIn.disableEdit();
				rectNdiOut.disableEdit();
				rectWebcam.disableEdit();
			}
		}
		else if (name == bReset.getName() && bReset)
		{
			bReset = false;

			reset_Mini_Previews();
		}

		//--

		// Webcam

#ifdef USE_WEBCAM
		else if (name == bWebcam.getName())
		{
			if (bWebcam.get()) restart_Webcam();
			else vidGrabber.close();
		}
		else if (name == index_WebcamDevice.getName() && bWebcam.get())
		{
			setup_Webcam(index_WebcamDevice.get());
		}
#endif

		//--

		// NDI in 

		else if (name == bNDI_Input.getName() && bNDI_Input.get())
		{
			setup_NDI_IN();

			//refresh_NDI_IN();
		}
		else if (name == NDI_Input_Index.getName() && bNDI_Input.get())
		{
			setup_NDI_IN();

			//refresh_NDI_IN();

			//int _nsendersRemote = ndiReceiver.GetSenderCount();
			//NDI_Input_Index.setMax(_nsendersRemote - 1);
			//int index = NDI_Input_Index.get();
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
		//else if (name == "NDI_IN" && bNDI_Input.get())
		//{
		//}

		//--

		// NDI out

		else if (name == bNDI_Output.getName())
		{
			if (bNDI_Output.get())
			{
				//setup_NDI_OUT();
			}
			else
			{
				//ndiSender.ReleaseSender();//?
			}
		}
		else if (name == bNDI_Output.getName() && bNDI_Output.get())
		{
			//setup_NDI_OUT();
		}

		//----

		else if (name == position_Gui.getName())
		{
			gui_User.setPosition(position_Gui.get().x, position_Gui.get().y);
		}
		else if (name == bActive.getName())
		{
			setActive(bActive);
		}
	}
}

////--------------------------------------------------------------
//void ofxNDIHelper::setKey_MODE_App(int k)
//{
//	key_MODE_App = k;
//}

//--------------------------------------------------------------
void ofxNDIHelper::setPathGlobal(string s)// must call before setup. disabled by default
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

// Webcam
#ifdef USE_WEBCAM

// TODO:
//--------------------------------------------------------------
void ofxNDIHelper::restart_Webcam() {
	// must close before reopen
	vidGrabber.close();

	// start device
	//if (bWebcam.get()) 
	{
		vidGrabber.setDeviceID(index_WebcamDevice.get());
		//vidGrabber.setDesiredFrameRate(60);
		vidGrabber.setup(1920, 1080);
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::setup_Webcam(int index) {
	// will load from index (index_WebcamDevice) not from name 

	// get back a list of devices.
	vidGrabber.setVerbose(true);
	vector<ofVideoDevice> _devs = vidGrabber.listDevices();
	index_WebcamDevice.setMax(_devs.size() - 1);

	// get all dev names
	name_Webcam_InputDevices = "";
	for (size_t i = 0; i < _devs.size(); i++)
	{
		// queue devices names
		name_Webcam_InputDevices += "[" + ofToString(i) + "] " + _devs[i].deviceName;
		if (i != _devs.size() - 1) name_Webcam_InputDevices += "\n";
	}

	//-

	// check valid index and clamp
	bool bError = false;
	if (index_WebcamDevice > _devs.size() - 1) {
		ofLogError(__FUNCTION__) << "CAMERA INDEX OUT OF RANGE";
		index_WebcamDevice = -1;
		name_Webcam = _dName = "UNKNOWN DEVICE";
		bError = true;
	}

	if (!bError) {// valid index

		// must close before reopen
		vidGrabber.close();

		// start device
		//if (bWebcam.get()) 
		{
			vidGrabber.setDeviceID(index_WebcamDevice.get());
			//vidGrabber.setDesiredFrameRate(60);
			vidGrabber.setup(1920, 1080);
		}

		//--

		// debug connected
		bool _isConnected = vidGrabber.isInitialized();
		ofLogNotice(__FUNCTION__) << "vidGrabber INITIALIZED: " << (_isConnected ? "TRUE" : "FALSE");
		if (!_isConnected) {
			ofLogError(__FUNCTION__) << "CAN'T INITIALIZE vidGrabber!";
			ofLogError(__FUNCTION__) << "CAMERA DISABLED";
			bWebcam = false;
		}

		//--

		if (index_WebcamDevice < _devs.size()) {
			_dName = _devs[index_WebcamDevice].deviceName;
			name_Webcam = _dName;
		}
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::setup_Webcam() {
	// will initialized the device from the loaded name from the xml settings, not from the index number!
	// that's because the index of a device could be changed if a new device is connected.

	// get back a list of devices.
	vidGrabber.setVerbose(true);
	vector<ofVideoDevice> _devs = vidGrabber.listDevices();

	// get all dev names
	name_Webcam_InputDevices = "";
	index_WebcamDevice.setMax(_devs.size() - 1);
	ofLogNotice(__FUNCTION__) << "LIST WEBCAM DEVICES:";

	// log list devices
	for (size_t i = 0; i < _devs.size(); i++)
	{
		// queue devices names
		name_Webcam_InputDevices += "[" + ofToString(i) + "] " + _devs[i].deviceName;
		if (i != _devs.size() - 1) name_Webcam_InputDevices += "\n";

		if (_devs[i].bAvailable) {
			// log the device
			ofLogNotice(__FUNCTION__) << _devs[i].id << ": " << _devs[i].deviceName;
		}
		else {
			// log the device and note it as unavailable
			ofLogNotice(__FUNCTION__) << _devs[i].id << ": " << _devs[i].deviceName << " - unavailable ";
		}
	}

	//--

	// load settings file

	ofXml _xml;
	bool _isLoaded;
	_isLoaded = _xml.load(path_GLOBAL + path_WebcamSettings);
	ofDeserialize(_xml, _dName);
	ofLogNotice(__FUNCTION__) << _xml.toString();
	ofLogNotice(__FUNCTION__) << "xml device name:\t" << _dName.get();

	//--

	// start device
	//bDISABLECALLBACKS = true;

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
	if (index_WebcamDevice == -1) {// error. try to load first device...
		index_WebcamDevice = 0;// select cam device

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

	// must close before reopen
	vidGrabber.close();

	// start device
	//if (bWebcam.get()) 
	{
		vidGrabber.setDeviceID(index_WebcamDevice.get());
		//vidGrabber.setDesiredFrameRate(60);
		vidGrabber.setup(1920, 1080);
	}

	//--

	//// debug connected
	//bool _isConnected = vidGrabber.isInitialized();
	//ofLogNotice(__FUNCTION__) << "vidGrabber INITIALIZED: " << (_isConnected ? "TRUE" : "FALSE");
	//if (!_isConnected) {
	//	ofLogError(__FUNCTION__) << "CAN'T INITIALIZE vidGrabber!";
	//	ofLogError(__FUNCTION__) << "CAMERA DISABLED";
	//	bWebcam = false;
	//}
}

//--------------------------------------------------------------
void ofxNDIHelper::draw_Preview_Webcam() {
	if (bWebcam.get())
	{
		ofPushStyle();
		ofSetColor(255, 255);

		ofRectangle r(0, 0, vidGrabber.getWidth(), vidGrabber.getHeight());

		if (!mini_Webcam.get())// full size 
		{
			r.scaleTo(ofGetWindowRect());
			//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_CENTER);
			//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_STRETCH_TO_FILL);
			//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_FILL);

			vidGrabber.draw(r.x, r.y, r.width, r.height);
		}
		else// mini
		{
			if (bLockRatio.get()) {
				float _ratio = vidGrabber.getHeight() / vidGrabber.getWidth();
				rectWebcam.height = rectWebcam.width * _ratio;
			}
			vidGrabber.draw(rectWebcam.x, rectWebcam.y, rectWebcam.width, rectWebcam.height);
			rectWebcam.draw();
			//bb border
			ofNoFill();
			ofSetColor(0);
			ofSetLineWidth(2.0);
			ofDrawRectangle(rectWebcam);
			//info
			if (bHelp) ofDrawBitmapStringHighlight("WEBCAM: " + _dName.get(), rectWebcam.x + 4 - 1, rectWebcam.y - 5);
		}

		ofPopStyle();
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::reset_Mini_Previews() {

	// reset mini previews layout

	float _xx = xPadPreview;
	float _yy = yPadPreview;
	float _ratio;
	float _pad = 60;

	_ratio = ndiReceiveTexture.getHeight() / ndiReceiveTexture.getWidth();
	rectNdiIn.width = wPreview;
	rectNdiIn.height = rectNdiIn.width * _ratio;
	rectNdiIn.x = _xx;
	rectNdiIn.y = _yy;

	_yy += _pad + rectNdiIn.height;
	_ratio = fbo_NDI_Sender.getHeight() / fbo_NDI_Sender.getWidth();
	rectNdiOut.width = wPreview;
	rectNdiOut.height = rectNdiOut.width * _ratio;
	rectNdiOut.x = _xx;
	rectNdiOut.y = _yy;

	_yy += _pad + rectNdiOut.height;
	_ratio = vidGrabber.getHeight() / vidGrabber.getWidth();
	rectWebcam.width = wPreview;
	rectWebcam.height = rectWebcam.width * _ratio;
	rectWebcam.x = _xx;
	rectWebcam.y = _yy;
}

//--------------------------------------------------------------
void ofxNDIHelper::draw_WebcamOut() {
	ofPushStyle();
	ofSetColor(255, 255);

	ofRectangle r(0, 0, vidGrabber.getWidth(), vidGrabber.getHeight());
	r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_FILL);
	//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_CENTER);
	//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_STRETCH_TO_FILL);
	//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_FILL);

	vidGrabber.draw(r.x, r.y, r.width, r.height);

	ofPopStyle();
}

//--------------------------------------------------------------
void ofxNDIHelper::exit_Webcam() {
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

	// Set the dimensions of the sender output here
	// This is independent of the size of the display window
	// Sender dimensions and fps are not known yet
	//senderWidth = (unsigned char)ofGetWidth();
	//senderHeight = (unsigned char)ofGetHeight();
	senderWidth = 1920;
	senderHeight = 1080;

	// fbo settings
	ofFbo::Settings settings;
	settings.internalformat = GL_RGBA;
	settings.width = senderWidth;
	settings.height = senderHeight;
	fbo_NDI_Sender.allocate(settings);

	fbo_NDI_Sender.begin();
	ofClear(0, 0, 0, 255);
	fbo_NDI_Sender.end();

	//// Create an RGBA fbo for collection of data
	//fbo_NDI_Sender.allocate(senderWidth, senderHeight, GL_RGBA);

	// Optionally set fbo readback using OpenGL pixel buffers
	ndiSender.SetReadback(); // Change to false to compare

	// Set NDI asynchronous sending for best performance
	ndiSender.SetAsync();

	////Create a new sender - default RGBA for ofPixels
	//ndiSender.CreateSender("Openframeworks NDI webcam", (unsigned int)vidGrabber.getWidth(), (unsigned int)vidGrabber.getHeight());
	//ofLogNotice(__FUNCTION__) << "Created NDI sender [Openframeworks NDI webcam] (" << vidGrabber.getWidth() << "x" << vidGrabber.getHeight() << ")" ;

	// Create a sender with default RGBA output format
	ndiSender.CreateSender(senderName, senderWidth, senderHeight);
	//ndiSender.CreateSender("OF TEST", senderWidth, senderHeight);

	ofLogNotice(__FUNCTION__) << "Created NDI sender [" << senderName << "] (" << senderWidth << "x" << senderHeight << ")";
}
#endif


#ifdef USE_ofxNDI_IN

//--------------------------------------------------------------
void ofxNDIHelper::draw_InfoDevices() {

	float _padx = 14;
	float _pady = 23;
	int x, y;

	//-

	// Webcam

#ifdef USE_WEBCAM
	if (bWebcam.get())
	{
		string str;
		str = "";
		str += "WEBCAM INPUT DEVICES\n";
		// display all devices
		// display device name
		//_str += ">" + name_Webcam.get();// +"\n";
		str += "" + _dName.get() + " [" + ofToString(index_WebcamDevice.get()) + "]";
		//_str += " [" + ofToString(index_WebcamDevice.get()) + "]";
		str += " " + ofToString(vidGrabber.isInitialized() ? "[ON]" : "[OFF]") + "\n";
		str += "\n";
		str += name_Webcam_InputDevices + "\n";
		str += "PRESS i: TO SELECT NEXT DEVICE";
		//_str += "\nI: restart device";

		x = rectWebcam.x + _padx;
		y = rectWebcam.y + rectWebcam.getHeight() + _pady;

		drawTextBoxed(str, x, y, rounded);
	}
#endif

	//--

	// NDI in

	if (bNDI_Input.get()) {
		string str = "";
		str += "NDI INPUT DEVICES\n";

		int nsenders = ndiReceiver.GetSenderCount();
		if (nsenders > 0) {
			if (ndiReceiver.ReceiverCreated()) {
				if (ndiReceiver.ReceiverConnected()) {
					// Show received sender information and received fps
					str += "(" + ofToString(ndiReceiver.GetSenderName().c_str()) + ")";
					str += "\n(" + ofToString(ndiReceiver.GetSenderWidth());
					str += "x" + ofToString(ndiReceiver.GetSenderHeight());
					str += "/" + ofToString(ndiReceiver.GetSenderFps()) + ")";
					str += " (" + ofToString(ndiReceiver.GetFps()) + " FPS)\n";
				}
				else {
					// Nothing received
					str += "> " + ofToString(ndiReceiver.GetSenderName().c_str());
				}
			}

			if (nsenders == 1) {
				str += "\n1 NET SOURCE";
			}
			else {
				str += "\n" + ofToString(nsenders) + " NET SOURCES.";
				str += " SPACE: LIST SENDERS\n";
			}
		}
		else {
			str += "CONNECTING...\n";
		}
		// NDI input devices list
		if (name_NDI_InputDevices.size() > 0) str += name_NDI_InputDevices;

		//--

		x = rectNdiIn.x + _padx;
		y = rectNdiIn.y + rectNdiIn.getHeight() + _pady;

		drawTextBoxed(str, x, y, rounded);
	}

	//-

	// NDI out

	if (bNDI_Output.get()) {
		// Show what it's sending
		if (ndiSender.SenderCreated()) {
			string str = "";
			str += "NDI OUPUT DEVICE\n";
			str += "PORT NAME: " + ofToString(senderName) + "\n";
			str += ofToString(senderWidth) + "x" + ofToString(senderHeight) + "\n";
			str += "FPS: " + ofToString((int)ofGetFrameRate()) + " APP / " + ofToString(ndiSender.GetFps()) + " NDI";

			x = rectNdiOut.x + _padx;
			y = rectNdiOut.y + rectNdiOut.getHeight() + _pady;

			drawTextBoxed(str, x, y, rounded);
		}
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::refresh_NDI_IN() {
	//// deep setup
	//setup_NDI_IN();

	char name[256];
	int nsenders = ndiReceiver.GetSenderCount();
	NDI_Input_Index.setMax(nsenders - 1);

	// List all the senders
	if (nsenders > 0) {
		ofLogNotice(__FUNCTION__) << "Number of NDI senders found: " << nsenders;
		name_NDI_InputDevices = "";
		for (int i = 0; i < nsenders; i++) {
			string name = ndiReceiver.GetSenderName(i);
			string str;
			str = "[" + ofToString(i) + "] " + name;
			ofLogNotice(__FUNCTION__) << str;
			name_NDI_InputDevices += str;

			if (i != nsenders - 1) name_NDI_InputDevices += "\n";
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

	// TODO:

	// ofFbo
	//// receiver dimensions and fps are not known yet
	//receiverWidth = (unsigned char)ofGetWidth();
	//receiverHeight = (unsigned char)ofGetHeight();
	receiverWidth = 1920;
	receiverHeight = 1080;

	fbo_NDI_Receiver.allocate(receiverWidth, receiverHeight, GL_RGBA);

	// Clear the fbo so the first frame draw is black
	fbo_NDI_Receiver.begin();
	ofClear(0, 0, 0, 255);
	fbo_NDI_Receiver.end();

	// ofTexture
	ndiReceiveTexture.allocate(receiverWidth, receiverHeight, GL_RGBA);
	//// ofImage
	//ndiReceiveImage.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR_ALPHA);
	//// ofPixels
	//ndiReceivePixels.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR_ALPHA);

	// unsigned char pixels
	ndiReceiveChars = new unsigned char[receiverWidth*receiverHeight * 4];

	//NDI_Input_Index.setMax(ndiReceiver.GetSenderCount() - 1);


	refresh_NDI_IN();


	int nsenders = ndiReceiver.GetSenderCount();
	NDI_Input_Index.setMax(nsenders - 1);
	int index = NDI_Input_Index.get();
	char name[256];

	if (nsenders > 0 && index >= 0 && index < nsenders) {
		// Update the receiver with the returned index
		// Returns false if the current sender is selected
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
void ofxNDIHelper::draw_Preview_NDI_IN() {
	if (bNDI_Input.get()) {

		// Receive ofTexture
		ndiReceiver.ReceiveImage(ndiReceiveTexture);//read to texture

		//-

		//// draw
		////ndiReceiveTexture.draw(0, 0, ofGetWidth(), ofGetHeight());
		//if (!bNDI_Input_Mini.get()) {
		//	ndiReceiveTexture.draw(0, 0, ofGetWidth(), ofGetHeight());
		//}
		//else {
		//	ndiReceiveTexture.draw(0, ofGetHeight()*0.55, ofGetWidth()*0.25, ofGetHeight()*0.25);
		//}

		//-

		ofPushStyle();
		ofSetColor(255, 255);

		if (!bNDI_Input_Mini.get())// full size 
		{
			ofRectangle r(0, 0, ndiReceiveTexture.getWidth(), ndiReceiveTexture.getHeight());

			r.scaleTo(ofGetWindowRect());
			//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_CENTER);
			//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_STRETCH_TO_FILL);
			//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_FILL);

			ndiReceiveTexture.draw(r.x, r.y, r.width, r.height);
		}
		else// mini
		{
			if (bLockRatio.get()) {
				float _ratio = ndiReceiveTexture.getHeight() / ndiReceiveTexture.getWidth();
				rectNdiIn.height = rectNdiIn.width * _ratio;
			}
			ndiReceiveTexture.draw(rectNdiIn.x, rectNdiIn.y, rectNdiIn.width, rectNdiIn.height);
			rectNdiIn.draw();
			// bb border
			ofNoFill();
			ofSetColor(0);
			ofSetLineWidth(2.0);
			ofDrawRectangle(rectNdiIn);
			// info
			if (bHelp) ofDrawBitmapStringHighlight("NDI In", rectNdiIn.x + 4 - 1, rectNdiIn.y - 5);
		}

		ofPopStyle();
	}
}

#endif

