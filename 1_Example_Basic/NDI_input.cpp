#include "NDI_input.h"

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
	path_Params_AppSettings = "NDI_IN_" + name + ".json";

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

	setup_Params();

	//--

	setup_NDI_IN();

	//--

	bDISABLECALLBACKS = false;

	loadSettings();
}

//--

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

	bEdit.set("EDIT LAYOUT", true);
	bReset.set("RESET LAYOUT", false);

	bReset.setSerializable(false);

	//--

	// NDI IN

	bNDI_Input.set("IN ENABLE", false);
	bNDI_Input_Draw.set("IN DRAW", true);
	NDI_Input_Index.set("IN DEVICE", 0, 0, 1);
	NDI_Input_Name.set("IN", "ofxNDIHelperIN");
	bNDI_Input_Mini.set("IN MINI", true);
	bNDI_Input_Scan.set("SCAN", false);
	bNDI_Input_Scan.setSerializable(false);

	params_NDI_Input.add(bNDI_Input);
	params_NDI_Input.add(bNDI_Input_Draw);
	params_NDI_Input.add(bNDI_Input_Mini);
	params_NDI_Input.add(NDI_Input_Index);
	params_NDI_Input.add(NDI_Input_Name);
	params_NDI_Input.add(bNDI_Input_Scan);

	//--

	// Input

	params_Internal.setName("CONTROL");
	params_Internal.add(bEdit);
	params_Internal.add(bLockRatio);
	params_Internal.add(bDebug);
	params_Internal.add(bReset);
	params_Internal.add(position_Gui);

	params.setName("NDI INPUT");
	params.add(params_Internal);
	params.add(params_NDI_Input);

	gui_User.setup("NDI INPUT USER" + name);
	gui_User.add(params);

	//-

	// Callback

	ofAddListener(params.parameterChangedE(), this, &NDI_input::Changed);
}

//--

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
	ndiReceiveChars = new unsigned char[receiverWidth * receiverHeight * 4];

	//NDI_Input_Index.setMax(ndiReceiver.GetSenderCount() - 1);

	doRefresh_NDI_IN();

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
			//NDI_Input_Name = my_string;
			NDI_Input_Name = ndiReceiver.GetSenderName(index);
			ofLogNotice(__FUNCTION__) << "NDI_Input_Name: " << NDI_Input_Name.get();
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
void NDI_input::drawGui()
{
	if (!bGui_Preview) return;
	if (!bGui_Internal) return;

	gui_User.draw();
}

//--------------------------------------------------------------
void NDI_input::draw() {
	if (!bGui_Preview) return;

	if (bDebug)
	{
		// Show what it is receiving
		draw_InfoDevices();
	}

	//-

	// NDI IN
	if (bNDI_Input.get() && bNDI_Input_Draw.get())
	{
		draw_NDI_IN();
	}
}

