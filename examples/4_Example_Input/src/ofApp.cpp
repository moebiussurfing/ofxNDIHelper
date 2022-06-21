#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	receiver.setPathGlobal("myReceiverSettings/"); // -> Optionally can customize the settings path.
	receiver.setup("myReceiver Phone"); // -> Optionally we can customize the name to use more than one input objects.
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
	if (key == 'd') receiver.setToggleDebug();
	if (key == 'm') receiver.setToggleMini();
}