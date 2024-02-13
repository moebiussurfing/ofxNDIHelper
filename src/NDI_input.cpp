#include "NDI_input.h"

#ifdef USE_ofxNDI_IN

//--------------------------------------------------------------
NDI_input::NDI_input() {
	ofAddListener(ofEvents().update, this, &NDI_input::update);
}

//--------------------------------------------------------------
NDI_input::~NDI_input() {
	ofRemoveListener(ofEvents().update, this, &NDI_input::update);

	ofRemoveListener(params.parameterChangedE(), this, &NDI_input::Changed);

	exit();
}

//--------------------------------------------------------------
void NDI_input::setup(string _name) {
	name = _name;
	path_rect_NDI_IN = "NDI_IN_" + name + "_Mini";

	rect_NDI_IN.setName(path_rect_NDI_IN);
	rect_NDI_IN.setPathGlobal(path_GLOBAL);
	rect_NDI_IN.setModeLayout(ofxSurfingBox::FREE_LAYOUT);
	rect_NDI_IN.setup();
	rect_NDI_IN.setUseBorder(true);

	path_Params_AppSettings = "NDI_IN_" + name + ".json";

	//string _str = "telegrama_render.otf";
	string _str = "JetBrainsMonoNL-ExtraBold.ttf";

	string _pathFont = "assets/fonts/" + _str;

	float _size;
	bool b;

	_size = 8;
	b = font.load(_pathFont, _size);
	if (!b) b = font.load(OF_TTF_MONO, _size);

	//--

	// window
	screenW = ofGetWidth();
	screenH = ofGetHeight();

	//--

	setup_Params();

	//--

	setup_Fbo();

	bDISABLE_CALLBACKS = false;

	//doScan();
	loadSettings();
}

//--------------------------------------------------------------
void NDI_input::startup() {
	ofLogNotice(__FUNCTION__) << "--------------------------------------------------------------";

	//loadSettings();

	doScan();

	//--

	bLoadedStartupDone = true;
}

//--------------------------------------------------------------
void NDI_input::startupDelayed() {
	ofLogNotice(__FUNCTION__) << "--------------------------------------------------------------";

	loadSettings();

	bStartupDelayedDone = true;
}

//--------------------------------------------------------------
void NDI_input::setup_Params() {
	bGui.set("NDI IN " + name, true);

	bGui_Internal.set("IN " + name, true);
	bGui_Preview.set("IN PREVIEW" + name, true);

	bLoad.set("Load Settings");

	bNext.set("Next", false);
	bLockAspect.set("Lock Aspect", true);
	bDebug.set("Debug", true);

	position_Gui.set("UI Position",
		glm::vec2(screenW * 0.5, screenH * 0.5),
		glm::vec2(0, 0),
		glm::vec2(screenW, screenH));

	scaleMode_Index.set("Scale", 0, 0, 3);
	scaleMode_Name.set("Scale Mode", "");

	bReset.set("Reset Preview", false);

	scaleMode_Names.clear();
	scaleMode_Names.push_back("FIT");
	scaleMode_Names.push_back("FILL");
	scaleMode_Names.push_back("CENTER");
	scaleMode_Names.push_back("STRETCH_TO_FILL");

	//--

	// Exclude

	#ifdef DEVICES_BY_NAME_INSTEAD_OF_BY_INDEX
	indexDevice.setSerializable(false);
	#else
	nameDevice.setSerializable(false);
	#endif
	bLoad.setSerializable(false);
	bReset.setSerializable(false);
	bNext.setSerializable(false);
	scaleMode_Name.setSerializable(false);

	//--

	// NDI IN

	bEnable.set("Enable", false);
	bDraw.set("Draw", true);
	bDrawMini.set("Mini", true);
	indexDevice.set("Device", 0, 0, 1);
	nameDevice.set("Name", "ofxNDIHelperIN");
	bScan.set("Scan", false);
	bScan.setSerializable(false);

	params_Settings.setName("NDI INPUT");
	params_Settings.add(bEnable);
	params_Settings.add(bDraw);
	params_Settings.add(bDrawMini);
	params_Settings.add(indexDevice);
	params_Settings.add(nameDevice);
	params_Settings.add(bNext);
	params_Settings.add(bScan); //now is automatic

	//--

	// Input

	params_Control.setName("CONTROL");
	params_Control.add(bDebug); //TODO: BUG: not linking when makeReference from parent scope..
	params_Control.add(rect_NDI_IN.bEdit);
	params_Control.add(bReset);
	params_Control.add(bLockAspect);
	params_Control.add(scaleMode_Index);
	params_Control.add(scaleMode_Name);
	params_Control.add(bLoad);
	params_Control.add(position_Gui);

	//--

	// All

	params.setName("NDI INPUT");
	params.add(params_Settings);
	params.add(params_Control);

	//--

	// Gui

	ofxSurfingHelpers::setThemeDarkMini_ofxGui();

	gui_Control.setup("IN " + name);
	gui_Control.add(params);

	auto & gc = gui_Control.getGroup(params.getName()).getGroup(params_Control.getName());
	auto & gp = gc.getGroup(position_Gui.getName());
	gc.minimize();
	gp.minimize();

	auto & gg = gui_Control.getGroup(params.getName());
	gg.minimize();

	//--

	// Callback

	ofAddListener(params.parameterChangedE(), this, &NDI_input::Changed);
}

