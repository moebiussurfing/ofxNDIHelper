#include "NDI_input.h"

#ifdef USE_ofxNDI_IN

//--------------------------------------------------------------
NDI_input::NDI_input()
{
}

//--------------------------------------------------------------
NDI_input::~NDI_input()
{
	ofRemoveListener(params.parameterChangedE(), this, &NDI_input::Changed);

	exit();
}

//--------------------------------------------------------------
void NDI_input::setup(string _name)
{
	name = _name;
	path_rect_NDI_IN = "NDI_IN_" + name + "_Mini";
	rect_NDI_IN.setName(path_rect_NDI_IN);
	rect_NDI_IN.setPathGlobal(path_GLOBAL);
	rect_NDI_IN.setMode(ofxSurfingBoxInteractive::FREE_LAYOUT);
	rect_NDI_IN.setup();

	path_Params_AppSettings = "NDI_IN_" + name + ".json";

	string _str = "telegrama_render.otf";
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

	setup_Params();

	//--

	setup_NDI_IN();

	//--

	bDISABLECALLBACKS = false;

	loadSettings();
}

//--

//--------------------------------------------------------------
void NDI_input::doScan()
{
	bScan = true;
}

//--------------------------------------------------------------
void NDI_input::startup()
{

#ifdef DEVICES_BY_NAME_INSTEAD_OF_BY_INDEX
	nameDevice = nameDevice;
#else
	indexDevice = indexDevice;
#endif

	doScan();
}

//--------------------------------------------------------------
void NDI_input::setup_Params()
{
	bGui_Preview.set("NDI PREVIEW " + name, true);
	bGui_Internal.set("NDI CONTROLS " + name, true);

	bLockRatio.set("LOCK ASPECT RATIO", true);
	bDebug.set("DEBUG", true);
	position_Gui.set("GUI POSITION",
		glm::vec2(screenW * 0.5, screenH * 0.5),
		glm::vec2(0, 0),
		glm::vec2(screenW, screenH)
	);

	//bEdit.set("LAYOUT EDIT", true);

	bReset.set("LAYOUT RESET", false);
	bReset.setSerializable(false);

#ifdef DEVICES_BY_NAME_INSTEAD_OF_BY_INDEX
	indexDevice.setSerializable(false);
#else
	nameDevice.setSerializable(false);
#endif


	//--

	// NDI IN

	bEnable.set("ENABLE", false);
	bDraw.set("DRAW", true);
	indexDevice.set("INDEX", 0, 0, 1);
	nameDevice.set("NAME", "ofxNDIHelperIN");
	bDrawMini.set("MINI", true);
	bScan.set("SCAN!", false);
	bScan.setSerializable(false);

	params_NDI_Input.add(bEnable);
	params_NDI_Input.add(bScan);
	params_NDI_Input.add(bDraw);
	params_NDI_Input.add(bDrawMini);
	params_NDI_Input.add(indexDevice);
	params_NDI_Input.add(nameDevice);

	//--

	// Input

	params_Control.setName("CONTROL");
	//params_Control.add(bEdit);
	params_Control.add(bReset);
	params_Control.add(bLockRatio);
	//params_Control.add(bDebug);//TODO: BUG: not linking when makeReference from parent scope..
	params_Control.add(position_Gui);

	params.setName("NDI INPUT");
	params.add(params_Control);
	params.add(params_NDI_Input);

	gui_Control.setup("NDI INPUT | " + name);
	gui_Control.add(params);

	auto& gc = gui_Control.getGroup(params.getName()).getGroup(params_Control.getName());
	auto& gp = gc.getGroup(position_Gui.getName());
	gc.minimize();
	gp.minimize();

	//--

	// Callback

	ofAddListener(params.parameterChangedE(), this, &NDI_input::Changed);
}

//--

//--------------------------------------------------------------
void NDI_input::setup_NDI_IN_ByIndex(int deviceIndex)
{
	if (indexDevice != deviceIndex) indexDevice = deviceIndex;

	setup_NDI_IN();
}

