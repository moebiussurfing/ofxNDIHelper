#include "ofxNDIHelper.h"

//--------------------------------------------------------------
ofxNDIHelper::ofxNDIHelper()
{
	bDISABLECALLBACKS = true;

	// Path for settings
	// This is to folder all files to avoid mixing with other add-ons data
	setPathGlobal("ofxNDIHelper/");

	path_Params_AppSettings = "Settings_App";

#ifdef USE_WEBCAM
	path_WebcamSettings = "Settings_Webcam.xml";
#endif

	//--

	// Help box
	textBoxWidget.setPath(path_GLOBAL + "HelpBox/");
	textBoxWidget.setup();

	// text
	std::string helpInfo = "";
	helpInfo += "HELP \n";
	helpInfo += "NDI MANAGER \n";
	helpInfo += "\n";
	helpInfo += "KEYS \n";
	helpInfo += "H              HELP \n";
	helpInfo += "E              EDIT \n";
	helpInfo += "I              WEBCAM NEXT \n";
	helpInfo += "SPACE          NDI INPUT SCAN! \n";
	helpInfo += "D              DEBUG \n";
	//helpInfo += "K              KEYS \n";

	textBoxWidget.setText(helpInfo);

	//--

	setActive(true); // add key and mouse listeners

	ofAddListener(ofEvents().update, this, &ofxNDIHelper::update);
}

//--------------------------------------------------------------
ofxNDIHelper::~ofxNDIHelper()
{
	setActive(false); // remove keys and mouse listeners

	// remove params callbacks listeners
	ofRemoveListener(params_AppsSettings.parameterChangedE(), this, &ofxNDIHelper::Changed);

	ofRemoveListener(ofEvents().update, this, &ofxNDIHelper::update);

	exit();
}

//--

//--------------------------------------------------------------
void ofxNDIHelper::setup()
{
	// log mode
	ofSetLogLevel("ofxNDIHelper", OF_LOG_NOTICE);

	// gui display font
	string _str = "telegrama_render.otf";
	string _pathFont = "assets/fonts/" + _str;

	float _size = 7;
	bool b = font.load(_pathFont, _size);
	if (!b) b = font.load(OF_TTF_MONO, _size);

	_size = 9;
	b = fontBig.load(_pathFont, _size);
	if (!b) b = fontBig.load(OF_TTF_MONO, _size);

	//--

	// window
	screenW = ofGetWidth();
	screenH = ofGetHeight();

	//--

	// Gui

	// ofxGui Theme

	ofxSurfingHelpers::setThemeDarkMini_ofxGui();

	//string strG = "telegrama_render.otf";
	//string pathFontG = "assets/fonts/" + _str;
	//ofxSurfingHelpers::setThemeDark_ofxGui(pathFontG, 7);

	//--

	setup_Params();

	//--

	// Inputs
	// sources / modules
	// must be called before the loading of settings bc how callbacks are listening...

#ifdef USE_WEBCAM

	setup_Webcam();

#endif

	//--

#ifdef USE_ofxNDI_IN

	NDI_Input1.setPathGlobal("NDI_Input1/");
	NDI_Input1.setup("macOS");

	NDI_Input2.setPathGlobal("NDI_Input2/");
	NDI_Input2.setup("RPi");

#endif

	//--

	// Outputs

#ifdef USE_ofxNDI_OUT

	setup_NDI_OUT();

#endif

	//--

	// Group all
	// this group will be saved as settings to a file on /data

	//--

	// Settings file
	params_AppsSettings.setName("NDI Helper");
	params_AppsSettings.add(bGui);
	params_AppsSettings.add(params_Internal);
	params_AppsSettings.add(params_User);
	//params_AppsSettings.add(bGui_Controls);

//#ifdef USE_WEBCAM
//	params_User.add(params_Webcam);
//#endif

	//--

	// Setup Gui

	gui_Controls.setup("ofxNDIHelper");
	gui_Controls.add(params_User);
	gui_Controls.add(params_Internal);

	//--

	// collapse groups

	auto& g0 = gui_Controls.getGroup(params_User.getName());// 1st level
	auto& g1 = g0.getGroup(params_Webcam.getName());
	auto& g3 = g0.getGroup(params_NDI_Output.getName());
	g1.minimize();
	g3.minimize();

	auto& gi = gui_Controls.getGroup(params_Internal.getName());
	auto& gp = gi.getGroup(position_Gui.getName());
	gi.minimize();
	gp.minimize();

	//--

	// Startup

	startup();
}

