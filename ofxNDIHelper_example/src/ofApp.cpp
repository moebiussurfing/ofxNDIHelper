#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
//#ifdef USE_WEBCAM
//	int _d = 0;
//	vidGrabber.setDeviceID(_d); // The first webcam
//	vidGrabber.setup(640, 480); // try to grab at this size.
//#endif

	//ndi
#ifdef USE_ofxNDI
	NDIHelper.setup();
#endif
}

//--------------------------------------------------------------
void ofApp::update() {

//#ifdef USE_WEBCAM
//	vidGrabber.update();
//#endif
#ifdef USE_ofxNDI
	NDIHelper.update();
#endif

}

//--------------------------------------------------------------
void ofApp::draw() {

	//ndi
#ifdef USE_ofxNDI
	NDIHelper.beginNDI_OUT();
	
	//if (SHOW_Webcam.get()) 
	{
		NDIHelper.drawWebcam();
	}

	NDIHelper.endNDI_OUT();
#endif

	//-

	//ndi
#ifdef USE_ofxNDI
	//NDIHelper.draw();

	NDIHelper.drawGui();
#endif
}

//#ifdef USE_WEBCAM
//#undef USE_WEBCAM
//#endif
//
//#ifdef USE_WEBCAM
////--------------------------------------------------------------
//void ofApp::drawCamera() {
//	//video
//	ofSetColor(255, 255, 255, 255);
//	ofRectangle videoRect(0, 0, vidGrabber.getWidth(), vidGrabber.getHeight());
//	videoRect.scaleTo(ofGetWindowRect());
//	vidGrabber.draw(videoRect.x, videoRect.y, videoRect.width, videoRect.height);
//}
//#endif

//--------------------------------------------------------------
void ofApp::exit()
{
	//ndi
#ifdef USE_ofxNDI
	NDIHelper.exit();
#endif
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	//ndi
#ifdef USE_ofxNDI
	NDIHelper.windowResized(w, h);
#endif
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
