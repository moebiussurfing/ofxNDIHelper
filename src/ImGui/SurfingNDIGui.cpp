#include "SurfingNDIGui.h"

//--------------------------------------------------------------
SurfingNDIGui::SurfingNDIGui()
{
	ofLogNotice(__FUNCTION__);

	//setup();
}

//--------------------------------------------------------------
SurfingNDIGui::~SurfingNDIGui()
{
	ofLogNotice(__FUNCTION__);

}

//--------------------------------------------------------------
void SurfingNDIGui::setup()
{
	ofLogNotice(__FUNCTION__);

	ui.setName("NDIGui");
	ui.setWindowsMode(IM_GUI_MODE_WINDOWS_SPECIAL_ORGANIZER);
	ui.setup();
}

//--------------------------------------------------------------
void SurfingNDIGui::startup()
{
	ofLogNotice(__FUNCTION__);

	ui.startup();
}

//--------------------------------------------------------------
void SurfingNDIGui::draw()
{
	//ofLogNotice(__FUNCTION__);

	//ui.begin();
	//{
	//	//IMGUI_SUGAR__WINDOWS_CONSTRAINTSW;
	//	//if (ui.beginWindowSpecial("ofApp"))
	//	//{
	//	//	ui.endWindowSpecial();
	//	//}
	//}
	//ui.end();
}