//--

//--------------------------------------------------------------
void NDI_input::setup_Fbo() {
	ofLogNotice(__FUNCTION__);

	//--

	// Fbo

	// receiver dimensions and fps are not known yet

	//wReceiver = (unsigned char)ofGetWidth();
	//hReceiver = (unsigned char)ofGetHeight();

	//TODO:
	// hardcoded
	wReceiver = 1920;
	hReceiver = 1080;

	fbo_NDI_Receiver.allocate(wReceiver, hReceiver, GL_RGBA);

	//--

	// Clear the fbo so the first frame draw is black
	fbo_NDI_Receiver.begin();
	ofClear(0, 0, 0, 255);
	fbo_NDI_Receiver.end();

	//--

	// ofTexture
	tex_NDI_Receiver.allocate(wReceiver, hReceiver, GL_RGBA);
}

//--------------------------------------------------------------
void NDI_input::setup_ByIndex() {
	ofLogNotice(__FUNCTION__);

	//--

	nsenders = NDI_Receiver.GetSenderCount();
	indexDevice.setMax(nsenders - 1);

	int i = indexDevice.get();

	if (nsenders > 0 && i >= 0 && i < nsenders) {
		// Update the receiver with the returned index
		// Returns false if the current sender is selected

		if (NDI_Receiver.SetSenderIndex(i)) {
			nameDevice.setWithoutEventNotifications(NDI_Receiver.GetSenderName(i));

			ofLogNotice(__FUNCTION__) << "Device Name: " << nameDevice.get() << " #" << i;
		}

		else
			ofLogWarning(__FUNCTION__) << "Stay in same sender #" << i;
	} else {
		ofLogError(__FUNCTION__) << "NOT ANY NDI SENDERS!";
		ofLogError(__FUNCTION__) << "OR INDEX OUT OF RANGE.";
	}
}

//--------------------------------------------------------------
void NDI_input::setup_ByIndex(int _indexDevice) {
	ofLogNotice(__FUNCTION__);

	// Search the device by index

	if (indexDevice != _indexDevice) indexDevice.setWithoutEventNotifications(_indexDevice);

	setup_ByIndex();
}

//--------------------------------------------------------------
void NDI_input::setup_ByName(string _nameDevice) {
	ofLogNotice(__FUNCTION__);

	// Search the device by name

	if (nameDevice.get() != _nameDevice) nameDevice.setWithoutEventNotifications(_nameDevice);

	int _indexTarget = -1;

	//--

	ofLogNotice(__FUNCTION__);

	nsenders = NDI_Receiver.GetSenderCount();
	indexDevice.setMax(nsenders - 1);

	// List all the senders

	if (nsenders > 0) {
		ofLogNotice(__FUNCTION__) << "Number of NDI senders found: " << nsenders;

		for (int i = 0; i < nsenders; i++) {
			// Same name found!

			if (nameDevice.get() == NDI_Receiver.GetSenderName(i)) {
				_indexTarget = i;

				indexDevice.setWithoutEventNotifications(i);
				setup_ByIndex(indexDevice);

				break;
			}

			ofLogNotice(__FUNCTION__) << "Sender: #" << ofToString(i) + " " + name;
		}
	} else
		ofLogNotice(__FUNCTION__) << "No NDI senders found";

	if (_indexTarget == -1) {
		ofLogError(__FUNCTION__) << "Device name not found on NDI senders names list!";

		return;
	}
}

//--

