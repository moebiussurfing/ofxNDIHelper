#include "ofxNDIHelper.h"

//--------------------------------------------------------------
ofxNDIHelper::ofxNDIHelper()
{
	bDISABLECALLBACKS = true;

	// Path for settings
	// This is to folder all files to avoid mixing with other add-ons data
	setPathGlobal("ofxNDIHelper/");

	path_Params_AppSettings = "Settings_App.json";

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

	ofRemoveListener(params_Callbacks.parameterChangedE(), this, &ofxNDIHelper::Changed);

	ofRemoveListener(ofEvents().update, this, &ofxNDIHelper::update);

	exit();
}

//--

//--------------------------------------------------------------
void ofxNDIHelper::reset_Gui()
{
	// Positions
	position_Gui = glm::vec2(padg, padg);
	gui_Controls.setPosition(position_Gui.get().x, position_Gui.get().y);

	float w = gui_Controls.getWidth();
	glm::vec2 pg = glm::vec2(position_Gui.get());
	glm::vec2 p = glm::vec2(pg.x + w + padg, pg.y);
	gui_Webcam.setPosition(p.x, p.y);
	gui_NDI_Out.setPosition(p.x + gui_Webcam.getWidth() + padg, p.y);

#ifdef USE_ofxNDI_IN
	glm::vec2 pTr = glm::vec2(gui_NDI_Out.getShape().getTopRight());
	glm::vec2 p2 = glm::vec2(pTr.x + padg, pTr.y);
	NDI_Input1.setPositionGui(p2);

	glm::vec2 p3 = glm::vec2(p2.x + padg + w, p2.y);
	NDI_Input2.setPositionGui(p3);
#endif
}

//--------------------------------------------------------------
void ofxNDIHelper::setup_Gui()
{
	//--

	// ofxGui Theme

	ofxSurfingHelpers::setThemeDarkMini_ofxGui();

	//--

	// Setup Gui

	gui_Controls.setup("ofxNDIHelper");
	gui_Controls.add(params_Panels);
	gui_Controls.add(params_User);
	gui_Controls.add(params_Internal);

#ifdef USE_WEBCAM

	gui_Webcam.setup("WEBCAM");
	//gui_Webcam.setup("WEBCAM | " + webcam_Name.get());

	gui_Webcam.add(params_Webcam);
	//gui_Webcam.minimizeAll();

#endif

#ifdef USE_ofxNDI_OUT

	gui_NDI_Out.setup("NDI OUTPUT | " + NDI_Output_Name.get());
	gui_NDI_Out.add(params_NDI_Output);
	//gui_NDI_Out.minimizeAll();

#endif

	//--

	reset_Gui();

	//--

	// Collapse groups

	auto& gi = gui_Controls.getGroup(params_Internal.getName());
	auto& gp = gi.getGroup(position_Gui.getName());
	gi.minimize();
	gp.minimize();

	auto& gpn = gui_Controls.getGroup(params_Panels.getName());
	gpn.minimize();
}

