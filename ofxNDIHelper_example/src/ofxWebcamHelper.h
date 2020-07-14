//#pragma once
//
//#include "ofMain.h"
//
//
//class ofApp : public ofBaseApp {
//
//public:
//
//	ofVideoGrabber vidGrabber;
//	void setupWebcam();//setup webcam from name device nor index
//	void setupWebcam(int index);//setup webcam from device index
//	void restartWebcam();//restart camera using the current index camera
//	void exitWebcam();//store camera device name to xml
//	void drawWebcam();
//	void drawWebcamInfo(int x, int y);
//	ofParameter <std::string> _dName{ "WEBCAM_DEVICE_NAME", "" };
//	//int _d;
//	ofParameter<bool> mini_Webcam;
//	ofParameter<int> index_WebcamDevice;
//	ofParameter<string> name_Webcam;
//	string path_WebcamSettings;
//
//	//--
//
//	ofTrueTypeFont font;
//
//	//--------------------------------------------------------------
//	void drawTextBoxed(string text, int x, int y)
//	{
//		ofPushStyle();
//		int _alpha = 200;//bbox
//		int pad = 20;
//
//		if (!font.isLoaded()) {
//			ofDrawBitmapStringHighlight(text, x, y);
//		}
//		else {
//			//bbox
//			ofSetColor(0, _alpha);
//			ofFill();
//			ofRectangle _r(font.getStringBoundingBox(text, x, y));
//			_r.setWidth(_r.getWidth() + pad);
//			_r.setHeight(_r.getHeight() + pad);
//			_r.setX(_r.getPosition().x - pad / 2.);
//			_r.setY(_r.getPosition().y - pad / 2.);
//			ofDrawRectangle(_r);
//
//			//text
//			ofSetColor(255, 255);
//			ofNoFill();
//			font.drawString(text, x, y);
//		}
//
//		ofPopStyle();
//	}
//
//private:
//	//--------------------------------------------------------------
//	void CheckFolder(string _path)
//	{
//		ofLogNotice(__FUNCTION__) << _path;
//
//		ofDirectory dataDirectory(ofToDataPath(_path, true));
//
//		//check if folder path exist
//		if (!dataDirectory.isDirectory())
//		{
//			ofLogError(__FUNCTION__) << "FOLDER NOT FOUND! TRYING TO CREATE...";
//
//			//try to create folder
//			bool b = dataDirectory.createDirectory(ofToDataPath(_path, true));
//
//			//debug if creation has been succeded
//			if (b) ofLogNotice(__FUNCTION__) << "CREATED '" << _path << "'  SUCCESSFULLY!";
//			else ofLogError(__FUNCTION__) << "UNABLE TO CREATE '" << _path << "' FOLDER!";
//		}
//		else
//		{
//			ofLogNotice(__FUNCTION__) << "OK! LOCATED FOLDER: '" << _path << "'";//nothing to do
//		}
//	}
//};