//--

//--------------------------------------------------------------
void ofxNDIHelper::setup_Params()
{
	bGui.set("NDI HELPER", true);
	//bGui_Controls.set("NDI CONTROLS", true);
	bEdit.set("LAYOUT EDIT", true);
	bReset.set("LAYOUT RESET", false);
	bLockRatio.set("LOCK ASPECT RATIO", true);
	bKeys.set("KEYS", true);
	bDebug.set("DEBUG", true);
	bReset.setSerializable(false);

	// Internal
	bActive.set("ACTIVE", true);
	bActive.setSerializable(false);
	bHelp.set("HELP", false);
	bAutoSave.set("AUTO SAVE", false);
	position_Gui.set("GUI POSITION",
		glm::vec2(screenW * 0.5, screenH * 0.5),
		glm::vec2(0, 0),
		glm::vec2(screenW, screenH)
	);

	//--

	// WEBCAM

#ifdef USE_WEBCAM

	bWebcam.set("CAM ENABLE", false);
	bWebcam_Draw.set("CAM DRAW", true);
	bWebcam_Mini.set("CAM MINI", true);
	webcam_Index_Device.set("CAM DEVICE", 0, 0, 1);
	webcam_Name.set("", "");

	webcam_Index_Device.setSerializable(false);
	webcam_Name.setSerializable(false);

	params_Webcam.setName("WEBCAM" );
	params_Webcam.add(bWebcam);
	params_Webcam.add(bWebcam_Draw);
	params_Webcam.add(bWebcam_Mini);
	params_Webcam.add(webcam_Index_Device);
	params_Webcam.add(webcam_Name);

#endif

	//--

	// NDI OUT

#ifdef USE_ofxNDI_OUT

	bNDI_Output.set("OUT ENABLE", false);
	bNDI_Output_Draw.set("OUT DRAW", true);
	bNDI_Output_Mini.set("OUT MINI", true);
	NDI_Output_Name.set("OUT", "ofxNDIHelperOUT");

	//?
	// exclude from settings
	//NDI_Input_Name.setSerializable(false);
	//NDI_Output_Name.setSerializable(false);

	params_NDI_Output.setName("NDI OUTPUT" );
	params_NDI_Output.add(bNDI_Output);
	params_NDI_Output.add(bNDI_Output_Draw);
	params_NDI_Output.add(bNDI_Output_Mini);
	params_NDI_Output.add(NDI_Output_Name);

	rect_NDI_OUT.enableEdit();
	rect_NDI_OUT.setRect(200, 200, 200, 400);

#endif

	//--

	// Gui params

	params_User.setName("NDI HELPER");
	params_User.add(bHelp);
	params_User.add(bEdit);
	params_User.add(bReset);
	params_User.add(bLockRatio);
	params_User.add(bKeys);
	params_User.add(bDebug);

	//--

	// WEBCAM

#ifdef USE_WEBCAM
	params_User.add(params_Webcam);
#endif

	//--

	// Input

//#ifdef USE_ofxNDI_IN
//	params_User.add(NDI_Input1.params);
//	params_User.add(NDI_Input2.params);
//#endif

	//--

	// Output

#ifdef USE_ofxNDI_OUT
	params_User.add(params_NDI_Output);
#endif

	//--

	// Queue more into params_AppsSettings

	// Params Internal 
	// (not shown on gui)

	params_Internal.setName("INTERNAL");
	params_Internal.add(bActive);
	params_Internal.add(bAutoSave);
	params_Internal.add(position_Gui);

	//--

	// Callback

	ofAddListener(params_AppsSettings.parameterChangedE(), this, &ofxNDIHelper::Changed);
}