//--------------------------------------------------------------
void NDI_input::drawGui() {
	//if (!bGui) return;
	if (!bGui_Internal) return;

	gui_Control.draw();
}

//--------------------------------------------------------------
void NDI_input::update(ofEventArgs & args) {
	updateWorkaround();
}

//--------------------------------------------------------------
void NDI_input::updateWorkaround() {
	//TODO: need to improve the startup process to correctly load settings
	// when scanning is ready done and corrent NDI sender name is found
	// currently broadcasting on the network.

	static int timerStartupDone = 0;

	if (bEnable) {
		static int nsenders_PRE = 0;
		if (nsenders != nsenders_PRE) {
			nsenders_PRE = nsenders;
			if (ofGetFrameNum() % 60 == 0) doListDevices(); //make slower. check once per sec
		}
	}

	if (bEnable)
		if (nsenders == 0 && (ofGetFrameNum() % 120 == 0)) {
			ofLogWarning(__FUNCTION__) << "NO DEVICES FOUND...";
		}

	//--

	// Startup phases:

	if (ofGetFrameNum() >= (int)DEFAULT_STARTUP_WAITING_TIME) {
		if (!bLoadedStartupDone) {
			ofLogNotice(__FUNCTION__) << "DEFAULT_STARTUP_WAITING_TIME Done!" << endl;

			startup(); // fix

			timerStartupDone = ofGetFrameNum();
		}
	}

	if (!bFoundSendersDone && bLoadedStartupDone) {
		if (nsenders > 0) {
			bFoundSendersDone = true;

			// reload settings again when senders are found!
			loadSettings();
		}
	}

	if (bLoadedStartupDone && bFoundSendersDone) {
		if ((ofGetFrameNum() - timerStartupDone) == (int)DEFAULT_STARTUP_WAITING_TIME) {
			startupDelayed();
		}
	}

	if (bStartupDelayedDone) {
		bStartupDelayedDone = false;
		doScan();
	}
}

//--------------------------------------------------------------
void NDI_input::draw() {
	if (!bGui) return;
	if (!bEnable) return;
	if (!bDraw) return;
	if (!bGui_Preview) return;
	if (!bLoadedStartupDone) return;

	//-

	draw_Main();

	if (bDebug) {
		// Show what it is receiving
		draw_InfoDevices();
	}
}

//--------------------------------------------------------------
void NDI_input::draw_InfoDevices() {

	//--

	// NDI IN

	if (bEnable.get() && bDraw.get()) {
		string str = "";

		nsenders = NDI_Receiver.GetSenderCount();
		if (nsenders > 0) {
			if (NDI_Receiver.ReceiverCreated()) {
				if (NDI_Receiver.ReceiverConnected()) {
					// Show received sender information and received fps
					str += ofToString(NDI_Receiver.GetSenderName().c_str());
					str += " " + ofToString(indexDevice.get()) + "\n";
					str += ofToString(NDI_Receiver.GetSenderWidth());
					str += "x" + ofToString(NDI_Receiver.GetSenderHeight()) + "px | ";
					str += "FPS " + ofToString(NDI_Receiver.GetSenderFps()) + " / ";
					str += ofToString(NDI_Receiver.GetFps()) + "";
				} else {
					// Nothing received
					str += "" + ofToString(NDI_Receiver.GetSenderName().c_str());
				}
			}
		} else {
			str += "\n";
			str += "CONNECTING...";
		}

		str += "\n\n";

		// NDI input devices list

		if (NDI_INPUT_Names_Devices.size() > 0) {
			str += NDI_INPUT_Names_Devices;
		}

		//--

		glm::vec2 p;

		// Top info
		//int px = 2;

		if (!bDrawMini.get()) {
			p = glm::vec2(20 + _padx, 40 + _pady);
		} else {
			if (bLabelsInner) {
				int h = pad / 2 + font.getSize() + padLabel;
				p = rect_NDI_IN.getRectangle().getTopLeft() + glm::vec2(_padx, h);
			} else {
				p = rect_NDI_IN.getRectangle().getTopLeft() + glm::vec2(_padx, _pady);
			}
		}

		ofxSurfingHelpers::drawTextBoxed(font, "NDI IN | " + name, p.x, p.y, 255, 0, false, 128, pad, round, -1, true);

		//--

		// Devices

		if (bDrawMini.get()) {
			p = rect_NDI_IN.getRectangle().getBottomLeft() + glm::vec2(_padx2, _pady2);
		} else {
			auto shape = ofxSurfingHelpers::getShapeBBtextBoxed(font, str);
			p = glm::vec2(ofGetWidth() - (shape.x), ofGetHeight() - (shape.y));
		}

		if (bLabelsInner) {
			int h = ofxSurfingHelpers::getHeightBBtextBoxed(font, str);
			//int h = pad / 2 + font.getSize() + padLabel;
			p = p - glm::vec2(0, h - 33);
		} else {
		}
		ofxSurfingHelpers::drawTextBoxed(font, str, p.x, p.y, 255, 0, false, 128, 20, round, -1, true);
	}
}