//--------------------------------------------------------------
void ofxNDIHelper::setup()
{
	// log mode
	ofSetLogLevel("ofxNDIHelper", OF_LOG_NOTICE);

	// Gui display font

	string _str = "telegrama_render.otf";

	//string _str = "iflash-502.ttf";
	//string _str = "iflash-705.ttf";
	//_size = 6;
	//_size = 8;

	string _pathFont = "assets/fonts/" + _str;
	float _size;
	bool b;

	_size = 7;
	b = font.load(_pathFont, _size);
	if (!b) b = font.load(OF_TTF_MONO, _size);

	_size = 9;
	b = fontBig.load(_pathFont, _size);
	if (!b) b = fontBig.load(OF_TTF_MONO, _size);

	//--

	// window
	screenW = ofGetWidth();
	screenH = ofGetHeight();

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

	NDI_Input1.setPathGlobal(path_GLOBAL + "NDI_Input1/");
	NDI_Input1.setup("macOS");
	NDI_Input1.bDebug.makeReferenceTo(bDebug);

	NDI_Input2.setPathGlobal(path_GLOBAL + "NDI_Input2/");
	NDI_Input2.setup("RPi");
	NDI_Input2.bDebug.makeReferenceTo(bDebug);

#endif

	//--

	// Outputs

#ifdef USE_ofxNDI_OUT

	setup_NDI_OUT();

#endif

#ifdef USE_ofxNDI_OUT

	//rect_NDI_OUT.loadSettings(path_rect_NDI_OUT, path_GLOBAL, false);
	rect_NDI_OUT.setMode(ofxSurfingBoxInteractive::FREE_LAYOUT);
	rect_NDI_OUT.setName(path_rect_NDI_OUT);
	rect_NDI_OUT.setPathGlobal(path_GLOBAL);
	rect_NDI_OUT.setup();

#endif

#ifdef USE_WEBCAM

	//rect_Webcam.loadSettings(path_rect_Webcam, path_GLOBAL, false);
	rect_Webcam.setMode(ofxSurfingBoxInteractive::FREE_LAYOUT);
	rect_Webcam.setName(path_rect_Webcam);
	rect_Webcam.setPathGlobal(path_GLOBAL);
	rect_Webcam.setup();

#endif

	//--

	// Group all
	// this group will be saved as settings to a file on /data

	//--

	// Settings File

	params_AppsSettings.setName("NDI Helper");
	params_AppsSettings.add(params_Internal);

	//params_AppsSettings.add(params_User); //-> crashes. why??
#ifdef USE_WEBCAM
	params_AppsSettings.add(params_Webcam);
#endif
#ifdef USE_ofxNDI_OUT
	params_AppsSettings.add(params_NDI_Output);
#endif

	//--

	setup_Gui();

	//--

	// Startup

	startup();
}

//--

//--------------------------------------------------------------
void ofxNDIHelper::setup_Params()
{
	bGui.set("NDI HELPER", true);
	bGui_Controls.set("NDI CONTROLS", true);
	bGui_Webcam.set("WEBCAM", true);
	bGui_NDI_OUT.set("NDI OUT", true);

	//bEdit.set("LAYOUT EDIT", true);
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
		glm::vec2(padg, padg),
		glm::vec2(0, 0),
		glm::vec2(screenW, screenH)
	);

	//--

	// WEBCAM

#ifdef USE_WEBCAM

	bWebcam_Enable.set("CAM ENABLE", false);
	bWebcam_Draw.set("CAM DRAW", true);
	bWebcam_Mini.set("CAM MINI", true);
	webcam_Index_Device.set("CAM DEVICE", 0, 0, 1);
	webcam_Name.set("", "");

	webcam_Index_Device.setSerializable(false);
	webcam_Name.setSerializable(false);

	params_Webcam.setName("WEBCAM");
	params_Webcam.add(bWebcam_Enable);
	params_Webcam.add(bWebcam_Draw);
	params_Webcam.add(bWebcam_Mini);
	params_Webcam.add(webcam_Index_Device);
	params_Webcam.add(webcam_Name);

#endif

	//--

	// NDI OUT

#ifdef USE_ofxNDI_OUT

	bNDI_Output_Enable.set("OUT ENABLE", false);
	bNDI_Output_Draw.set("OUT DRAW", true);
	bNDI_Output_Mini.set("OUT MINI", true);
	NDI_Output_Name.set("OUT", "oF OUT");

	params_NDI_Output.setName("NDI OUTPUT");
	params_NDI_Output.add(bNDI_Output_Enable);
	params_NDI_Output.add(bNDI_Output_Draw);
	params_NDI_Output.add(bNDI_Output_Mini);
	params_NDI_Output.add(NDI_Output_Name);

#endif

	//--

	// Gui Params

	params_User.setName("NDI HELPER");
	params_User.add(bHelp);
	params_User.add(bLockRatio);
	params_User.add(bKeys);
	params_User.add(bDebug);//TODO: BUG: not linking when makeReference from parent scope..
	params_User.add(bReset);

	//--

	// Params Internal 

	params_Panels.setName("PANELS");
	//params_Panels.add(bGui);
	params_Panels.add(bGui_Controls);
	params_Panels.add(bGui_Webcam);
	params_Panels.add(bGui_NDI_OUT);

