#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
#ifdef USE_ofxNDI
	NDIHelper.setup();
#endif

#ifdef USE_ofxWindowApp
	windowApp.setFrameRate(60);
	windowApp.setVerticalSync(true);
#endif
}

//--------------------------------------------------------------
void ofApp::update() {

#ifdef USE_ofxNDI
	NDIHelper.update();
#endif

}

//--------------------------------------------------------------
void ofApp::draw() {
#ifdef USE_ofxNDI
	NDIHelper.begin_NDI_OUT();
	{
		//webcam
		NDIHelper.drawWebcam();

		//ndi input
		NDIHelper.draw_NDI_IN();
	}
	NDIHelper.end_NDI_OUT();

	//-
	
	////webcam
	//NDIHelper.drawWebcam();

	//draw ndi out
	NDIHelper.draw_NDI_OUT();
	
	//-

	//gui
	NDIHelper.drawGui();
#endif
}

//--------------------------------------------------------------
void ofApp::exit()
{
#ifdef USE_ofxNDI
	NDIHelper.exit();
#endif

#ifdef USE_ofxWindowApp
	windowApp.exit();
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
