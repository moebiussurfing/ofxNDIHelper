#include "ofxNDIHelper.h"

//--------------------------------------------------------------
ofxNDIHelper::ofxNDIHelper() {
	bDISABLE_CALLBACKS = true;

	// Path for settings
	// This is to folder all files to avoid mixing with other add-ons data
	setPathGlobal("ofxNDIHelper/");

	path_Params_AppSettings = "Settings_App.json";

#ifdef USE_WEBCAM
	name_WebCamSettings = "Settings_WebCam.xml";
	path_WebCamSettings = "WebCam/";
#endif

	//--

	// Help box
	textBoxWidget.setPath(path_GLOBAL + "HelpBox/");

	// text
	std::string helpInfo = "";
	helpInfo += "HELP \n";
	helpInfo += "NDI MANAGER \n";
	textBoxWidget.setTitle(helpInfo);

	helpInfo = "";
	helpInfo += "\n\n\n\n\n"; //fix some spacing..
	helpInfo += "Preview Boxes\n";
	helpInfo += "DoubleClick to Edit/Lock\n";
	helpInfo += "MouseWheel over to Scale size\n";
	helpInfo += "\n";
	helpInfo += "KEYS \n";
	helpInfo += "H             Help\n";
	helpInfo += "I             WebCam Next\n";
	helpInfo += "Alt+MOUSE     WebCam Transform\n";
	helpInfo += "SPACE         NDI Inputs Scan\n";
	helpInfo += "R             Layout Reset\n";
	helpInfo += "D             Debug\n";

	textBoxWidget.setup();
	textBoxWidget.setText(helpInfo);

	bHelp.makeReferenceTo(textBoxWidget.bGui);

	//--

	setActive(true); // add key and mouse listeners

#ifdef USE_OFX_CHILD_FRAME
	ofAddListener(ofEvents().update, this, &ofxNDIHelper::update);
	ofAddListener(ofEvents().mouseScrolled, this, &ofxNDIHelper::mouseScrolled);
	ofAddListener(ofEvents().mouseDragged, this, &ofxNDIHelper::mouseDragged);
#endif
}

//--------------------------------------------------------------
ofxNDIHelper::~ofxNDIHelper() {
	setActive(false); // remove keys and mouse listeners

	// remove params callbacks listeners

	ofRemoveListener(params_Callbacks.parameterChangedE(), this, &ofxNDIHelper::Changed);

#ifdef USE_WEBCAM
	ofRemoveListener(params_WebCam.parameterChangedE(), this, &ofxNDIHelper::Changed_WebCam);
#endif

#ifdef USE_ofxNDI_OUT
	ofRemoveListener(params_NDI_Output.parameterChangedE(), this, &ofxNDIHelper::Changed_NDI_Out);
#endif

#ifdef USE_OFX_CHILD_FRAME
	ofRemoveListener(ofEvents().update, this, &ofxNDIHelper::update);
	ofRemoveListener(ofEvents().mouseScrolled, this, &ofxNDIHelper::mouseScrolled);
	ofRemoveListener(ofEvents().mouseDragged, this, &ofxNDIHelper::mouseDragged);
#endif

	exit();
}

//--------------------------------------------------------------
void ofxNDIHelper::setup() {
	// log mode
	ofSetLogLevel("ofxNDIHelper", OF_LOG_NOTICE);

	//--

	// ofxGui Theme

	ofxSurfingHelpers::setThemeDarkMini_ofxGui();

	//--

	// Gui display font

	string _str = "JetBrainsMonoNL-ExtraBold.ttf";
	//string _str = "telegrama_render.otf";
	//string _str = "AnonymousPro-Regular.ttf";
	//string _str = "JetBrainsMono-ExtraBold.ttf";
	//string _str = "Inconsolata_Condensed-Light.ttf";

	//string _str = "iflash-502.ttf";
	//string _str = "iflash-705.ttf";
	//_size = 6;
	//_size = 8;

	string _pathFont = "assets/fonts/" + _str;
	float _size;
	bool b;

	_size = 8;
	//_size = 7;
	b = font.load(_pathFont, _size);
	if (!b) b = font.load(OF_TTF_MONO, _size);

	//_size = 13;
	////_size = 9;
	//b = fontBig.load(_pathFont, _size);
	//if (!b) b = fontBig.load(OF_TTF_MONO, _size);

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
	setup_WebCam();
#endif

	//--

#ifdef USE_ofxNDI_IN
	NDI_Input1.setPathGlobal(path_GLOBAL + "NDI_Input1/");
	NDI_Input1.setup("1");
	NDI_Input1.bDebug.makeReferenceTo(bDebug);

	NDI_Input2.setPathGlobal(path_GLOBAL + "NDI_Input2/");
	NDI_Input2.setup("2");
	NDI_Input2.bDebug.makeReferenceTo(bDebug);
#endif

	//--

#ifdef USE_ofxNDI_OUT
	setup_NDI_OUT();

	//rect_NDI_OUT.loadSettings(path_rect_NDI_OUT, path_GLOBAL, false);
	rect_NDI_OUT.setModeLayout(ofxSurfingBox::FREE_LAYOUT);
	rect_NDI_OUT.setName(name_rect_NDI_OUT);
	rect_NDI_OUT.setPathGlobal(path_GLOBAL + path_rect_NDI_OUT);
	rect_NDI_OUT.setup();
	rect_NDI_OUT.setUseBorder(true);
	rect_NDI_OUT.setBorderColor(ofColor(0, 200));
#endif

	//--

	// Group all
	// this group will be saved as settings to a file on /data

	//--

	// Settings File

	params_AppsSettings.setName("NDI Helper");
	params_AppsSettings.add(params_Internal);

#ifdef USE_WEBCAM
	params_AppsSettings.add(params_WebCam);
#endif

#ifdef USE_ofxNDI_OUT
	params_AppsSettings.add(params_NDI_Output);
#endif

#ifdef USE_ofxNDI_OUT || USE_ofxNDI_IN || USE_WEBCAM
	params_AppsSettings.add(params_Panels);
#endif

	//params_AppsSettings.add(bLockAspect);
	//params_AppsSettings.add(params_Helper); //-> crashes. why??

	//--

	setup_GuiInternal();

	//--

	// Startup

	startup();
}

//--

//--------------------------------------------------------------
void ofxNDIHelper::setup_GuiInternal() {
	//--

	// Setup Gui

	gui_Controls.setup("ofxNDIHelper");

#ifdef USE_ofxNDI_OUT || USE_ofxNDI_IN || USE_WEBCAM
	gui_Controls.add(params_Panels);
#endif

	gui_Controls.add(params_Helper);
	gui_Controls.add(params_Internal);

#ifdef USE_WEBCAM
	gui_WebCam.setup("WEBCAM");
	//gui_WebCam.setup("WEBCAM | " + webcam_Name.get());

	gui_WebCam.add(params_WebCam);
	//gui_WebCam.minimizeAll();
#endif

#ifdef USE_ofxNDI_OUT
	gui_NDI_Out.setup("OUT | " + NDI_Output_Name.get());
	gui_NDI_Out.add(params_NDI_Output);
	//gui_NDI_Out.minimizeAll();
#endif

	//--

	doReset_Gui();

	//--

	// Collapse groups

	auto & gi = gui_Controls.getGroup(params_Internal.getName());
	auto & gp = gi.getGroup(position_Gui.getName());
	gi.minimize();
	gp.minimize();

	auto & gh = gui_Controls.getGroup(params_Helper.getName());
	gh.minimize();

#ifdef USE_ofxNDI_OUT || USE_ofxNDI_IN || USE_WEBCAM
	auto & gpn = gui_Controls.getGroup(params_Panels.getName());
	gpn.minimize();
#endif

#ifdef USE_WEBCAM
	auto & gw = gui_WebCam.getGroup(params_WebCam.getName());
	gw.minimize();
#endif

#ifdef USE_ofxNDI_OUT
	auto & go = gui_NDI_Out.getGroup(params_NDI_Output.getName());
	go.minimize();
#endif
}