#ifdef USE_ofxNDI_IN
	params_Panels.add(NDI_Input1.bGui_Internal);
	params_Panels.add(NDI_Input2.bGui_Internal);
#endif

	//--

	params_Internal.setName("INTERNAL");


	params_Internal.add(bActive);
	params_Internal.add(bAutoSave);
	params_Internal.add(position_Gui);
	//params_Internal.add(bDebug);

	//--

	// Callbacks

	params_Callbacks.setName("Callbacks");
	params_Callbacks.add(bReset);
	params_Callbacks.add(bWebcam_Enable);
	params_Callbacks.add(webcam_Index_Device);
	params_Callbacks.add(bNDI_Output_Enable);
	params_Callbacks.add(NDI_Output_Name);
	params_Callbacks.add(position_Gui);
	params_Callbacks.add(bActive);

	ofAddListener(params_Callbacks.parameterChangedE(), this, &ofxNDIHelper::Changed);
}

//--------------------------------------------------------------
void ofxNDIHelper::startup()
{
	ofLogNotice(__FUNCTION__);

	//---

	bDISABLECALLBACKS = false;

	//position_Gui.set(glm::vec2(ofGetWidth() - 210, 10));

	//--

	loadSettings();

	//bEdit = bEdit;
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
		bWebcam_Enable = true;
	}

#endif

	//-

	if (bWebcam_Enable.get())
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

	NDI_Input1.draw();
	NDI_Input2.draw();

#endif

	//-

#ifdef USE_WEBCAM

	// WEBCAM
	if (bWebcam_Enable.get() && bWebcam_Draw.get())
	{
		draw_Webcam();
	}

#endif

	//-

#ifdef USE_ofxNDI_OUT

	// NDI OUT
	if (bNDI_Output_Enable.get() && bNDI_Output_Draw.get())
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

	// Gui Panel
	if (bGui_Controls)
	{
		NDI_Input1.drawGui();
		NDI_Input2.drawGui();

		// Gui panel
		gui_Controls.draw();

		if (bGui_Webcam) gui_Webcam.draw();
		if (bGui_NDI_OUT) gui_NDI_Out.draw();
	}

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
	}
	else if (key == 'g')
	{
		bGui_Controls = !bGui_Controls;
	}

	//--

	//else if (key == 'E')
	//{
	//	bEdit = !bEdit;
	//}

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
	bGui = b;
}

//--------------------------------------------------------------
void ofxNDIHelper::setGuiToggleVisible()
{
	bGui = !bGui;
}

// addon engine params
//--------------------------------------------------------------
void ofxNDIHelper::Changed(ofAbstractParameter& e)
{
	if (bDISABLECALLBACKS) return;

	string name = e.getName();

	// exclude debugs
	//if (name != "exclude"
	//	&& name != "exclude")
	{
		ofLogNotice(__FUNCTION__) << name << " : " << e;
	}

	//----

	if (0) {}

	else if (name == bReset.getName() && bReset)
	{
		bReset = false;

		doReset_Mini_Previews();
	}

	//----

	// WEBCAM

#ifdef USE_WEBCAM

	else if (name == bWebcam_Enable.getName())
	{
		if (bWebcam_Enable.get()) doRestart_Webcam();
		else vidGrabber.close();
	}

	else if (name == webcam_Index_Device.getName() && bWebcam_Enable.get())
	{
		setup_Webcam(webcam_Index_Device.get());
	}

#endif

	//----

	// NDI OUT

#ifdef USE_ofxNDI_OUT 

	else if (name == bNDI_Output_Enable.getName())
		//else if (name == bNDI_Output_Enable.getName() && bNDI_Output_Enable.get())
	{
		if (bNDI_Output_Enable.get())
		{
			//setup_NDI_OUT();
		}
		else
		{
			//ndiSender.ReleaseSender();//?
		}
	}
	else if (name == NDI_Output_Name.getName())
	{
		setup_NDI_OUT();
	}

#endif

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
		webcam_Names_InputDevices += ofToString(i) + " " + _devs[i].deviceName;
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
		//if (bWebcam_Enable.get()) 
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
			bWebcam_Enable = false;
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
		webcam_Names_InputDevices += ofToString(i) + " " + _devs[i].deviceName;
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

#endif
}

