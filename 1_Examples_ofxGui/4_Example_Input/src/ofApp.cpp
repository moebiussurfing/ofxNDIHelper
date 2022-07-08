#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	// Optional: customize settings path and name
	//receiver.setPathGlobal("ofApp_NDI/"); // -> Optionally can customize the settings path.

	receiver.setup();
	
	// -> Optionally we can customize the name to use more than one input objects,
	// // and avoid to collide same settings filenames.
	//receiver.setup("myReceiver Phone"); 
}

//--------------------------------------------------------------
void ofApp::draw()
{
	// Draw Previews
	receiver.draw();

	//----

	// Gui Controls
	receiver.drawGui();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	//if (key == 'd') receiver.setToggleDebug();
	//if (key == 'm') receiver.setToggleFullScreen();
}