//--------------------------------------------------------------
void ofxNDIHelper::startup()
{
	ofLogNotice(__FUNCTION__);

	//---

	bDISABLECALLBACKS = false;

	position_Gui.set(glm::vec2(ofGetWidth() - 210, 10));

	//--

	loadSettings();

	bEdit = bEdit;
}

//--------------------------------------------------------------
void ofxNDIHelper::update(ofEventArgs& args)
{
	if (ofGetFrameNum() == 1)
	{
		startupFix();
	}

	if (!bActive)
	{
		return;
	}

	//-

#ifdef USE_WEBCAM

#ifdef FIX_WORKAROUND_STARTUP_FREEZE // Sometimes Webcam hangs on startup

	// workaround
	// restart camera after startup to make sure will be initialized fine.
	static const float _timeWait = 5.0f;
	// in x seconds at 60fps

	if (bDoRestartup && (ofGetFrameNum() == (int)(_timeWait * 60)))
	{
		//vidGrabber.close();
		doRestart_Webcam();
		bWebcam = true;
	}

#endif

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

	if (bAutoSave && ofGetElapsedTimeMillis() - timerLast_Autosave > timeToAutosave)
	{
		ofLogNotice(__FUNCTION__) << "AutoSaved Settings";

		ofLogLevel _logPre = ofGetLogLevel();
		ofSetLogLevel(OF_LOG_SILENT);
		saveSettings();
		timerLast_Autosave = ofGetElapsedTimeMillis();
		ofSetLogLevel(_logPre);
	}
}

#ifdef USE_ofxNDI_IN

// Channel 1
//--------------------------------------------------------------
void ofxNDIHelper::draw_NDI_IN_1() {
	NDI_Input1.draw_NDI_IN();
}
//--------------------------------------------------------------
void ofxNDIHelper::draw_NDI_IN_1_MiniPreview() {
	NDI_Input1.draw_NDI_IN_MiniPreview();
}
//--------------------------------------------------------------
void ofxNDIHelper::draw_NDI_IN_1_Full() {
	NDI_Input1.draw_NDI_IN_Full();
}

// Channel 2
//--------------------------------------------------------------
void ofxNDIHelper::draw_NDI_IN_2() {
	NDI_Input2.draw_NDI_IN();
}
//--------------------------------------------------------------
void ofxNDIHelper::draw_NDI_IN_2_MiniPreview() {
	NDI_Input2.draw_NDI_IN_MiniPreview();
}
//--------------------------------------------------------------
void ofxNDIHelper::draw_NDI_IN_2_Full() {
	NDI_Input2.draw_NDI_IN_Full();
}

#endif

//--------------------------------------------------------------
void ofxNDIHelper::draw() {
	if (!bGui) return;


	//-

#ifdef USE_ofxNDI_IN

	//if (bDebug)
	{
		NDI_Input1.draw();
		NDI_Input2.draw();
	}

#endif

	//-

#ifdef USE_WEBCAM

	// WEBCAM
	if (bWebcam.get() && bWebcam_Draw.get())
	{
		draw_Webcam();
	}

#endif

	//-

#ifdef USE_ofxNDI_OUT

	// NDI OUT
	if (bNDI_Output.get() && bNDI_Output_Draw.get())
	{
		draw_NDI_OUT();
	}

#endif

	//-
	 
	if (bDebug) draw_InfoDevices();

}

//--------------------------------------------------------------
void ofxNDIHelper::draw_Gui()
{
	ofDisableDepthTest();

	if (!bGui) return;

	//-

	// Gui panel
	//if (bGui_Controls)
	{
		NDI_Input1.drawGui();
		NDI_Input2.drawGui();
	}

	// Gui panel
	//if (bGui_Controls) gui_Controls.draw();
	gui_Controls.draw();

	// HelpBox
	if (bHelp) textBoxWidget.draw();

}