//--------------------------------------------------------------
void NDI_input::setup_NDI_IN_ByName(string deviceName)
{
	// Search the device index by name

	if (nameDevice.get() != deviceName) nameDevice = deviceName;

	int _indexTarget = -1;

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
	// receiver dimensions and fps are not known yet
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
	ndiReceiveChars = new unsigned char[receiverWidth * receiverHeight * 4];

	//indexDevice.setMax(ndiReceiver.GetSenderCount() - 1);

	//doRefresh_NDI_IN();
	{
		ofLogNotice(__FUNCTION__);

		//char name[256];
		int nsenders = ndiReceiver.GetSenderCount();
		indexDevice.setMax(nsenders - 1);

		// List all the senders
		if (nsenders > 0)
		{
			ofLogNotice(__FUNCTION__) << "Number of NDI senders found: " << nsenders;

			NDI_INPUT_Names_Devices = "";
			for (int i = 0; i < nsenders; i++)
			{
				string name = ndiReceiver.GetSenderName(i);

				if (nameDevice.get() == name) {
					_indexTarget = i;
					indexDevice = i;
				}

				string str = ofToString(i) + " " + name;
				ofLogNotice(__FUNCTION__) << str;

				NDI_INPUT_Names_Devices += str;
				NDI_INPUT_Names_Devices += "\n";

				//if (i != nsenders - 1) NDI_INPUT_Names_Devices += "\n";
			}

			//if (nsenders > 1)
			//	ofLogNotice(__FUNCTION__) << "Press key [0] to [" << nsenders - 1 << "] to select a sender";
		}
		else ofLogNotice(__FUNCTION__) << "No NDI senders found";

		if (_indexTarget == -1)
		{
			ofLogError(__FUNCTION__) << "Device name not found on NDI senders list!";
			return;
		}
	}

	//-
	// 
	//{
	//	int nsenders = ndiReceiver.GetSenderCount();
	//	indexDevice.setMax(nsenders - 1);
	//	int index = indexDevice.get();
	//	char name[256];

	//	if (nsenders > 0 && index >= 0 && index < nsenders)
	//	{
	//		// Update the receiver with the returned index
	//		// Returns false if the current sender is selected
	//		if (ndiReceiver.SetSenderIndex(index))
	//		{
	//			ofLogNotice(__FUNCTION__) << "Selected [" << ndiReceiver.GetSenderName(index) << "]";
	//			ndiReceiver.GetSenderName(name, 256, index);
	//			ofLogNotice(__FUNCTION__) << "    Sender " << index << " [" << name << "]";
	//			//std::string my_string(name);
	//			//nameDevice = my_string;
	//			nameDevice = ndiReceiver.GetSenderName(index);//TODO: not required
	//			ofLogNotice(__FUNCTION__) << "nameDevice: " << nameDevice.get();
	//		}
	//		else ofLogNotice(__FUNCTION__) << "Stay in same sender";
	//	}
	//	else
	//	{
	//		ofLogError(__FUNCTION__) << "NOT ANY NDI SENDERS!";
	//	}
	//}
}

//--------------------------------------------------------------
void NDI_input::setup_NDI_IN() {

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
	// receiver dimensions and fps are not known yet
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
	ndiReceiveChars = new unsigned char[receiverWidth * receiverHeight * 4];

	//indexDevice.setMax(ndiReceiver.GetSenderCount() - 1);

	doRefresh_NDI_IN();

	int nsenders = ndiReceiver.GetSenderCount();
	indexDevice.setMax(nsenders - 1);
	int index = indexDevice.get();
	char name[256];

	if (nsenders > 0 && index >= 0 && index < nsenders)
	{
		// Update the receiver with the returned index
		// Returns false if the current sender is selected
		if (ndiReceiver.SetSenderIndex(index))
		{
			ofLogNotice(__FUNCTION__) << "Selected [" << ndiReceiver.GetSenderName(index) << "]";
			ndiReceiver.GetSenderName(name, 256, index);
			ofLogNotice(__FUNCTION__) << "    Sender " << index << " [" << name << "]";
			//std::string my_string(name);
			//nameDevice = my_string;
			nameDevice = ndiReceiver.GetSenderName(index);
			ofLogNotice(__FUNCTION__) << "nameDevice: " << nameDevice.get();
		}
		else ofLogNotice(__FUNCTION__) << "Stay in same sender";
	}
	else
	{
		ofLogError(__FUNCTION__) << "NOT ANY NDI SENDERS!";
	}
}

//--------------------------------------------------------------
void NDI_input::drawGui()
{
	if (!bGui_Preview) return;
	if (!bGui_Internal) return;

	gui_Control.draw();
}

//--------------------------------------------------------------
void NDI_input::draw() {
	if (ofGetFrameNum() == 120) startup();//fix

	if (!bGui_Preview) return;

	if (bDebug)
	{
		// Show what it is receiving
		draw_InfoDevices();
	}

	//-

	// NDI IN
	if (bEnable.get() && bDraw.get())
	{
		draw_NDI_IN();
	}
}