//--------------------------------------------------------------
void NDI_input::draw_InfoDevices() {

	//--

	// NDI IN

	if (bNDI_Input.get() && bNDI_Input_Draw.get() && bNDI_Input_Mini.get())
	{
		string str = "";

		int nsenders = ndiReceiver.GetSenderCount();
		if (nsenders > 0) {
			if (ndiReceiver.ReceiverCreated()) {
				if (ndiReceiver.ReceiverConnected()) {
					// Show received sender information and received fps
					str += ofToString(ndiReceiver.GetSenderName().c_str());
					str += " #" + ofToString(NDI_Input_Index.get()) + "\n";

					str += ofToString(ndiReceiver.GetSenderWidth());
					str += "x" + ofToString(ndiReceiver.GetSenderHeight()) + " px \n";
					str += "FPS " + ofToString(ndiReceiver.GetSenderFps()) + " ";
					str += ofToString(ndiReceiver.GetFps()) + "";
				}
				else {
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
		else {
			str += "\n";
			str += "Connecting...";
		}
		str += "\n\n";

		// NDI input devices list
		if (NDI_INPUT_Names_Devices.size() > 0) {
			str += "DEVICES \n\n";
			str += NDI_INPUT_Names_Devices;
		}

		//--

		auto p = rect_NDI_IN.getBottomLeft() + glm::vec2(_padx2, _pady2);
		ofxSurfingHelpers::drawTextBoxed(font, str, p.x, p.y, 255, 0, false, 128, 20, 3, -1, true);
	}
}

//--

//--------------------------------------------------------------
void NDI_input::draw_NDI_IN_MiniPreview(bool bInfo)
{
	if (!bNDI_Input.get()) return;

	// Receive ofTexture
	ndiReceiver.ReceiveImage(ndiReceiveTexture);//read to texture

	ofPushStyle();
	ofSetColor(255, 255);

	if (bLockRatio.get()) {
		float _ratio = ndiReceiveTexture.getHeight() / ndiReceiveTexture.getWidth();
		rect_NDI_IN.height = rect_NDI_IN.width * _ratio;
	}
	ndiReceiveTexture.draw(rect_NDI_IN.x, rect_NDI_IN.y, rect_NDI_IN.width, rect_NDI_IN.height);
	rect_NDI_IN.draw();

	// bb border
	ofNoFill();
	ofSetColor(0);
	ofSetLineWidth(2.0);
	ofDrawRectangle(rect_NDI_IN);

	// Top info
	if (bDebug && bInfo)
	{
		auto p = rect_NDI_IN.getTopLeft() + glm::vec2(_padx, _pady);
		ofxSurfingHelpers::drawTextBoxed(font, "NDI IN : " + name, p.x, p.y, 255, 0, false, 128, pad, 3, -1, true);
	}

	ofPopStyle();
}

//--------------------------------------------------------------
void NDI_input::draw_NDI_IN_Full()
{
	if (!bNDI_Input.get()) return;

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
		ofxSurfingHelpers::drawTextBoxed(font, "NDI IN : " + name, p.x, p.y, 255, 0, false, 128, pad, 3, -1, true);
	}
}

//--------------------------------------------------------------
void NDI_input::draw_NDI_IN()
{
	if (bNDI_Input_Mini.get()) draw_NDI_IN_MiniPreview(true);
	else draw_NDI_IN_Full();
}

//--------------------------------------------------------------
void NDI_input::doRefresh_NDI_IN() {
	ofLogNotice(__FUNCTION__);

	char name[256];
	int nsenders = ndiReceiver.GetSenderCount();
	NDI_Input_Index.setMax(nsenders - 1);

	// List all the senders
	if (nsenders > 0)
	{
		ofLogNotice(__FUNCTION__) << "Number of NDI senders found: " << nsenders;

		NDI_INPUT_Names_Devices = "";
		for (int i = 0; i < nsenders; i++) {
			string name = ndiReceiver.GetSenderName(i);
			string str;
			str = "#" + ofToString(i) + " " + name;
			ofLogNotice(__FUNCTION__) << str;
			NDI_INPUT_Names_Devices += str;

			if (i != nsenders - 1) NDI_INPUT_Names_Devices += "\n";
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
	doReset_Mini_Previews();

	// Load Settings

	rect_NDI_IN.loadSettings(path_rect_NDI_IN, path_GLOBAL, false);


	gui_User.setPosition(position_Gui.get().x, position_Gui.get().y);

	bEdit = bEdit;
	NDI_Input_Index = NDI_Input_Index;
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

	position_Gui = glm::vec2(gui_User.getPosition());

	ofxSurfingHelpers::CheckFolder(path_GLOBAL);
	ofxSurfingHelpers::saveGroup(params, path_GLOBAL + path_Params_AppSettings);

	//--

	rect_NDI_IN.saveSettings(path_rect_NDI_IN, path_GLOBAL, false);
}

//--------------------------------------------------------------
void NDI_input::doReset_Mini_Previews() {

	float _pad = 200;
	float _xx = xPadPreview;
	float _yy = yPadPreview;
	float _ratio;

	_ratio = ndiReceiveTexture.getHeight() / ndiReceiveTexture.getWidth();
	rect_NDI_IN.width = wPreview;
	rect_NDI_IN.height = rect_NDI_IN.width * _ratio;
	rect_NDI_IN.x = _xx;
	rect_NDI_IN.y = _yy;
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

		else if (name == bEdit.getName())
		{
			if (bEdit.get())
			{
				rect_NDI_IN.enableEdit();

				//--

				//if (bNDI_Input.get()) 
				//{
				//	doRefresh_NDI_IN();
				//}
			}
			else
			{
				rect_NDI_IN.disableEdit();
			}
		}

		else if (name == bReset.getName() && bReset)
		{
			bReset = false;

			doReset_Mini_Previews();
		}

		else if (name == bNDI_Input_Scan.getName() && bNDI_Input_Scan.get())
		{
			bNDI_Input_Scan = false;

			doRefresh_NDI_IN();
		}

		//----

		// NDI IN 

		else if (name == bNDI_Input.getName() && bNDI_Input.get())
		{
			setup_NDI_IN();
		}
		else if (name == NDI_Input_Index.getName() && bNDI_Input.get())
		{
			setup_NDI_IN();
		}

		//----

		else if (name == position_Gui.getName())
		{
			gui_User.setPosition(position_Gui.get().x, position_Gui.get().y);
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