//--

#ifdef USE_WEBCAM

//--------------------------------------------------------------
void ofxNDIHelper::webcam_Next() {
	auto _devs = webcam_Grabber.listDevices();
	webcam_Index_Device++;
	if (webcam_Index_Device.get() > _devs.size() - 1) webcam_Index_Device = 0;
	webcam_Name_ = _devs[webcam_Index_Device].deviceName;
	webcam_Name = webcam_Name_;

	// select cam device
	webcam_Grabber.close();
	webcam_Grabber.setDeviceID(webcam_Index_Device.get());
	webcam_Grabber.setup(1920, 1080);
}

//--------------------------------------------------------------
void ofxNDIHelper::setup_WebCam_Params() {
	bWebCam_Restart.set("Restart", false);
	bWebCam_Next.set("Next", false);
	bWebCam_Enable.set("Enable", false);
	bWebCam_LockRatio.set("Lock Aspect", true);
	bWebCam_Draw.set("Draw", true);
	bWebCam_DrawMini.set("Mini", true);
	webcam_Index_Device.set("Device", 0, 0, 1);
	scaleMode_Index.set("Scale", 0, 0, 3);
	scaleMode_Name.set("Name", "");
	webcam_Name.set("", "");

	webcam_Index_Device.setSerializable(false);
	webcam_Name.setSerializable(false);
	scaleMode_Name.setSerializable(false);
	bWebCam_Restart.setSerializable(false);
	bWebCam_Next.setSerializable(false);

	params_WebCam.setName("WEBCAM");
	params_WebCam.add(bWebCam_Enable);
	params_WebCam.add(bWebCam_Draw);
	params_WebCam.add(bWebCam_DrawMini);
	params_WebCam.add(bWebCam_Next);
	params_WebCam.add(webcam_Index_Device);
	params_WebCam.add(webcam_Name);
	params_WebCam.add(scaleMode_Index);
	params_WebCam.add(scaleMode_Name);
	params_WebCam.add(bWebCam_LockRatio);
	params_WebCam.add(rect_WebCam.bEdit);
	params_WebCam.add(bWebCam_Restart);

	scaleMode_Names.clear();
	scaleMode_Names.push_back("FIT");
	scaleMode_Names.push_back("FILL");
	scaleMode_Names.push_back("CENTER");
	scaleMode_Names.push_back("STRETCH_TO_FILL");
}

#endif

//--------------------------------------------------------------
void ofxNDIHelper::setup_Params() {
	bGui_Internal.set("ofxGui", true);
	bGui.set("NDI HELPER", true);
	//bGui_Controls.set("NDI CONTROLS", true);

#ifdef USE_WEBCAM
	bGui_WebCam.set("WEBCAM", true);
#endif

	bGui_NDI_OUT.set("OUT", true);

	//bEdit.set("LAYOUT EDIT", true);
	//bLockAspect.set("LOCK ASPECT", true);
	bResetLayout.set("Reset Layout", false);
	bResetGui.set("Reset UI", false);
	bKeys.set("Keys", true);
	bDebug.set("Debug", true);

	bResetLayout.setSerializable(false);
	bResetGui.setSerializable(false);
	bActive.setSerializable(false);

	// Internal
	bActive.set("Active", true);
	bHelp.set("Help", false);
	bAutoSave.set("Auto Save", false);
	position_Gui.set("UI Position",
		glm::vec2(padg, padg),
		glm::vec2(0, 0),
		glm::vec2(screenW, screenH));

	//--

	// WEBCAM

#ifdef USE_WEBCAM
	setup_WebCam_Params();
#endif

	//--

	// NDI OUT

#ifdef USE_ofxNDI_OUT
	bNDI_Output_Enable.set("Enable", false);
	bNDI_Output_Draw.set("Draw", true);
	bNDI_Output_Mini.set("Mini", true);
	NDI_Output_Name.set("Name", "oF");

	params_NDI_Output.setName("NDI Output");
	params_NDI_Output.add(NDI_Output_Name);
	params_NDI_Output.add(bNDI_Output_Enable);
	params_NDI_Output.add(bNDI_Output_Draw);
	params_NDI_Output.add(bNDI_Output_Mini);
	params_NDI_Output.add(rect_NDI_OUT.bEdit);
#endif

	//--

	// Gui Params

	params_Helper.setName("NDI Helper");
	params_Helper.add(bResetLayout);
	params_Helper.add(bResetGui);
	//TODO: BUG: not linking when makeReference from parent scope..
	//params_Helper.add(bLockAspect);

	params_Helper.add(bDebug);
	params_Helper.add(bKeys);
	params_Helper.add(bHelp);

	//--

	// Params Internal

	params_Panels.setName("PANELS");

#ifdef USE_WEBCAM
	params_Panels.add(bGui_WebCam);
#endif

#ifdef USE_ofxNDI_IN
	bGui_NDI_IN1.makeReferenceTo(NDI_Input1.bGui_Internal);
	bGui_NDI_IN2.makeReferenceTo(NDI_Input2.bGui_Internal);
	params_Panels.add(bGui_NDI_IN1);
	params_Panels.add(bGui_NDI_IN2);
#endif

#ifdef USE_ofxNDI_OUT
	params_Panels.add(bGui_NDI_OUT);
#endif

	//--

	params_Internal.setName("INTERNAL");

	params_Internal.add(bGui_Internal);
	params_Internal.add(bActive);
	params_Internal.add(bAutoSave);
	params_Internal.add(position_Gui);

	//--

	// Callbacks

	params_Callbacks.setName("Callbacks");

	params_Callbacks.add(bResetLayout);
	params_Callbacks.add(bResetGui);
	params_Callbacks.add(position_Gui);
	params_Callbacks.add(bActive);

	ofAddListener(params_Callbacks.parameterChangedE(), this, &ofxNDIHelper::Changed);

#ifdef USE_WEBCAM
	ofAddListener(params_WebCam.parameterChangedE(), this, &ofxNDIHelper::Changed_WebCam);
#endif

#ifdef USE_ofxNDI_OUT
	ofAddListener(params_NDI_Output.parameterChangedE(), this, &ofxNDIHelper::Changed_NDI_Out);
#endif
}

//--------------------------------------------------------------
void ofxNDIHelper::startup() {
	ofLogNotice(__FUNCTION__);

	//---

	bDISABLE_CALLBACKS = false;

	//--

#ifdef USE_WEBCAM
	// load settings file
	webcam_LoadSettings();
#endif

	//--

// set to 1 to reset and ignore settings.
// useful when something is broke, like gui not visible etc.
#define RESET_SETTINGS 0
#if (RESET_SETTINGS)
	bGui_Internal = true;
	bGui_NDI_IN1 = true;
	bGui_NDI_IN2 = true;
	bAutoSave = true;
	bActive = true;
	position_Gui = { 10, 10 };
	bResetGui = true;
	bResetLayout = true;
#else
	loadSettings();
#endif
}