//--------------------------------------------------------------
void ofxNDIHelper::draw_Webcam_MiniPreview(bool bInfo)
{
	if (bWebcam_Enable.get())
	{
		ofPushStyle();
		ofSetColor(255, 255);

		ofRectangle r(0, 0, vidGrabber.getWidth(), vidGrabber.getHeight());

		if (bLockRatio.get()) {
			float _ratio = vidGrabber.getHeight() / vidGrabber.getWidth();
			rect_Webcam.setHeight(rect_Webcam.getWidth() * _ratio);
		}
		vidGrabber.draw(rect_Webcam.getX(), rect_Webcam.getY(), rect_Webcam.getWidth(), rect_Webcam.getHeight());
		rect_Webcam.draw();

		// bb border
		ofNoFill();
		ofSetColor(0);
		ofSetLineWidth(2.0);
		ofDrawRectangle(rect_Webcam.getRectangle());

		//// Top info
		//if (bDebug && bInfo)
		//{
		//	glm::vec2 p = rect_Webcam.getRectangle().getTopLeft() + glm::vec2(_padx, _pady);
		//	//int h = ofxSurfingHelpers::getHeightBBtextBoxed(font, "WEBCAM  " + webcam_Name_.get());
		//	ofxSurfingHelpers::drawTextBoxed(font, "WEBCAM | " + webcam_Name_.get(), p.x, p.y, 255, 0, false, 128, pad, 3, -1, true);
		//}

		ofPopStyle();
	}
}
//--------------------------------------------------------------
void ofxNDIHelper::webcam_SaveSettings()
{
	ofLogNotice(__FUNCTION__);

	ofXml _xml;
	ofSerialize(_xml, webcam_Name_);
	_xml.save(path_GLOBAL + path_WebcamSettings);
}
//--------------------------------------------------------------
void ofxNDIHelper::webcam_LoadSettings()
{
	ofLogNotice(__FUNCTION__);

	//TODO: use this file for settings.
	//TODO: camera is loading from index on app settings...

	ofXml _xml;
	bool _isLoaded;
	_isLoaded = _xml.load(path_GLOBAL + path_WebcamSettings);
	ofDeserialize(_xml, webcam_Name_);
	ofLogNotice(__FUNCTION__) << _xml.toString();
	ofLogNotice(__FUNCTION__) << "xml device name:\t" << webcam_Name_.get();


	//--

	// start device
	//bDISABLECALLBACKS = true;

	webcam_Index_Device = -1;
	if (_isLoaded) {
		for (int i = 0; i < _devs.size(); i++) {
			if (_devs[i].deviceName == webcam_Name_.get()) {
				webcam_Index_Device = i;
				ofLogNotice(__FUNCTION__) << "device name: \t" << webcam_Name_.get();
				ofLogNotice(__FUNCTION__) << "device index: \t" << webcam_Index_Device;
			}
		}
	}
	if (webcam_Index_Device == -1) {// error. try to load first device...
		webcam_Index_Device = 0;// force select first cam device

		if (webcam_Index_Device < _devs.size()) {
			webcam_Name_ = _devs[webcam_Index_Device].deviceName;
			webcam_Name = _devs[webcam_Index_Device].deviceName;
		}
		else
		{
			ofLogError(__FUNCTION__) << "CAMERA INDEX OUT OF RANGE";
			webcam_Name = webcam_Name_ = "UNKNOWN DEVICE";
			vidGrabber.close();
			return;//cancel and exit!
		}
	}

	webcam_Name = webcam_Name_;

	//--

	// 1. Must close before reopen
	vidGrabber.close();

	// 2. Start device
	//if (bWebcam_Enable.get()) 
	{
		vidGrabber.setDeviceID(webcam_Index_Device.get());
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
	//	bWebcam_Enable = false;
	//}
}

#endif

//--------------------------------------------------------------
void ofxNDIHelper::doReset_Mini_Previews()
{
#ifdef USE_WEBCAM

	//float _ratio = vidGrabber.getHeight() / vidGrabber.getWidth();
	//float _pad = 20;
	//float _xx = gui_Controls.getShape().getTopRight().x + _pad;
	//float _yy = gui_Controls.getShape().getTopRight().y + 28;

	////rect_Webcam.width = wPreview;
	////rect_Webcam.height = rect_Webcam.width * _ratio;
	////rect_Webcam.x = _xx;
	////rect_Webcam.y = _yy;

	////glm::vec2 p = glm::vec2(gui_Control.getShape().getTopRight());
	////float _xx = p.x + _pad;
	////float _yy = p.y + 28;

	//rect_Webcam.setShape(ofRectangle(_xx, _yy, wPreview, wPreview * _ratio));
	rect_Webcam.reset();

#endif

#ifdef USE_ofxNDI_OUT

	//_ratio = fbo_NDI_Sender.getHeight() / fbo_NDI_Sender.getWidth();
	//_yy += rect_Webcam.getRectangle().getBottomLeft().y + yPadPreview;
	////rect_NDI_OUT.width = wPreview;
	////rect_NDI_OUT.height = rect_NDI_OUT.width * _ratio;
	////rect_NDI_OUT.x = _xx;
	////rect_NDI_OUT.y = _yy;

	//rect_NDI_OUT.setShape(ofRectangle(_xx, _yy, wPreview, wPreview * _ratio));
	rect_NDI_OUT.reset();

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

	// Fbo settings
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

	if (bWebcam_Enable.get() && bWebcam_Draw.get() && bWebcam_Mini.get())
	{
		// Top info
		if (bDebug)
		{
			glm::vec2 p = rect_Webcam.getRectangle().getTopLeft() + glm::vec2(_padx, _pady);
			//int h = ofxSurfingHelpers::getHeightBBtextBoxed(font, "WEBCAM  " + webcam_Name_.get());
			ofxSurfingHelpers::drawTextBoxed(font, "WEBCAM | " + webcam_Name_.get(), p.x, p.y, 255, 0, false, 128, pad, 3, -1, true);
		}

		//--

		// display devices

		string str;
		str = "";
		str += webcam_Name_.get();
		str += " " + ofToString(webcam_Index_Device.get());
		str += "\n\n";
		//str += "DEVICES \n\n";
		str += webcam_Names_InputDevices;
		//str += "\n\n";
		//str += "I key > Next device";
		//str += " " + ofToString(vidGrabber.isInitialized() ? "ON" : "OFF") + "\n";

		glm::vec2 p = rect_Webcam.getRectangle().getBottomLeft() + glm::vec2(_padx2, _pady2);
		ofxSurfingHelpers::drawTextBoxed(font, str, p.x, p.y, 255, 0, false, 128, 20, 3, -1, true);
	}

#endif

	//--

	// NDI OUTPUT

#ifdef USE_ofxNDI_OUT

	if (bNDI_Output_Enable.get() && bNDI_Output_Draw.get() && bNDI_Output_Mini.get())
	{
		// Show what it's sending
		if (ndiSender.SenderCreated())
		{
			// Top info
			if (bDebug)
			{
				glm::vec2 p = rect_NDI_OUT.getRectangle().getTopLeft() + glm::vec2(_padx, _pady);
				string s = "NDI OUT | " + NDI_Output_Name.get();
				ofxSurfingHelpers::drawTextBoxed(font, s, p.x, p.y, 255, 0, false, 128, pad, 3, -1, true);
			}

			//--

			string str = "";
			//str += "DEVICE\n";
			str += ofToString(senderName) + "\n";
			str += ofToString(senderWidth) + "x" + ofToString(senderHeight) + "px | ";
			str += "FPS " + ofToString(ndiSender.GetFps()) + " / " + ofToString((int)ofGetFrameRate()) + "";

			x2 = rect_NDI_OUT.getX() + _padx2;
			y2 = rect_NDI_OUT.getY() + rect_NDI_OUT.getHeight() + _pady2;

			glm::vec2 p = rect_NDI_OUT.getRectangle().getBottomLeft() + glm::vec2(_padx2, _pady2);
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
	if (bNDI_Output_Enable.get())
	{
		fbo_NDI_Sender.begin();
		ofClear(0, 0, 0, 255);
	}
}
//--------------------------------------------------------------
void ofxNDIHelper::end_NDI_OUT()
{
	if (bNDI_Output_Enable.get())
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
	if (!bNDI_Output_Enable.get()) return;

	if (bNDI_Output_Mini.get()) draw_NDI_OUT_MiniPreview(true);
	else draw_NDI_OUT_Full();
}

//--------------------------------------------------------------
void ofxNDIHelper::draw_NDI_OUT_MiniPreview(bool bInfo)
{
	if (!bNDI_Output_Enable.get()) return;

	ofPushStyle();
	ofSetColor(255, 255);

	if (bLockRatio.get())
	{
		float _ratio = fbo_NDI_Sender.getHeight() / fbo_NDI_Sender.getWidth();
		rect_NDI_OUT.setHeight(rect_NDI_OUT.getWidth() * _ratio);
	}
	fbo_NDI_Sender.draw(rect_NDI_OUT.getX(), rect_NDI_OUT.getY(), rect_NDI_OUT.getWidth(), rect_NDI_OUT.getHeight());
	rect_NDI_OUT.draw();

	// bb border
	ofNoFill();
	ofSetColor(0);
	ofSetLineWidth(2.0);
	ofDrawRectangle(rect_NDI_OUT.getRectangle());

	//// Top info
	//if (bDebug && bInfo)
	//{
	//	glm::vec2 p = rect_NDI_OUT.getRectangle().getTopLeft() + glm::vec2(_padx, _pady);
	//	string s = "NDI OUT | " + NDI_Output_Name.get();
	//	ofxSurfingHelpers::drawTextBoxed(font, s, p.x, p.y, 255, 0, false, 128, pad, 3, -1, true);
	//}

	//fbo_NDI_Sender.draw(0, 0, ofGetWidth(), ofGetHeight());

	ofPopStyle();
}

//--------------------------------------------------------------
void ofxNDIHelper::draw_NDI_OUT_Full()
{
	if (!bNDI_Output_Enable.get()) return;

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
	if (bWebcam_Enable.get())
	{
		bDoRestartup = true;
		bWebcam_Enable = false;
		vidGrabber.close();
	}

#endif	

	//--

#endif

	//--

	// Reset preview rectangles positions and sizes
	//doReset_Mini_Previews();

	// Load Settings

//#ifdef USE_ofxNDI_OUT
//	rect_NDI_OUT.loadSettings(path_rect_NDI_OUT, path_GLOBAL, false);
//#endif
//
//#ifdef USE_WEBCAM
//	rect_Webcam.loadSettings(path_rect_Webcam, path_GLOBAL, false);
//#endif

	// fix workaround startup
	//bEdit_PRE = bEdit;
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

	//ofxSurfingHelpers::saveGroup()

	//--

#ifdef USE_WEBCAM

	// Save settings file
	webcam_SaveSettings();

#endif

	//--

	// Viewports

//#ifdef USE_WEBCAM
//	rect_NDI_OUT.saveSettings(path_rect_NDI_OUT, path_GLOBAL, false);
//#endif
//
//#ifdef USE_ofxNDI_OUT
//	rect_Webcam.saveSettings(path_rect_Webcam, path_GLOBAL, false);
//#endif
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