//--------------------------------------------------------------
void NDI_input::draw_InfoDevices() {

	//--

	// NDI IN

	if (bEnable.get() && bDraw.get() && bDrawMini.get())
	{
		string str = "";

		int nsenders = ndiReceiver.GetSenderCount();
		if (nsenders > 0)
		{
			if (ndiReceiver.ReceiverCreated())
			{
				if (ndiReceiver.ReceiverConnected())
				{
					// Show received sender information and received fps
					str += ofToString(ndiReceiver.GetSenderName().c_str());
					str += " " + ofToString(indexDevice.get()) + "\n";
					str += ofToString(ndiReceiver.GetSenderWidth());
					str += "x" + ofToString(ndiReceiver.GetSenderHeight()) + "px | ";
					str += "FPS " + ofToString(ndiReceiver.GetSenderFps()) + " / ";
					str += ofToString(ndiReceiver.GetFps()) + "";
				}
				else 
				{
					// Nothing received
					str += "> " + ofToString(ndiReceiver.GetSenderName().c_str());
				}
			}

			//str += "\n\n";
			//if (nsenders == 1) {
			//	str += "1 SOURCE";
			//}
			//else {
			//	str += ofToString(nsenders) + " SOURCES \n";
			//	//str += "Space key > List sources";
			//}
		}
		else
		{
			str += "\n";
			str += "Connecting...";
		}

		str += "\n\n";

		// NDI input devices list
		if (NDI_INPUT_Names_Devices.size() > 0)
		{
			//str += "DEVICES \n\n";
			str += NDI_INPUT_Names_Devices;
		}

		//--

		glm::vec2 p = rect_NDI_IN.getRectangle().getBottomLeft() + glm::vec2(_padx2, _pady2);
		ofxSurfingHelpers::drawTextBoxed(font, str, p.x, p.y, 255, 0, false, 128, 20, 3, -1, true);
	}
}

//--

//--------------------------------------------------------------
void NDI_input::draw_NDI_IN_MiniPreview(bool bInfo)
{
	if (!bEnable.get()) return;

	// Receive ofTexture
	ndiReceiver.ReceiveImage(ndiReceiveTexture);//read to texture

	ofPushStyle();
	ofSetColor(255, 255);

	if (bLockRatio.get()) {
		float _ratio = ndiReceiveTexture.getHeight() / ndiReceiveTexture.getWidth();
		rect_NDI_IN.setHeight(rect_NDI_IN.getWidth() * _ratio);
	}
	ndiReceiveTexture.draw(rect_NDI_IN.getX(), rect_NDI_IN.getY(), rect_NDI_IN.getWidth(), rect_NDI_IN.getHeight());
	rect_NDI_IN.draw();

	// bb border
	ofNoFill();
	ofSetColor(0);
	ofSetLineWidth(2.0);
	ofDrawRectangle(rect_NDI_IN.getRectangle());

	// Top info: Name
	if (bDebug && bInfo)
	{
		glm::vec2 p = rect_NDI_IN.getRectangle().getTopLeft() + glm::vec2(_padx, _pady);
		ofxSurfingHelpers::drawTextBoxed(font, "NDI IN | " + name, p.x, p.y, 255, 0, false, 128, pad, 3, -1, true);
	}

	ofPopStyle();
}

//--------------------------------------------------------------
void NDI_input::draw_NDI_IN_Full()
{
	if (!bEnable.get()) return;

	// Receive ofTexture
	ndiReceiver.ReceiveImage(ndiReceiveTexture);//read to texture

	ofPushStyle();
	ofSetColor(255, 255);

	ofRectangle r(0, 0, ndiReceiveTexture.getWidth(), ndiReceiveTexture.getHeight());

	r.scaleTo(ofGetWindowRect());
	//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_CENTER);
	//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_STRETCH_TO_FILL);
	//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_FILL);

	ndiReceiveTexture.draw(r.x, r.y, r.width, r.height);

	ofPopStyle();

	//--

	// Top info
	if (bDebug)
	{
		auto p = glm::vec2(20 + _padx, 40 + _pady);
		ofxSurfingHelpers::drawTextBoxed(font, "NDI IN | " + name, p.x, p.y, 255, 0, false, 128, pad, 3, -1, true);
	}
}

//--------------------------------------------------------------
void NDI_input::draw_NDI_IN()
{
	if (bDrawMini.get()) draw_NDI_IN_MiniPreview(true);
	else draw_NDI_IN_Full();
}