// keys
//--------------------------------------------------------------
void ofxNDIHelper::keyPressed(ofKeyEventArgs& eventArgs)
{
	const int& key = eventArgs.key;
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

	//--

	// key enabler

	//if (key == 'K' /*&& mod_CONTROL*/)
	//{
	//	bKeys = !bKeys;

	//	ofLogNotice(__FUNCTION__) << "KEYS: " << (bKeys ? "ON" : "OFF");

	//	if (!bKeys)
	//	{
	//		ofLogNotice(__FUNCTION__) << "ALL KEYS DISABLED. PRESS '/*CTRL + */K' TO ENABLE GAIN!";
	//	}
	//	else
	//	{
	//		ofLogNotice(__FUNCTION__) << "KEYS ENABLED BACK";
	//	}
	//}

	// disabler for all keys. (independent from bActive)
	if (!bKeys) return;

	//-

	if (0) {}

	else if (key == 'G')
	{
		bGui = !bGui;
		//bGui_Controls = !bGui_Controls;
	}

	//--

	else if (key == 'E')
	{
		bEdit = !bEdit;
	}

	else if (key == 'D')
	{
		bDebug = !bDebug;
	}

	else if (key == 'H')
	{
		bHelp = !bHelp;
	}

	//--

	// WEBCAM

#ifdef USE_WEBCAM

	else if (key == 'I')
	{
		// WEBCAM
		auto _devs = vidGrabber.listDevices();
		webcam_Index_Device++;
		if (webcam_Index_Device.get() > _devs.size() - 1) webcam_Index_Device = 0;
		webcam_Name_ = _devs[webcam_Index_Device].deviceName;
		webcam_Name = webcam_Name_;

		// select cam device
		vidGrabber.close();
		vidGrabber.setDeviceID(webcam_Index_Device.get());
		vidGrabber.setup(1920, 1080);
	}

#endif

	//--

#ifdef USE_ofxNDI_IN

	else if (key == ' ')
	{
		NDI_Input1.doScan();
		NDI_Input2.doScan();
	}

#endif

	//--
}