//--

//--------------------------------------------------------------
void NDI_input::draw_MiniPreview() {
	if (!bEnable.get()) return;
	if (!bDraw) return;
	if (!bDrawMini) return;

	// Receive ofTexture
	NDI_Receiver.ReceiveImage(tex_NDI_Receiver); //read to texture

	ofPushStyle();
	ofSetColor(255, 255);

	if (bLockAspect.get()) {
		float _ratio = tex_NDI_Receiver.getHeight() / tex_NDI_Receiver.getWidth();
		rect_NDI_IN.setHeight(rect_NDI_IN.getWidth() * _ratio);
	}

	ofRectangle rSrc(0, 0, tex_NDI_Receiver.getWidth(), tex_NDI_Receiver.getHeight());
	ofRectangle rTar(rect_NDI_IN.getX(), rect_NDI_IN.getY(), rect_NDI_IN.getWidth(), rect_NDI_IN.getHeight());
	rSrc.scaleTo(rTar, scaleMode);

	// Image
	//tex_NDI_Receiver.draw(rect_NDI_IN.getX(), rect_NDI_IN.getY(), rect_NDI_IN.getWidth(), rect_NDI_IN.getHeight());

	tex_NDI_Receiver.draw(rSrc.getX(), rSrc.getY(), rSrc.getWidth(), rSrc.getHeight());
	//tex_NDI_Receiver.draw(rTar.getX(), rTar.getY(), rTar.getWidth(), rTar.getHeight());

	// Rect container
	// (overlay changes when editing)
	rect_NDI_IN.draw();

	//// Top info: Name
	//if (bDebug)
	//{
	//	glm::vec2 p = rect_NDI_IN.getRectangle().getTopLeft() + glm::vec2(_padx, _pady);
	//	ofxSurfingHelpers::drawTextBoxed(font, "NDI IN | " + name, p.x, p.y, 255, 0, false, 128, pad,round, -1, true);
	//}

	ofPopStyle();
}

//--------------------------------------------------------------
void NDI_input::draw_FullScreen() {
	if (!bEnable.get()) return;
	if (!bDraw) return;
	//if (!bDrawMini) return;

	// Receive ofTexture
	NDI_Receiver.ReceiveImage(tex_NDI_Receiver); //read to texture

	ofPushStyle();
	ofSetColor(255, 255);

	ofRectangle r(0, 0, tex_NDI_Receiver.getWidth(), tex_NDI_Receiver.getHeight());
	r.scaleTo(ofGetWindowRect(), scaleMode);

	tex_NDI_Receiver.draw(r.x, r.y, r.width, r.height);

	ofPopStyle();
}

//--------------------------------------------------------------
void NDI_input::draw_Main() {
	if (bDrawMini.get())
		draw_MiniPreview();
	else
		draw_FullScreen();
}

//--------------------------------------------------------------
void NDI_input::doListDevices() //list devices
{
	ofLogNotice(__FUNCTION__);

	#ifdef _WIN64
	ofLogNotice(__FUNCTION__) << "\n ofxNDI example receiver - 64 bit";
	#else //_WIN64
	ofLogNotice(__FUNCTION__) << "\n ofxNDI example receiver - 32 bit";
	#endif //_WIN64
	ofLogNotice(__FUNCTION__) << NDI_Receiver.GetNDIversion() << " (http://ndi.tv/)";
	ofLogNotice(__FUNCTION__) << "Press 'SPACE' to list NDI senders";

	//--

	nsenders = NDI_Receiver.GetSenderCount();
	indexDevice.setMax(nsenders - 1);

	//--

	// List all the senders

	namesDevices.clear();

	if (nsenders > 0) {
		ofLogNotice(__FUNCTION__) << "Number of NDI senders found: " << nsenders;

		NDI_INPUT_Names_Devices = "";

		for (int i = 0; i < nsenders; i++) {
			string str = ofToString(i) + " " + NDI_Receiver.GetSenderName(i);
			ofLogNotice(__FUNCTION__) << str;

			namesDevices.push_back(NDI_Receiver.GetSenderName(i));

			NDI_INPUT_Names_Devices += str;
			NDI_INPUT_Names_Devices += "\n";
		}
	} else
		ofLogError(__FUNCTION__) << "No NDI senders found";
}