//--------------------------------------------------------------
void NDI_input::doRefresh_NDI_IN() {
	ofLogNotice(__FUNCTION__);

	//char name[256];
	int nsenders = ndiReceiver.GetSenderCount();
	indexDevice.setMax(nsenders - 1);

	// List all the senders
	if (nsenders > 0)
	{
		ofLogNotice(__FUNCTION__) << "Number of NDI senders found: " << nsenders;

		NDI_INPUT_Names_Devices = "";
		for (int i = 0; i < nsenders; i++) 
		{
			string name = ndiReceiver.GetSenderName(i);

			string str = ofToString(i) + " " + name;
			ofLogNotice(__FUNCTION__) << str;
			
			NDI_INPUT_Names_Devices += str;
			NDI_INPUT_Names_Devices += "\n";

			//if (i != nsenders - 1) NDI_INPUT_Names_Devices += "\n";
		}

		//if (nsenders > 1)
		//	ofLogNotice(__FUNCTION__) << "Press key [0] to [" << nsenders - 1 << "] to select a sender";
	}
	else ofLogNotice(__FUNCTION__) << "No NDI senders found";
}

//--------------------------------------------------------------
void NDI_input::loadSettings()
{
	ofLogNotice(__FUNCTION__);

	// load file settings
	ofxSurfingHelpers::loadGroup(params, path_GLOBAL + path_Params_AppSettings);

	//---

	doRefresh_NDI_IN();

	//--

	// Reset preview rectangles positions and sizes
	//doReset_Mini_Previews();

	// Load Settings
	//rect_NDI_IN.loadSettings(path_rect_NDI_IN, path_GLOBAL, false);

	gui_Control.setPosition(position_Gui.get().x, position_Gui.get().y);

	//--

	//bEdit = bEdit;
	indexDevice = indexDevice;
}

//--------------------------------------------------------------
void NDI_input::exit()
{
	ofLogNotice(__FUNCTION__);

	saveSettings();
}

//--------------------------------------------------------------
void NDI_input::saveSettings()
{
	ofLogNotice(__FUNCTION__);

	// get gui position before save

	position_Gui = glm::vec2(gui_Control.getPosition());

	ofxSurfingHelpers::CheckFolder(path_GLOBAL);
	ofxSurfingHelpers::saveGroup(params, path_GLOBAL + path_Params_AppSettings);

	//--

	//rect_NDI_IN.saveSettings(path_rect_NDI_IN, path_GLOBAL, false);
}

//--------------------------------------------------------------
void NDI_input::doReset_Mini_Previews() {

	//float _pad = 200;
	//float _xx = xPadPreview;
	//float _yy = yPadPreview;

	//float _ratio = ndiReceiveTexture.getHeight() / ndiReceiveTexture.getWidth();
	//float _pad = 10;
	//glm::vec2 p = glm::vec2(gui_Control.getShape().getTopRight());
	//float _xx = p.x + _pad;
	//float _yy = p.y + 28;

	////rect_NDI_IN.
	////rect_NDI_IN.width = wPreview;
	////rect_NDI_IN.height = rect_NDI_IN.width * _ratio;
	////rect_NDI_IN.x = _xx;
	////rect_NDI_IN.y = _yy;

	//rect_NDI_IN.setShape(ofRectangle(_xx, _yy, wPreview, wPreview * _ratio));
	rect_NDI_IN.reset();
}

//--------------------------------------------------------------
void NDI_input::Changed(ofAbstractParameter& e)
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

		else if (name == bReset.getName() && bReset)
		{
			bReset = false;

			doReset_Mini_Previews();
		}

		else if (name == bScan.getName() && bScan.get())
		{
			bScan = false;

			doRefresh_NDI_IN();
		}

		//----

		// NDI IN 

		else if (name == bEnable.getName() && bEnable.get())
		{
#ifdef DEVICES_BY_NAME_INSTEAD_OF_BY_INDEX
			setup_NDI_IN_ByName(nameDevice);
#else
			setup_NDI_IN(indexDevice);
#endif
		}

		else if (name == indexDevice.getName() && bEnable.get())
		{
			setup_NDI_IN_ByIndex(indexDevice);
		}

#ifdef DEVICES_BY_NAME_INSTEAD_OF_BY_INDEX
		else if (name == nameDevice.getName() && bEnable.get())
		{
			setup_NDI_IN_ByName(nameDevice);
		}
#endif
		//----

		else if (name == position_Gui.getName())
		{
			gui_Control.setPosition(position_Gui.get().x, position_Gui.get().y);
		}
	}
}

//--------------------------------------------------------------
void NDI_input::windowResized(int w, int h)
{
	screenW = w;
	screenH = h;

	//TODO:
	//ndiSender.UpdateSender(1920, 1080);//update size
}

#endif