//--------------------------------------------------------------
void ofxNDIHelper::update(ofEventArgs & args) {
	if (!bActive) return;

	// Startup waiting
	if (!bLoadedStartupDone) {

#ifdef USE_ofxNDI_IN
		if (NDI_Input1.bLoadedStartupDone && NDI_Input2.bLoadedStartupDone) {
			ofLogNotice(__FUNCTION__) << "LOADED STARTUP Done";

			bLoadedStartupDone = true;
		}
#endif
	}

	//--

#ifdef USE_WEBCAM
	#ifdef FIX_WORKAROUND_STARTUP_FREEZE // Sometimes WebCam hangs on startup
	// workaround
	// restart camera after startup to make sure will be initialized fine.
	static const float _timeWait = 5.0f;
	// in x seconds at 60fps

	if (bDoRestartup && (ofGetFrameNum() == (int)(_timeWait * 60))) {
		//webcam_Grabber.close();
		doRestart_WebCam();
		bWebCam_Enable = true;
	}
	#endif

	//--

	if (bWebCam_Enable.get()) {
		webcam_Grabber.update();
	}
#endif

	//--

	// Auto save

	if (bAutoSave && ofGetElapsedTimeMillis() - timerLast_Autosave > timeToAutosave) {
		ofLogNotice(__FUNCTION__) << "Auto Saved Settings";

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
	NDI_Input1.draw_Main();
}
//--------------------------------------------------------------
void ofxNDIHelper::draw_NDI_IN_1_MiniPreview() {
	NDI_Input1.draw_MiniPreview();
}
//--------------------------------------------------------------
void ofxNDIHelper::draw_NDI_IN_1_Full() {
	NDI_Input1.draw_FullScreen();
}

// Channel 2
//--------------------------------------------------------------
void ofxNDIHelper::draw_NDI_IN_2() {
	NDI_Input2.draw_Main();
}
//--------------------------------------------------------------
void ofxNDIHelper::draw_NDI_IN_2_MiniPreview() {
	NDI_Input2.draw_MiniPreview();
}
//--------------------------------------------------------------
void ofxNDIHelper::draw_NDI_IN_2_Full() {
	NDI_Input2.draw_FullScreen();
}

#endif

//--------------------------------------------------------------
void ofxNDIHelper::drawPreviews() {

#ifdef USE_ofxNDI_IN
	NDI_Input1.draw_MiniPreview();
	NDI_Input2.draw_MiniPreview();
#endif

	//--

#ifdef USE_WEBCAM
	// WEBCAM
	if (bWebCam_Enable && bWebCam_Draw) {
		if (bWebCam_Enable)
			if (bWebCam_Draw)
				if (bWebCam_DrawMini) draw_WebCam_MiniPreview(true);
	}
#endif

	//--

#ifdef USE_ofxNDI_OUT
	// NDI OUT
	if (bNDI_Output_Enable && bNDI_Output_Draw) {
		if (bNDI_Output_Mini) draw_NDI_OUT_MiniPreview(true);
	}
#endif

	//--

	if (bDebug) draw_InfoDevices();
}
//--------------------------------------------------------------
void ofxNDIHelper::draw() {
	//if (!bGui) return;
	//if (!bLoadedStartupDone) return;

	//--

#ifdef USE_ofxNDI_IN
	NDI_Input1.draw();
	NDI_Input2.draw();
#endif

	//--

#ifdef USE_WEBCAM
	// WEBCAM
	if (bWebCam_Enable && bWebCam_Draw) {
		draw_WebCam();
	}
#endif

	//--

#ifdef USE_ofxNDI_OUT
	// NDI OUT
	if (bNDI_Output_Enable && bNDI_Output_Draw) {
		draw_NDI_OUT();
	}

#endif

	//--

	if (bDebug) draw_InfoDevices();
}

//--------------------------------------------------------------
void ofxNDIHelper::draw_Gui() {
	ofDisableDepthTest();

	// HelpBox
	if (bHelp) textBoxWidget.draw();

	//--

	if (!bGui) return;
	if (!bMode_ofxGui) return;

	//--

	if (bGui_Internal) {
		// Gui Panel
		//if (bGui_Controls)
		if (bMode_ofxGui) {
			bool bGuiLink = true;
			if (bGuiLink) {
				glm::vec2 pad(1, 0);
				auto p = gui_Controls.getShape().getTopRight() + pad;

#ifdef USE_WEBCAM
				if (bGui_WebCam) {
					gui_WebCam.setPosition(p);
					p = gui_WebCam.getShape().getTopRight() + pad;
				}
#endif

#ifdef USE_ofxNDI_IN
				if (NDI_Input1.bGui_Internal) {
					NDI_Input1.gui_Control.setPosition(p);
					p = NDI_Input1.gui_Control.getShape().getTopRight() + pad;
				}
				if (NDI_Input2.bGui_Internal) {
					NDI_Input2.gui_Control.setPosition(p);
					p = NDI_Input2.gui_Control.getShape().getTopRight() + pad;
				}
#endif

#ifdef USE_ofxNDI_OUT
				if (bGui_NDI_OUT) {
					gui_NDI_Out.setPosition(p);
				}
#endif
			}

			//--

			if (bMode_ofxGui) {
				// Main Gui
				gui_Controls.draw();

				// Other panels

#ifdef USE_WEBCAM
				// WebCam
				if (bGui_WebCam) gui_WebCam.draw();
#endif

#ifdef USE_ofxNDI_IN
				// Inputs
				NDI_Input1.drawGui();
				NDI_Input2.drawGui();
#endif

#ifdef USE_ofxNDI_OUT
				// Output
				if (bGui_NDI_OUT) gui_NDI_Out.draw();
#endif
			}
		}
	}
}

// keys
//--------------------------------------------------------------
void ofxNDIHelper::keyPressed(ofKeyEventArgs & eventArgs) {
	// disabler for all keys. (independent from bActive)
	if (!bKeys) return;

	const int & key = eventArgs.key;
	ofLogNotice(__FUNCTION__) << "keyPressed: " << (char)key << " [" << key << "]";

	// modifiers
	bool mod_COMMAND = eventArgs.hasModifier(OF_KEY_COMMAND);
	bool mod_CONTROL = eventArgs.hasModifier(OF_KEY_CONTROL);
	bool mod_ALT = eventArgs.hasModifier(OF_KEY_ALT);
	bool mod_SHIFT = eventArgs.hasModifier(OF_KEY_SHIFT);

	if (0) {
		ofLogNotice(__FUNCTION__) << "mod_COMMAND : " << (mod_COMMAND ? "ON" : "OFF");
		ofLogNotice(__FUNCTION__) << "mod_CONTROL : " << (mod_CONTROL ? "ON" : "OFF");
		ofLogNotice(__FUNCTION__) << "mod_ALT     : " << (mod_ALT ? "ON" : "OFF");
		ofLogNotice(__FUNCTION__) << "mod_SHIFT   : " << (mod_SHIFT ? "ON" : "OFF");
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

	//-

	if (0) {
	}

	else if (key == 'G') {
		bGui = !bGui;
	}
	//else if (key == 'g')
	//{
	//	bGui_Controls = !bGui_Controls;
	//}

	//--

	//else if (key == 'E')
	//{
	//	bEdit = !bEdit;
	//}

	else if (key == 'D') {
		bDebug = !bDebug;
	}

	else if (key == 'H') {
		bHelp = !bHelp;
	}

	else if (key == 'R') {
		doReset_Mini_PreviewsLayout();
	}

	//--

#ifdef USE_WEBCAM
	// WEBCAM
	else if (key == 'I') {
		webcam_Next();
	}

	else if (mod_ALT)
	//else if (key == key_ChildFrame)
	{
		bKeyChildFrameState = true;
	}
#endif

	//--

#ifdef USE_ofxNDI_IN
	else if (key == ' ') {
		NDI_Input1.doScan();
		NDI_Input2.doScan();
	}
#endif

	//--
}

//--------------------------------------------------------------
void ofxNDIHelper::keyReleased(ofKeyEventArgs & eventArgs) {
	if (!bKeys) return;

	const int & key = eventArgs.key;
	ofLogNotice(__FUNCTION__) << "keyReleased: " << (char)key << " [" << key << "]";

	// modifiers
	bool mod_COMMAND = eventArgs.hasModifier(OF_KEY_COMMAND);
	bool mod_CONTROL = eventArgs.hasModifier(OF_KEY_CONTROL);
	bool mod_ALT = eventArgs.hasModifier(OF_KEY_ALT);
	bool mod_SHIFT = eventArgs.hasModifier(OF_KEY_SHIFT);

	if (0) {
		ofLogNotice(__FUNCTION__) << "mod_COMMAND : " << (mod_COMMAND ? "ON" : "OFF");
		ofLogNotice(__FUNCTION__) << "mod_CONTROL : " << (mod_CONTROL ? "ON" : "OFF");
		ofLogNotice(__FUNCTION__) << "mod_ALT     : " << (mod_ALT ? "ON" : "OFF");
		ofLogNotice(__FUNCTION__) << "mod_SHIFT   : " << (mod_SHIFT ? "ON" : "OFF");
	}

	//--

#ifdef USE_WEBCAM
	// WEBCAM
	//if (key == key_ChildFrame)
	if (mod_ALT || key == 4) {
		bKeyChildFrameState = false;
	}
#endif
}

//--------------------------------------------------------------
void ofxNDIHelper::addKeysListeners() {
	ofAddListener(ofEvents().keyPressed, this, &ofxNDIHelper::keyPressed);
	ofAddListener(ofEvents().keyReleased, this, &ofxNDIHelper::keyReleased);
}

//--------------------------------------------------------------
void ofxNDIHelper::removeKeysListeners() {
	ofRemoveListener(ofEvents().keyPressed, this, &ofxNDIHelper::keyPressed);
	ofRemoveListener(ofEvents().keyReleased, this, &ofxNDIHelper::keyReleased);
}

//--------------------------------------------------------------
void ofxNDIHelper::setActive(bool b) {
	ofLogNotice(__FUNCTION__) << b;

	bActive = b;
	if (!b) {
		removeKeysListeners();
	} else {
		addKeysListeners();
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::setGuiVisible(bool b) {
	bGui = b;
}

//--------------------------------------------------------------
void ofxNDIHelper::setGuiInternalVisible(bool b) {
	bGui_Internal = b;
}

//--------------------------------------------------------------
void ofxNDIHelper::setGuiToggleVisible() {
	bGui = !bGui;
}

//--------------------------------------------------------------
void ofxNDIHelper::doReset_Gui() {
	// Positions
	position_Gui = glm::vec2(padg, padg);
	gui_Controls.setPosition(position_Gui.get().x, position_Gui.get().y);

	float w = gui_Controls.getWidth();
	glm::vec2 pg = glm::vec2(position_Gui.get());
	glm::vec2 p = glm::vec2(pg.x + w + padg, pg.y);

#ifdef USE_WEBCAM
	gui_WebCam.setPosition(p.x, p.y);
	gui_NDI_Out.setPosition(p.x + gui_WebCam.getWidth() + padg, p.y);
#else
	gui_NDI_Out.setPosition(p.x, p.y);
#endif

#ifdef USE_ofxNDI_IN
	glm::vec2 pTr = glm::vec2(gui_NDI_Out.getShape().getTopRight());

	glm::vec2 p2 = glm::vec2(pTr.x + padg, pTr.y);
	NDI_Input1.setPositionGui(p2);

	glm::vec2 p3 = glm::vec2(p2.x + padg + w, p2.y);
	NDI_Input2.setPositionGui(p3);
#endif
}

#ifdef USE_ofxNDI_OUT
//--------------------------------------------------------------
void ofxNDIHelper::Changed_NDI_Out(ofAbstractParameter & e) {
	if (bDISABLE_CALLBACKS) return;

	string name = e.getName();

	// exclude debugs
	#ifdef USE_WEBCAM
	if (name != scaleMode_Name.getName()) {
		ofLogNotice(__FUNCTION__) << name << " : " << e;
	}
	#endif

	if (0) {
	}

	//----

	// NDI OUT

	else if (name == bNDI_Output_Enable.getName())
	//else if (name == bNDI_Output_Enable.getName() && bNDI_Output_Enable.get())
	{
		if (bNDI_Output_Enable.get()) {
			//setup_NDI_OUT();
		} else {
			//NDI_OUT_Sender.ReleaseSender();//?
		}
	} else if (name == NDI_Output_Name.getName()) {
		setup_NDI_OUT();
	}
}
#endif

#ifdef USE_WEBCAM
//--------------------------------------------------------------
void ofxNDIHelper::Changed_WebCam(ofAbstractParameter & e) {
	if (bDISABLE_CALLBACKS) return;

	string name = e.getName();

	// exclude debugs
	if (name != scaleMode_Name.getName()
		//	&& name != "exclude"
	) {
		ofLogNotice(__FUNCTION__) << name << " : " << e;
	}

	if (0) {
	}

	//----

	else if (name == bWebCam_Enable.getName()) {
		if (bWebCam_Enable.get())
			doRestart_WebCam();
		else
			webcam_Grabber.close();
	}

	else if (name == webcam_Name.getName()) {
		//TODO:
		// 1. get index for a name
		// 2. setup cam for index
	}

	else if (name == webcam_Index_Device.getName() && bWebCam_Enable.get()) {
		setup_WebCam(webcam_Index_Device.get());
	}

	else if (name == bWebCam_Next.getName() && bWebCam_Next.get()) {
		bWebCam_Next = false;
		webcam_Next();
	}

	else if (name == bWebCam_Restart.getName() && bWebCam_Restart) {
		bWebCam_Restart = false;

		bWebCam_Enable = false;
		// must close before reopen
		webcam_Grabber.close();
		//doRestart_WebCam();
		setup_WebCam(webcam_Index_Device);
		bWebCam_Enable = true;
	}
}
#endif

//--------------------------------------------------------------
void ofxNDIHelper::Changed(ofAbstractParameter & e) {
	if (bDISABLE_CALLBACKS) return;

	string name = e.getName();

	// exclude debugs
	//if (name != "exclude"
	//	&& name != "exclude")
	{
		ofLogNotice(__FUNCTION__) << name << " : " << e;
	}

	//----

	if (0) {
	}

	else if (name == bResetLayout.getName() && bResetLayout) {
		bResetLayout = false;

		doReset_Mini_PreviewsLayout();
	}

	else if (name == bResetGui.getName() && bResetGui) {
		bResetGui = false;

		doReset_Gui();
	}

	//----

	else if (name == position_Gui.getName()) {
		gui_Controls.setPosition(position_Gui.get().x, position_Gui.get().y);
	}

	else if (name == bActive.getName()) {
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

#ifdef USE_WEBCAM
// WEBCAM

// TODO:
//--------------------------------------------------------------
void ofxNDIHelper::doRestart_WebCam() {
	// must close before reopen
	webcam_Grabber.close();

	// start device
	{
		webcam_Grabber.setDeviceID(webcam_Index_Device.get());
		//webcam_Grabber.setDesiredFrameRate(60);
		webcam_Grabber.setup(1920, 1080);
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::setup_WebCam(int index) {
	// will load from index (webcam_Index_Device) not from name

	// get back a list of devices.
	webcam_Grabber.setVerbose(true);
	vector<ofVideoDevice> _devs = webcam_Grabber.listDevices();
	webcam_Index_Device.setMax(_devs.size() - 1);

	// get all dev names
	webcam_Names_InputDevices = "";
	webcam_Names.clear();
	for (size_t i = 0; i < _devs.size(); i++) {
		// queue devices names
		webcam_Names_InputDevices += ofToString(i) + " " + _devs[i].deviceName;
		if (i != _devs.size() - 1) webcam_Names_InputDevices += "\n";

		webcam_Names.push_back(_devs[i].deviceName);
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

	// valid index
	if (!bError) {
		// must close before reopen
		webcam_Grabber.close();

		// start device
		{
			webcam_Grabber.setDeviceID(webcam_Index_Device.get());

			//TODO:
			// now size is hardcoded
			//
			//webcam_Grabber.setDesiredFrameRate(60);
			webcam_Grabber.setup(1920, 1080);
		}

		//--

		// debug connected
		bool _isConnected = webcam_Grabber.isInitialized();
		ofLogNotice(__FUNCTION__) << "webcam_Grabber INITIALIZED: " << (_isConnected ? "TRUE" : "FALSE");

		if (!_isConnected) {
			ofLogError(__FUNCTION__) << "CAN'T INITIALIZE webcam_Grabber!";
			ofLogError(__FUNCTION__) << "CAMERA DISABLED";

			bWebCam_Enable = false;

			//return;
		}

		//--

		if (webcam_Index_Device < _devs.size()) {
			webcam_Name_ = _devs[webcam_Index_Device].deviceName;
			webcam_Name = webcam_Name_;
		}

		//--

		////TODO: child frame
		//float w = webcam_Grabber.getWidth();
		//float h = webcam_Grabber.getHeight();
		//float wc = w / 2.f;
		//float hc = h / 2.f;
		//frame_.setSize(w, h);
		//frame_.getInnerTransformNode().setAnchorPoint(wc, hc, 0);
		//frame_.getInnerTransformNode().setTranslation(wc, hc, 0);
	}
}
	#ifdef USE_OFX_CHILD_FRAME

//--------------------------------------------------------------
void ofxNDIHelper::mouseDragged(ofMouseEventArgs & mouse) {

	int x = mouse.x;
	int y = mouse.y;
	int button = mouse.button;

	if (!bEnable_ChildFrame) return;
	if (!bKeyChildFrameState) return;

	switch (button) {

	//case OF_MOUSE_BUTTON_2:
	case OF_MOUSE_BUTTON_LEFT: {
		TransformNode & node = frame_.getInnerTransformNode();
		node.addTranslationX(x - ofGetPreviousMouseX());
		node.addTranslationY(y - ofGetPreviousMouseY());

		childFrame.anchor = node.getAnchorPoint();
		childFrame.translat = node.getTranslation();
		childFrame.scale = node.getScale();
	} break;

	case OF_MOUSE_BUTTON_RIGHT: {
		TransformNode & node = frame_.getInnerTransformNode();

		float sz = -100;
		float diff = y - ofGetPreviousMouseY();
		//float diff = MAX(x - ofGetPreviousMouseX(), y - ofGetPreviousMouseY());
		ofVec3f v(sz + diff, sz + diff, sz);
		node.mulScale(v / sz);

		// Clamp
		auto s = node.getScale();
		float sxclamp = ofClamp(s.x, 1, CHILD_FRAME_MAX_SCALE);
		float syclamp = ofClamp(s.y, 1, CHILD_FRAME_MAX_SCALE);
		node.setScaleX(sxclamp);
		node.setScaleY(syclamp);

		childFrame.anchor = node.getAnchorPoint();
		childFrame.translat = node.getTranslation();
		childFrame.scale = node.getTranslation();
	} break;
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::mouseScrolled(ofMouseEventArgs & mouse) {
	if (!bEnable_ChildFrame) return;
	if (!bKeyChildFrameState) return;

	float ms = mouse.scrollY;
	float step = 5;
	float st = ofMap(ms, -2, 2, -step, step);

	TransformNode & node = frame_.getInnerTransformNode();

	float n = 100; //bigger is slower/sensible

	ofVec3f v(n + st, n + st, n);

	//v = ofVec3f (
	//	ofClamp(v.x, 1, CHILD_FRAME_MAX_SCALE),
	//	ofClamp(v.y, 1, CHILD_FRAME_MAX_SCALE),
	//	ofClamp(n, 1, CHILD_FRAME_MAX_SCALE));

	node.mulScale(v / n);

	// Clamp
	auto s = node.getScale();
	float sxclamp = ofClamp(s.x, 1, CHILD_FRAME_MAX_SCALE);
	float syclamp = ofClamp(s.y, 1, CHILD_FRAME_MAX_SCALE);
	node.setScaleX(sxclamp);
	node.setScaleY(syclamp);

	childFrame.anchor = node.getAnchorPoint();
	childFrame.translat = node.getTranslation();
	childFrame.scale = node.getTranslation();
}
	#endif

//--------------------------------------------------------------
void ofxNDIHelper::setup_WebCam() {
	ofLogNotice(__FUNCTION__);

	// List the cam devices. nothing more yet!

	// will initialized the device from the loaded name from the xml settings, not from the index number!
	// that's because the index of a device could be changed if a new device is connected.

	// get back a list of devices.
	webcam_Grabber.setVerbose(true);
	//vector<ofVideoDevice> _devs = webcam_Grabber.listDevices();
	_devs = webcam_Grabber.listDevices();

	// get all dev names
	webcam_Names_InputDevices = "";
	webcam_Index_Device.setMax(_devs.size() - 1);
	ofLogNotice(__FUNCTION__) << "LIST WEBCAM DEVICES:";

	webcam_Names.clear();

	// log list devices
	for (size_t i = 0; i < _devs.size(); i++) {
		////TODO:
		////should skip names for not available devices...
		//if (_devs[i].bAvailable)
		//{

		// queue devices names
		webcam_Names_InputDevices += ofToString(i) + " " + _devs[i].deviceName;

		webcam_Names_InputDevices += "\n";
		//if (i != _devs.size() - 1) webcam_Names_InputDevices += "\n";

		if (_devs[i].bAvailable) {
			// log the device
			ofLogNotice(__FUNCTION__) << _devs[i].id << ": " << _devs[i].deviceName;
		} else {
			// log the device and note it as unavailable
			ofLogNotice(__FUNCTION__) << _devs[i].id << ": " << _devs[i].deviceName << " - unavailable ";
		}

		webcam_Names.push_back(_devs[i].deviceName);
	}

	//--

	#ifdef USE_WEBCAM
	//rect_WebCam.loadSettings(path_rect_WebCam, path_GLOBAL, false);
	rect_WebCam.setModeLayout(ofxSurfingBox::FREE_LAYOUT);
	rect_WebCam.setName(path_rect_WebCam);
	rect_WebCam.setPathGlobal(path_GLOBAL + path_WebCamSettings);
	rect_WebCam.setup();
	rect_WebCam.setUseBorder(true);

		#ifdef USE_OFX_CHILD_FRAME
	x = rect_WebCam.getX();
	y = rect_WebCam.getY();
	w = rect_WebCam.getWidth();
	h = rect_WebCam.getHeight();
	wc = w / 2.f;
	hc = h / 2.f;

	frame_.setSize(w, h);
	frame_.getInnerTransformNode().setAnchorPoint(x + wc, y + hc, 0);
	frame_.getInnerTransformNode().setTranslation(wc, hc, 0);
		#endif
	#endif
}

//--------------------------------------------------------------
void ofxNDIHelper::draw_WebCam_MiniPreview(bool bInfo) {
	if (!bWebCam_Enable) return;
	if (!bWebCam_Draw) return;
	if (!bWebCam_DrawMini) return;

	//----

	ofPushStyle();
	//ofSetColor(255, 255);

	//TODO:
	#ifdef USE_OFX_CHILD_FRAME

	//TODO: child frame

	if (rect_WebCam.isChanged()) {
		//cout << __FUNCTION__ << "rect_WebCam.isChanged()" << endl;

		bEnable_ChildFrame = !rect_WebCam.isEditing();

		x = rect_WebCam.getX();
		y = rect_WebCam.getY();
		w = rect_WebCam.getWidth();
		h = rect_WebCam.getHeight();
		wc = w / 2.f;
		hc = h / 2.f;

		frame_.setSize(w, h);

		////if (bEnable_ChildFrame)
		//{
		//  frame_.setSize(w, h);
		//	TransformNode& node = frame_.getInnerTransformNode();
		//	node.setAnchorPoint(x + wc, y + hc, 0);
		//	node.setTranslation(wc, hc, 0);
		//	//node.setAnchorPoint(childFrame.anchor);
		//	//node.setTranslation(childFrame.translat);
		//	//node.setScale(childFrame.scale);
		//}
	}

	//if (bEnable_ChildFrame)
	{
		x = rect_WebCam.getX();
		y = rect_WebCam.getY();
		w = rect_WebCam.getWidth();
		h = rect_WebCam.getHeight();
		wc = w / 2.f;
		hc = h / 2.f;

		// required to mantain position after moving box..
		//frame_.setSize(w, h);
		TransformNode & node = frame_.getInnerTransformNode();
		node.setAnchorPoint(x + wc, y + hc, 0);
		node.setTranslation(wc, hc, 0);

		/*
		TransformNode& node = frame_.getInnerTransformNode();
		node.setAnchorPoint(childFrame.anchor);
		node.setTranslation(childFrame.translat);
		node.setScale(childFrame.scale);
		*/
	}

	// draw
	frame_.begin();
	ofSetColor(255, 255);
	webcam_Grabber.draw(x, y, w, h);
	frame_.end();
	frame_.draw(x, y);

	// debug
	if (0) {
		stringstream ss;
		ss << "anchor     : " << childFrame.anchor << endl;
		ss << "translate  : " << childFrame.translat << endl;
		ss << "scale      : " << childFrame.scale << endl
		   << endl;
		ofDrawBitmapStringHighlight(ss.str(), x + w + 10, y + h + 10);
	}
	#endif

	#ifndef USE_OFX_CHILD_FRAME
	x = rect_WebCam.getX();
	y = rect_WebCam.getY();
	w = rect_WebCam.getWidth();
	h = rect_WebCam.getHeight();
	//ofSetColor(255, 255);
	webcam_Grabber.draw(x, y, w, h);
	#endif

	//--

	//TODO:

	// Viewport

	ofRectangle rSrc(0, 0, webcam_Grabber.getWidth(), webcam_Grabber.getHeight());
	ofRectangle rOut(rect_WebCam.getX(), rect_WebCam.getY(), rect_WebCam.getWidth(), rect_WebCam.getHeight());

	if (bWebCam_LockRatio.get()) {
		float _ratio = webcam_Grabber.getHeight() / webcam_Grabber.getWidth();
		rect_WebCam.setHeight(rect_WebCam.getWidth() * _ratio);
	}

	ofScaleMode scaleMode;
	switch (scaleMode_Index) {
	case 0:
		scaleMode = OF_SCALEMODE_FIT;
		scaleMode_Name = "FIT";
		break;
	case 1:
		scaleMode = OF_SCALEMODE_FILL;
		scaleMode_Name = "FILL";
		break;
	case 2:
		scaleMode = OF_SCALEMODE_CENTER;
		scaleMode_Name = "CENTER";
		break;
	case 3:
		scaleMode = OF_SCALEMODE_STRETCH_TO_FILL;
		scaleMode_Name = "STRETCH_TO_FILL";
		break;
	}

	//rOut.scaleTo(rSrc, scaleMode);
	rSrc.scaleTo(rOut, scaleMode);

	// image

	////webcam_Grabber.draw(rect_WebCam.getX(), rect_WebCam.getY(), rect_WebCam.getWidth(), rect_WebCam.getHeight());

	////webcam_Grabber.draw(rOut.getX(), rOut.getY(), rOut.getWidth(), rOut.getHeight());
	//webcam_Grabber.draw(rOut.getX(), rOut.getY(), rOut.getWidth(), rOut.getHeight());

	// interactive box
	rect_WebCam.draw();

	ofPopStyle();
}

//--------------------------------------------------------------
void ofxNDIHelper::webcam_SaveSettings() {
	ofLogNotice(__FUNCTION__);

	ofXml _xml;
	ofSerialize(_xml, webcam_Name_);
	ofxSurfingHelpers::CheckFolder(path_GLOBAL + path_WebCamSettings);
	_xml.save(path_GLOBAL + path_WebCamSettings + name_WebCamSettings);
}

//--------------------------------------------------------------
void ofxNDIHelper::webcam_LoadSettings() {
	ofLogNotice(__FUNCTION__) << endl;
	ofLogNotice(__FUNCTION__) << "--------------------------------";

	//--

	#ifdef FIX_WORKAROUND_STARTUP_FREEZE // Sometimes WebCam hangs on startup
	// fix workaround startup
	if (bWebCam_Enable.get()) {
		bDoRestartup = true;
		bWebCam_Enable = false;
		webcam_Grabber.close();
	}
	#endif

	//--

	//TODO: use this file for settings.
	//TODO: camera is loading from index on app settings...

	ofXml _xml;
	bool _isLoaded = _xml.load(path_GLOBAL + path_WebCamSettings + name_WebCamSettings);
	ofDeserialize(_xml, webcam_Name_);

	ofLogNotice(__FUNCTION__) << endl
							  << "XML CONTENT : \n"
							  << _xml.toString();
	ofLogNotice(__FUNCTION__) << endl
							  << "LOOKING FOR DEVICE : \n"
							  << webcam_Name_.get();

	//--

	webcam_Index_Device.setWithoutEventNotifications(-1);

	if (_isLoaded) {
		// Iterate devices names to found the device name read from the file settings.
		for (int i = 0; i < _devs.size(); i++) {
			ofLogNotice(__FUNCTION__) << "DEVICE #" << i << " " << _devs[i].deviceName;

			if (_devs[i].deviceName == webcam_Name_.get()) {
				webcam_Index_Device = i;

				ofLogNotice(__FUNCTION__) << "DEVICE FROM SETTINGS FOUND!";
				ofLogNotice(__FUNCTION__) << "DEVICE NAME  : \t" << webcam_Name_.get();
				ofLogNotice(__FUNCTION__) << "DEVICE INDEX : \t" << webcam_Index_Device;

				break;
			}
		}
	} else {
		ofLogError(__FUNCTION__) << "SETTINGS FILE NOT FOUND!";
		ofLogError(__FUNCTION__) << "FORCE TRY TO LOAD THE FIRST AVAILABLE DEVICE";
	}

	// Error. Desired name is not on the current list of devices!
	// Then we try to load the first device...

	if (webcam_Index_Device == -1) {
		webcam_Index_Device = 0; // force select first cam device

		if (webcam_Index_Device < _devs.size()) {
			webcam_Name_ = _devs[webcam_Index_Device].deviceName;
			webcam_Name = _devs[webcam_Index_Device].deviceName;
		} else {
			ofLogError(__FUNCTION__) << "CAMERA INDEX OUT OF RANGE";
			webcam_Name = webcam_Name_ = "UNKNOWN DEVICE";
			webcam_Grabber.close();
			return; //cancel and exit!
		}
	}

	webcam_Name = webcam_Name_;

	//--

	// 1. Must close before reopen

	webcam_Grabber.close();

	//--

	// 2. Start device

	//if (bWebCam_Enable.get())
	{
		ofLogNotice(__FUNCTION__) << "OPENING DEVICE!";

		webcam_Grabber.setDeviceID(webcam_Index_Device.get());
		//webcam_Grabber.setDesiredFrameRate(60);
		webcam_Grabber.setup(1920, 1080);
	}

	//--

	//// debug connected
	//bool _isConnected = webcam_Grabber.isInitialized();
	//ofLogNotice(__FUNCTION__) << "webcam_Grabber INITIALIZED: " << (_isConnected ? "TRUE" : "FALSE");
	//if (!_isConnected) {
	//	ofLogError(__FUNCTION__) << "CAN'T INITIALIZE webcam_Grabber!";
	//	ofLogError(__FUNCTION__) << "CAMERA DISABLED";
	//	bWebCam_Enable = false;
	//}
}

#endif

//--------------------------------------------------------------
void ofxNDIHelper::doReset_Mini_PreviewsLayout() {
// Align all the previews using WebCam as anchor!
#ifdef USE_WEBCAM
	float _xx0 = rect_WebCam.getX();
	float _yy0 = rect_WebCam.getY();
#else
	float _xx0 = 100;
	float _yy0 = 250;
#endif

	float _ratio;
	float _xx;
	float _yy;
	float _ww;
	float _hh;

	//--

#ifdef USE_WEBCAM

	//rect_WebCam.reset();

	_ratio = webcam_Grabber.getHeight() / webcam_Grabber.getWidth();

	_xx = _xx0;
	_yy = _yy0;
	_ww = rect_WebCam.getWidth();
	//_ww = wPreview;
	_hh = _ww * _ratio;

	rect_WebCam.setShape(ofRectangle(_xx, _yy, _ww, _hh));

#endif

	//--

#ifdef USE_ofxNDI_OUT
	//rect_NDI_OUT.reset();

	_ratio = fbo_NDI_Sender.getHeight() / fbo_NDI_Sender.getWidth();

	//// A. below webcam
	//_xx = _xx0;
	//_yy += _hh + yPadPreview;
	//_ww = rect_NDI_OUT.getWidth();
	////_ww = wPreview;
	//_hh = _ww * _ratio;

	// B. on the bottom right
	//_ww = rect_NDI_OUT.getWidth();
	//_yy += _hh + yPadPreview;
	_ww = wPreview;
	_hh = _ww * _ratio;
	_xx = ofGetWidth() - 15 - _ww;
	_yy = ofGetHeight() - 50 - _hh;

	rect_NDI_OUT.setShape(ofRectangle(_xx, _yy, _ww, _hh));
#endif

	//--

#ifdef USE_ofxNDI_IN
	//NDI_Input1.doReset_Mini_PreviewsSize();
	#ifdef USE_WEBCAM
	_xx = _xx0 + MAX(rect_WebCam.getWidth(), rect_NDI_OUT.getWidth()) + pad;
	#else
		#ifdef USE_ofxNDI_OUT
	_xx = _xx0 + rect_NDI_OUT.getWidth() + pad;
		#else
	_xx = _xx0;
		#endif
	#endif

	_yy = _yy0;
	//_ww = wPreview;
	_hh = _ww * _ratio;

	auto r = NDI_Input1.getPreviewRect();
	_ratio = r.getHeight() / r.getWidth();
	_ww = r.getWidth();

	NDI_Input1.setPositionPreview(glm::vec2(_xx, _yy));

	//--

	//NDI_Input2.doReset_Mini_PreviewsSize();

	_xx = _xx + _ww + pad;

	auto r2 = NDI_Input2.getPreviewRect();
	_ratio = r2.getHeight() / r2.getWidth();
	_ww = r2.getWidth();

	NDI_Input2.setPositionPreview(glm::vec2(_xx, _yy));
#endif
}

#ifdef USE_WEBCAM
//--------------------------------------------------------------
void ofxNDIHelper::draw_WebCam() {
	if (!bWebCam_Enable) return;
	if (!bWebCam_Draw) return;

	if (bWebCam_DrawMini)
		draw_WebCam_MiniPreview(true);
	else
		draw_WebCam_Full();
}

//--------------------------------------------------------------
void ofxNDIHelper::draw_WebCam_Full() {
	if (!bWebCam_Enable) return;
	if (!bWebCam_Draw) return;

	//TODO:
	// making fit view ports

	ofPushStyle();
	ofSetColor(255, 255);

	ofScaleMode scaleMode;
	switch (scaleMode_Index) {
	case 0:
		scaleMode = OF_SCALEMODE_FIT;
		scaleMode_Name = "FIT";
		break;
	case 1:
		scaleMode = OF_SCALEMODE_FILL;
		scaleMode_Name = "FILL";
		break;
	case 2:
		scaleMode = OF_SCALEMODE_CENTER;
		scaleMode_Name = "CENTER";
		break;
	case 3:
		scaleMode = OF_SCALEMODE_STRETCH_TO_FILL;
		scaleMode_Name = "STRETCH_TO_FILL";
		break;
	}

	ofRectangle rOut(0, 0, NDI_OUT_Sender.GetWidth(), NDI_OUT_Sender.GetHeight());
	//ofRectangle rOut = ofGetWindowRect();

	ofRectangle rSrc(0, 0, webcam_Grabber.getWidth(), webcam_Grabber.getHeight());

	rOut.scaleTo(rSrc, scaleMode);
	//rSrc.scaleTo(rOut, scaleMode);

	webcam_Grabber.draw(rOut.x, rOut.y, rOut.width, rOut.height);
	//webcam_Grabber.draw(rSrc.x, rSrc.y, rSrc.width, rSrc.height);

	ofPopStyle();
}
#endif

//--

#ifdef USE_ofxNDI_OUT
//--------------------------------------------------------------
void ofxNDIHelper::setup_NDI_OUT() {

	NDI_OUT_Sender.ReleaseSender(); //?

	ofLogNotice(__FUNCTION__) << NDI_OUT_Sender.GetNDIversion() << " (http://ndi.tv/)";
	//strcpy_s(senderName, 256, "OF NDI Sender"); // Set the sender name

	const std::string str = NDI_Output_Name.get();

	//    strcpy_s(senderName, 256, str.c_str()); //Set the sender name // VS Windows. fails on macOS
	//    strncpy(senderName, 256, str.c_str()); //Set the sender name // macOS Xcode
	std::snprintf(senderName, sizeof(senderName), "%s", str.c_str()); // macOS Xcode

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
	NDI_OUT_Sender.SetReadback(); // Change to false to compare

	// Set NDI asynchronous sending for best performance
	NDI_OUT_Sender.SetAsync();

	////Create a new sender - default RGBA for ofPixels
	//NDI_OUT_Sender.CreateSender("Openframeworks NDI webcam", (unsigned int)webcam_Grabber.getWidth(), (unsigned int)webcam_Grabber.getHeight());
	//ofLogNotice(__FUNCTION__) << "Created NDI sender [Openframeworks NDI webcam] (" << webcam_Grabber.getWidth() << "x" << webcam_Grabber.getHeight() << ")" ;

	// Create a sender with default RGBA output format
	NDI_OUT_Sender.CreateSender(senderName, senderWidth, senderHeight);
	//NDI_OUT_Sender.CreateSender("OF TEST", senderWidth, senderHeight);

	ofLogNotice(__FUNCTION__) << "Created NDI sender [" << senderName << "] (" << senderWidth << "x" << senderHeight << ")";
}
#endif

#ifdef USE_ofxNDI_OUT || USE_ofxNDI_IN || USE_WEBCAM
//--------------------------------------------------------------
void ofxNDIHelper::draw_InfoDevices() {

	#ifdef USE_WEBCAM
	// WEBCAM
	if (bWebCam_Enable.get() && bWebCam_Draw.get() && bWebCam_DrawMini.get()) {
		//--

		// Top info

		if (bDebug) {
			string s = "WEBCAM | " + webcam_Name_.get();
			glm::vec2 p;

			if (bLabelsInner) {
				int h = pad / 2 + font.getSize() + padLabel;
				p = rect_WebCam.getRectangle().getTopLeft() + glm::vec2(_padx, h);
			} else {
				p = rect_WebCam.getRectangle().getTopLeft() + glm::vec2(_padx, _pady);
			}

			ofxSurfingHelpers::drawTextBoxed(font, s, p.x, p.y, 255, 0, false, 128, pad, round, -1, true);
		}

		//--

		// Display devices

		string s;
		s = "";
		s += webcam_Name_.get();
		s += " " + ofToString(webcam_Index_Device.get());
		s += "\n\n";
		s += webcam_Names_InputDevices;

		glm::vec2 p;
		if (bLabelsInner) {
			int h = ofxSurfingHelpers::getHeightBBtextBoxed(font, s);
			p = rect_WebCam.getRectangle().getBottomLeft() + glm::vec2(_padx2, _pady2 - h + 32);
		} else {
			p = rect_WebCam.getRectangle().getBottomLeft() + glm::vec2(_padx2, _pady2);
		}

		ofxSurfingHelpers::drawTextBoxed(font, s, p.x, p.y, 255, 0, false, 128, 20, round, -1, true);
	}
	#endif

	//--

	#ifdef USE_ofxNDI_OUT
	// NDI OUTPUT
	if (bNDI_Output_Enable.get() && bNDI_Output_Draw.get() && bNDI_Output_Mini.get()) {
		// Show what it's sending
		if (NDI_OUT_Sender.SenderCreated()) {
			//--

			// Top info

			if (bDebug) {
				string s = "NDI OUT | " + NDI_Output_Name.get();
				glm::vec2 p;

				if (bLabelsInner) {
					int h = pad / 2 + font.getSize() + padLabel;
					p = rect_NDI_OUT.getRectangle().getTopLeft() + glm::vec2(_padx, h);
				} else {
					p = rect_NDI_OUT.getRectangle().getTopLeft() + glm::vec2(_padx, _pady);
				}

				ofxSurfingHelpers::drawTextBoxed(font, s, p.x, p.y, 255, 0, false, 128, pad, round, -1, true);
			}

			//--

			// Display devices

			string s = "";
			s += ofToString(senderName) + "\n";
			s += ofToString(senderWidth) + "x" + ofToString(senderHeight) + "px | ";
			s += "FPS " + ofToString(NDI_OUT_Sender.GetFps()) + " / " + ofToString((int)ofGetFrameRate()) + "";

			x2 = rect_NDI_OUT.getX() + _padx2;
			y2 = rect_NDI_OUT.getY() + rect_NDI_OUT.getHeight() + _pady2;

			glm::vec2 p;
			if (bLabelsInner) {
				int h = ofxSurfingHelpers::getHeightBBtextBoxed(font, s);
				p = rect_NDI_OUT.getRectangle().getBottomLeft() + glm::vec2(_padx2, _pady2 - h + 32);
			} else {
				p = rect_NDI_OUT.getRectangle().getBottomLeft() + glm::vec2(_padx2, _pady2);
			}

			ofxSurfingHelpers::drawTextBoxed(font, s, p.x, p.y, 255, 0, false, 128, 20, round, -1, true);
		}
	}
	#endif
}

	//--

	// NDI OUT
	#ifdef USE_ofxNDI_OUT
//--------------------------------------------------------------
void ofxNDIHelper::begin_NDI_OUT() {
	if (bNDI_Output_Enable.get()) {
		fbo_NDI_Sender.begin();
		ofClear(0, 0, 0, 255);
	}
}

//--------------------------------------------------------------
void ofxNDIHelper::end_NDI_OUT() {
	if (bNDI_Output_Enable.get()) {
		// Overlay layer
		//if (bDebug.get()) ofDrawBitmapStringHighlight("NDI OUT", 40, 60);

		fbo_NDI_Sender.end();

		//--

		// do the sending

		ofColor(255);
		NDI_OUT_Sender.SendImage(fbo_NDI_Sender);
		//NDI_OUT_Sender.SendImage(fbo_NDI_Sender, false);// send processed
		//NDI_OUT_Sender.SendImage(webcam_Grabber.getPixels());// send clean webcam
	}
}

//--

// NDI Out
//--------------------------------------------------------------
void ofxNDIHelper::draw_NDI_OUT() {
	if (!bNDI_Output_Enable.get()) return;

	if (bNDI_Output_Mini.get())
		draw_NDI_OUT_MiniPreview(true);
	else
		draw_NDI_OUT_Full();
}

//--------------------------------------------------------------
void ofxNDIHelper::draw_NDI_OUT_MiniPreview(bool bInfo) {
	if (!bNDI_Output_Enable.get()) return;

	ofPushStyle();
	ofSetColor(255, 255);

	//if (bLockAspect.get())
	{
		float _ratio = fbo_NDI_Sender.getHeight() / fbo_NDI_Sender.getWidth();
		rect_NDI_OUT.setHeight(rect_NDI_OUT.getWidth() * _ratio);
	}

	fbo_NDI_Sender.draw(rect_NDI_OUT.getX(), rect_NDI_OUT.getY(), rect_NDI_OUT.getWidth(), rect_NDI_OUT.getHeight());
	rect_NDI_OUT.draw();

	//fbo_NDI_Sender.draw(0, 0, ofGetWidth(), ofGetHeight());

	ofSetColor(ofColor::yellow);
	ofNoFill();
	ofSetLineWidth(2);
	ofDrawRectangle(rect_NDI_OUT.getRectangle());

	ofPopStyle();
}

//--------------------------------------------------------------
void ofxNDIHelper::draw_NDI_OUT_Full() {
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
#else
//to not break the examples when all are disabled!
void ofxNDIHelper::begin_NDI_OUT() {};
void ofxNDIHelper::end_NDI_OUT() {};
void ofxNDIHelper::draw_InfoDevices() {};
#endif

//--------------------------------------------------------------
void ofxNDIHelper::loadSettings() {
	ofLogNotice(__FUNCTION__);

	// load file settings
	ofxSurfingHelpers::loadGroup(params_AppsSettings, path_GLOBAL + path_Params_AppSettings);

	//--

	// Reset preview rectangles positions and sizes
	//doReset_Mini_PreviewsLayout();

	// Load Settings

	//#ifdef USE_ofxNDI_OUT
	//	rect_NDI_OUT.loadSettings(path_rect_NDI_OUT, path_GLOBAL, false);
	//#endif
	//
	//#ifdef USE_WEBCAM
	//	rect_WebCam.loadSettings(path_rect_WebCam, path_GLOBAL, false);
	//#endif

	// fix workaround startup
	//bEdit_PRE = bEdit;
}

//--------------------------------------------------------------
void ofxNDIHelper::saveSettings() {
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
	//	rect_WebCam.saveSettings(path_rect_WebCam, path_GLOBAL, false);
	//#endif
}

//--------------------------------------------------------------
void ofxNDIHelper::exit() {
	ofLogNotice(__FUNCTION__);

	saveSettings();

	//--

#ifdef USE_ofxNDI_OUT
	// The sender must be released
	// or NDI sender discovery will still find it
	NDI_OUT_Sender.ReleaseSender();
#endif
}

//--------------------------------------------------------------
void ofxNDIHelper::setLogLevel(ofLogLevel level) {
	ofSetLogLevel("ofxNDIHelper", level);
}

//--------------------------------------------------------------
void ofxNDIHelper::windowResized(int w, int h) {
	screenW = w;
	screenH = h;

	//TODO:\
	// don't does nothing yet!
	//NDI_OUT_Sender.UpdateSender(1920, 1080);//update size

#ifdef USE_ofxNDI_IN
	NDI_Input1.windowResized(w, h);
	NDI_Input2.windowResized(w, h);
#endif
}