//--------------------------------------------------------------
void ofxNDIHelper::keyReleased(ofKeyEventArgs& eventArgs)
{
	const int& key = eventArgs.key;
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

//--------------------------------------------------------------
void ofxNDIHelper::setActive(bool b)
{
	ofLogNotice(__FUNCTION__) << b;

	bActive = b;
	if (!b)
	{
		removeKeysListeners();
	}
	else
	{
		addKeysListeners();
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::setGuiVisible(bool b)
{
	//bGui_Controls = b;
	bGui= b;
}

//--------------------------------------------------------------
void ofxNDIHelper::setGuiToggleVisible()
{
	bGui= !bGui;
	//bGui_Controls = !bGui_Controls;
}

// addon engine params
//--------------------------------------------------------------
void ofxNDIHelper::Changed(ofAbstractParameter& e)
{
	if (bDISABLECALLBACKS) return;

	{
		string name = e.getName();

		// exclude debugs

		//if (name != "exclude"
		//	&& name != "exclude")
		{
			ofLogNotice(__FUNCTION__) << name << " : " << e;
		}

		//----

		if (0) {}

		else if (name == bEdit.getName())
		{
			if (bEdit.get())
			{
#ifdef USE_WEBCAM
				rect_Webcam.enableEdit();
#endif

#ifdef USE_ofxNDI_IN
				NDI_Input1.setEdit(true);
				NDI_Input2.setEdit(true);
#endif

#ifdef USE_ofxNDI_OUT
				rect_NDI_OUT.enableEdit();
#endif
				//--

				//if (bNDI_Input.get()) 
				//{
				//	doRefresh_NDI_IN();
				//}
			}
			else
			{
#ifdef USE_WEBCAM
				rect_Webcam.disableEdit();
#endif

#ifdef USE_ofxNDI_IN
				NDI_Input1.setEdit(false);
				NDI_Input2.setEdit(false);
#endif

#ifdef USE_ofxNDI_OUT
				rect_NDI_OUT.disableEdit();
#endif
			}
		}

		else if (name == bReset.getName() && bReset)
		{
			bReset = false;

			doReset_Mini_Previews();
		}

		//----

		// WEBCAM

#ifdef USE_WEBCAM

		else if (name == bWebcam.getName())
		{
			if (bWebcam.get()) doRestart_Webcam();
			else vidGrabber.close();
		}

		else if (name == webcam_Index_Device.getName() && bWebcam.get())
		{
			setup_Webcam(webcam_Index_Device.get());
		}

#endif

		//----

		// NDI OUT

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
			gui_Controls.setPosition(position_Gui.get().x, position_Gui.get().y);
		}
		else if (name == bActive.getName())
		{
			setActive(bActive);
		}
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::setPathGlobal(string s) // must call before setup.
{
	path_GLOBAL = s;

	ofxSurfingHelpers::CheckFolder(path_GLOBAL);
}

//--

// WEBCAM

#ifdef USE_WEBCAM

// TODO:
//--------------------------------------------------------------
void ofxNDIHelper::doRestart_Webcam()
{
	// must close before reopen
	vidGrabber.close();

	// start device
	{
		vidGrabber.setDeviceID(webcam_Index_Device.get());
		//vidGrabber.setDesiredFrameRate(60);
		vidGrabber.setup(1920, 1080);
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::setup_Webcam(int index) {
	// will load from index (webcam_Index_Device) not from name 

	// get back a list of devices.
	vidGrabber.setVerbose(true);
	vector<ofVideoDevice> _devs = vidGrabber.listDevices();
	webcam_Index_Device.setMax(_devs.size() - 1);

	// get all dev names
	webcam_Names_InputDevices = "";
	for (size_t i = 0; i < _devs.size(); i++)
	{
		// queue devices names
		webcam_Names_InputDevices += "#" + ofToString(i) + " " + _devs[i].deviceName;
		if (i != _devs.size() - 1) webcam_Names_InputDevices += "\n";
	}

	//-

	// check valid index and clamp
	bool bError = false;
	if (webcam_Index_Device > _devs.size() - 1) {
		ofLogError(__FUNCTION__) << "CAMERA INDEX OUT OF RANGE";
		webcam_Index_Device = -1;
		webcam_Name = webcam_Name_ = "UNKNOWN DEVICE";
		bError = true;
	}

	if (!bError) {// valid index

		// must close before reopen
		vidGrabber.close();

		// start device
		//if (bWebcam.get()) 
		{
			vidGrabber.setDeviceID(webcam_Index_Device.get());
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

		if (webcam_Index_Device < _devs.size()) {
			webcam_Name_ = _devs[webcam_Index_Device].deviceName;
			webcam_Name = webcam_Name_;
		}
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::setup_Webcam() {
	ofLogNotice(__FUNCTION__);

	// will initialized the device from the loaded name from the xml settings, not from the index number!
	// that's because the index of a device could be changed if a new device is connected.

	// get back a list of devices.
	vidGrabber.setVerbose(true);
	//vector<ofVideoDevice> _devs = vidGrabber.listDevices();
	_devs = vidGrabber.listDevices();

	// get all dev names
	webcam_Names_InputDevices = "";
	webcam_Index_Device.setMax(_devs.size() - 1);
	ofLogNotice(__FUNCTION__) << "LIST WEBCAM DEVICES:";

	// log list devices
	for (size_t i = 0; i < _devs.size(); i++)
	{
		// queue devices names
		webcam_Names_InputDevices += "#" + ofToString(i) + " " + _devs[i].deviceName;
		if (i != _devs.size() - 1) webcam_Names_InputDevices += "\n";

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

#ifdef USE_WEBCAM
	
	// load settings file
	webcam_LoadSettings();
	
#endif//USE_WEBCAM
}

//--------------------------------------------------------------
void ofxNDIHelper::draw_Webcam_MiniPreview(bool bInfo)
{
	if (bWebcam.get())
	{
		ofPushStyle();
		ofSetColor(255, 255);

		ofRectangle r(0, 0, vidGrabber.getWidth(), vidGrabber.getHeight());

		if (bLockRatio.get()) {
			float _ratio = vidGrabber.getHeight() / vidGrabber.getWidth();
			rect_Webcam.height = rect_Webcam.width * _ratio;
		}
		vidGrabber.draw(rect_Webcam.x, rect_Webcam.y, rect_Webcam.width, rect_Webcam.height);
		rect_Webcam.draw();

		// bb border
		ofNoFill();
		ofSetColor(0);
		ofSetLineWidth(2.0);
		ofDrawRectangle(rect_Webcam);

		// Top info
		if (bDebug && bInfo)
		{
			auto p = rect_Webcam.getTopLeft() + glm::vec2(_padx, _pady);
			//int h = ofxSurfingHelpers::getHeightBBtextBoxed(font, "WEBCAM  " + webcam_Name_.get());
			ofxSurfingHelpers::drawTextBoxed(font, "WEBCAM  " + webcam_Name_.get(), p.x, p.y, 255, 0, false, 128, pad, 3, -1, true);
		}

		ofPopStyle();
	}
}

#endif

//--------------------------------------------------------------
void ofxNDIHelper::doReset_Mini_Previews()
{
#ifdef USE_WEBCAM
	float _pad = 20;
	float _xx = gui_Controls.getShape().getTopRight().x + _pad;
	float _yy = gui_Controls.getShape().getTopRight().y + 28;
	float _ratio = vidGrabber.getHeight() / vidGrabber.getWidth();
	rect_Webcam.width = wPreview;
	rect_Webcam.height = rect_Webcam.width * _ratio;
	rect_Webcam.x = _xx;
	rect_Webcam.y = _yy;
#endif

#ifdef USE_ofxNDI_OUT
	_yy += rect_Webcam.height + yPadPreview ;
	_ratio = fbo_NDI_Sender.getHeight() / fbo_NDI_Sender.getWidth();
	rect_NDI_OUT.width = wPreview;
	rect_NDI_OUT.height = rect_NDI_OUT.width * _ratio;
	rect_NDI_OUT.x = _xx;
	rect_NDI_OUT.y = _yy;
#endif

	//--

//	float _pad = 200;
//	float _xx = xPadPreview;
//	float _yy = yPadPreview;
//	float _ratio;
//
//#ifdef USE_WEBCAM
//	_yy += _pad + rect_NDI_OUT.height;
//	_ratio = vidGrabber.getHeight() / vidGrabber.getWidth();
//	rect_Webcam.width = wPreview;
//	rect_Webcam.height = rect_Webcam.width * _ratio;
//	rect_Webcam.x = _xx;
//	rect_Webcam.y = _yy;
//#endif
//
//#ifdef USE_ofxNDI_OUT
//	_xx += xPadPreview + rect_Webcam.width;
//	_yy = yPadPreview;
//	//_yy += _pad + rect_NDI_IN.height;
//	_ratio = fbo_NDI_Sender.getHeight() / fbo_NDI_Sender.getWidth();
//	rect_NDI_OUT.width = wPreview;
//	rect_NDI_OUT.height = rect_NDI_OUT.width * _ratio;
//	rect_NDI_OUT.x = _xx;
//	rect_NDI_OUT.y = _yy;
//#endif
}

#ifdef USE_WEBCAM

//--------------------------------------------------------------
void ofxNDIHelper::draw_Webcam()
{
	if (bWebcam_Mini) draw_Webcam_MiniPreview(true);
	else draw_Webcam_Full();
}

//--------------------------------------------------------------
void ofxNDIHelper::draw_Webcam_Full()
{
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

#endif

//--

#ifdef USE_ofxNDI_OUT

//--------------------------------------------------------------
void ofxNDIHelper::setup_NDI_OUT() {

	ndiSender.ReleaseSender();//?

	ofLogNotice(__FUNCTION__) << ndiSender.GetNDIversion() << " (http://ndi.tv/)";
	//strcpy_s(senderName, 256, "OF NDI Sender"); // Set the sender name

	const std::string str = NDI_Output_Name.get();
	strcpy_s(senderName, 256, str.c_str()); //Set the sender name

	// Set the dimensions of the sender output here
	// This is independent of the size of the display window
	// Sender dimensions and fps are not known yet
	//TODO:
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

#ifdef USE_ofxNDI_OUT || USE_ofxNDI_IN || USE_WEBCAM

//--------------------------------------------------------------
void ofxNDIHelper::draw_InfoDevices() {

	//--

	// WEBCAM

#ifdef USE_WEBCAM

	if (bWebcam.get() && bWebcam_Draw.get() && bWebcam_Mini.get())
	{
		// display devices

		string str;
		str = "";
		str += webcam_Name_.get();
		str += " #" + ofToString(webcam_Index_Device.get()) + "";
		str += "\n\n";
		str += "DEVICES \n\n";
		str += webcam_Names_InputDevices;
		//str += "\n\n";
		//str += "I key > Next device";
		//str += " " + ofToString(vidGrabber.isInitialized() ? "ON" : "OFF") + "\n";

		auto p = rect_Webcam.getBottomLeft() + glm::vec2(_padx2, _pady2);
		ofxSurfingHelpers::drawTextBoxed(font, str, p.x, p.y, 255, 0, false, 128, 20, 3, -1, true);
	}

#endif

	//--

	// NDI OUTPUT

#ifdef USE_ofxNDI_OUT

	if (bNDI_Output.get() && bNDI_Output_Draw.get() && bNDI_Output_Mini.get())
	{
		// Show what it's sending
		if (ndiSender.SenderCreated()) {
			string str = "";
			//str += "DEVICE\n";
			str += ofToString(senderName) + "\n";
			str += ofToString(senderWidth) + " x " + ofToString(senderHeight) + " px \n";
			str += "FPS " + ofToString(ndiSender.GetFps()) + " " + ofToString((int)ofGetFrameRate()) + "";

			x2 = rect_NDI_OUT.x + _padx2;
			y2 = rect_NDI_OUT.y + rect_NDI_OUT.getHeight() + _pady2;

			auto p = rect_NDI_OUT.getBottomLeft() + glm::vec2(_padx2, _pady2);
			ofxSurfingHelpers::drawTextBoxed(font, str, p.x, p.y, 255, 0, false, 128, 20, 3, -1, true);
		}
	}

#endif
}

//--

// NDI OUT

#ifdef USE_ofxNDI_OUT
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
		// Overlay layer
		//if (bDebug.get()) ofDrawBitmapStringHighlight("NDI OUT", 40, 60);

		fbo_NDI_Sender.end();

		//--

		// do the sending

		ofColor(255);
		ndiSender.SendImage(fbo_NDI_Sender);
		//ndiSender.SendImage(fbo_NDI_Sender, false);// send processed
		//ndiSender.SendImage(vidGrabber.getPixels());// send clean webcam
	}
}

//--

// NDI Out
//--------------------------------------------------------------
void ofxNDIHelper::draw_NDI_OUT()
{
	if (!bNDI_Output.get()) return;

	if (bNDI_Output_Mini.get()) draw_NDI_OUT_MiniPreview(true);
	else draw_NDI_OUT_Full();
}

//--------------------------------------------------------------
void ofxNDIHelper::draw_NDI_OUT_MiniPreview(bool bInfo)
{
	if (!bNDI_Output.get()) return;

	ofPushStyle();
	ofSetColor(255, 255);

	if (bLockRatio.get())
	{
		float _ratio = fbo_NDI_Sender.getHeight() / fbo_NDI_Sender.getWidth();
		rect_NDI_OUT.height = rect_NDI_OUT.width * _ratio;
	}
	fbo_NDI_Sender.draw(rect_NDI_OUT.x, rect_NDI_OUT.y, rect_NDI_OUT.width, rect_NDI_OUT.height);
	rect_NDI_OUT.draw();

	// bb border
	ofNoFill();
	ofSetColor(0);
	ofSetLineWidth(2.0);
	ofDrawRectangle(rect_NDI_OUT);

	// Top info
	if (bDebug && bInfo)
	{
		auto p = rect_NDI_OUT.getTopLeft() + glm::vec2(_padx, _pady);
		ofxSurfingHelpers::drawTextBoxed(font, "NDI OUT", p.x, p.y, 255, 0, false, 128, pad, 3, -1, true);
	}

	//fbo_NDI_Sender.draw(0, 0, ofGetWidth(), ofGetHeight());

	ofPopStyle();
}

//--------------------------------------------------------------
void ofxNDIHelper::draw_NDI_OUT_Full()
{
	if (!bNDI_Output.get()) return;

	ofPushStyle();
	ofSetColor(255, 255);

	ofRectangle r(0, 0, fbo_NDI_Sender.getWidth(), fbo_NDI_Sender.getHeight());
	r.scaleTo(ofGetWindowRect());
	//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_CENTER);
	//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_STRETCH_TO_FILL);
	//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_FILL);

	fbo_NDI_Sender.draw(r.x, r.y, r.width, r.height);

	ofPopStyle();
}

#endif

#endif

//--------------------------------------------------------------
void ofxNDIHelper::loadSettings()
{
	ofLogNotice(__FUNCTION__);

	// load file settings
	ofxSurfingHelpers::loadGroup(params_AppsSettings, path_GLOBAL + path_Params_AppSettings);

	//--

#ifdef USE_WEBCAM

	//webcamLoadSettings();

	//--

#ifdef FIX_WORKAROUND_STARTUP_FREEZE // Sometimes Webcam hangs on startup

	// fix workaround startup
	if (bWebcam.get())
	{
		bDoRestartup = true;
		bWebcam = false;
		vidGrabber.close();
	}

#endif	

	//--

#endif

	//--

	// Reset preview rectangles positions and sizes
	doReset_Mini_Previews();

	// Load Settings

#ifdef USE_ofxNDI_OUT
	rect_NDI_OUT.loadSettings(path_rect_NDI_OUT, path_GLOBAL, false);
#endif

#ifdef USE_WEBCAM
	rect_Webcam.loadSettings(path_rect_Webcam, path_GLOBAL, false);
#endif

	// fix workaround startup
	bEdit_PRE = bEdit;
}

//--------------------------------------------------------------
void ofxNDIHelper::saveSettings()
{
	ofLogNotice(__FUNCTION__);

	// Get gui position before save
	position_Gui = glm::vec2(gui_Controls.getPosition());

	ofxSurfingHelpers::CheckFolder(path_GLOBAL);

	string p = path_GLOBAL + path_Params_AppSettings;
	ofxSurfingHelpers::saveGroup(params_AppsSettings, p);

	//--

#ifdef USE_WEBCAM

	// Save settings file
	webcam_SaveSettings();

#endif // USE_WEBCAM
	
	//--

	// Viewports

#ifdef USE_WEBCAM
	rect_NDI_OUT.saveSettings(path_rect_NDI_OUT, path_GLOBAL, false);
#endif

#ifdef USE_ofxNDI_OUT
	rect_Webcam.saveSettings(path_rect_Webcam, path_GLOBAL, false);
#endif
}

//--------------------------------------------------------------
void ofxNDIHelper::exit()
{
	ofLogNotice(__FUNCTION__);

	saveSettings();

	//--

#ifdef USE_ofxNDI_OUT

	// The sender must be released 
	// or NDI sender discovery will still find it
	ndiSender.ReleaseSender();

#endif
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

	//TODO:
	//ndiSender.UpdateSender(1920, 1080);//update size

#ifdef USE_ofxNDI_IN

	NDI_Input1.windowResized(w, h);
	NDI_Input2.windowResized(w, h);

#endif
}