//--------------------------------------------------------------
void NDI_input::loadSettings() {
	ofLogNotice(__FUNCTION__);

	// load file settings
	ofxSurfingHelpers::loadGroup(params, path_GLOBAL + path_Params_AppSettings);

	//--

	// Reset preview rectangles positions and sizes
	//doReset_Mini_PreviewsSize();

	// Load Settings
	//rect_NDI_IN.loadSettings(path_rect_NDI_IN, path_GLOBAL, false);

	gui_Control.setPosition(position_Gui.get().x, position_Gui.get().y);
}

//--------------------------------------------------------------
void NDI_input::saveSettings() {
	ofLogNotice(__FUNCTION__);

	// get gui position before save

	position_Gui = glm::vec2(gui_Control.getPosition());

	ofxSurfingHelpers::CheckFolder(path_GLOBAL);
	ofxSurfingHelpers::saveGroup(params, path_GLOBAL + path_Params_AppSettings);

	//--

	//rect_NDI_IN.saveSettings(path_rect_NDI_IN, path_GLOBAL, false);
}

//--

//--------------------------------------------------------------
void NDI_input::doNext() {
	ofLogNotice(__FUNCTION__);

	int i = indexDevice;
	i++;
	if (i == indexDevice.getMax() + 1) i = 0;
	indexDevice = i;
}

//--------------------------------------------------------------
void NDI_input::doScan() {
	ofLogNotice(__FUNCTION__);

	doListDevices();
}

//--------------------------------------------------------------
void NDI_input::exit() {
	ofLogNotice(__FUNCTION__);

	saveSettings();
}

//--------------------------------------------------------------
void NDI_input::doReset_Mini_PreviewsSize() {

	rect_NDI_IN.reset(false, wPreview);
}

//--------------------------------------------------------------
void NDI_input::Changed(ofAbstractParameter & e) {
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

	//--

	// Device
	// NDI IN

	else if (name == bEnable.getName() && bEnable.get()) {
	#ifdef DEVICES_BY_NAME_INSTEAD_OF_BY_INDEX
		setup_ByName(nameDevice);
	#else
		setup_ByIndex(indexDevice);
	#endif
		//--

		////TODO:
		////close device
		//if (!bEnable) {
		//	NDI_Receiver.
		//}
	}

	//--

	#ifdef DEVICES_BY_NAME_INSTEAD_OF_BY_INDEX
	else if (name == nameDevice.getName()) {
		setup_ByName(nameDevice);
	}
	#endif

	//--

	else if (name == indexDevice.getName()) {
		setup_ByIndex(indexDevice);
	}

	//--

	else if (name == scaleMode_Index.getName()) {
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
	}

	//--

	//else if (name == bEdit.getName())
	//{
	//	if (bEdit.get())
	//	{
	//		rect_NDI_IN.enableEdit();
	//		//--
	//		//if (bEnable.get())
	//		//{
	//		//	doRefresh_NDI_IN();
	//		//}
	//	}
	//	else
	//	{
	//		rect_NDI_IN.disableEdit();
	//	}
	//}

	//--

	else if (name == bReset.getName() && bReset) {
		bReset = false;

		doReset_Mini_PreviewsSize();
	}

	else if (name == bLoad.getName()) {
		loadSettings();
	}

	//--

	else if (name == bScan.getName() && bScan.get()) {
		bScan = false;

		doScan();
	}

	//--

	else if (name == bNext.getName() && bNext.get()) {
		bNext = false;

		doNext();
	}

	//--

	else if (name == position_Gui.getName()) {
		gui_Control.setPosition(position_Gui.get().x, position_Gui.get().y);
	}
}

//--------------------------------------------------------------
void NDI_input::windowResized(int w, int h) {
	screenW = w;
	screenH = h;

	//TODO:
	//NDI_OUT_Sender.UpdateSender(1920, 1080);//update size
}

#endif