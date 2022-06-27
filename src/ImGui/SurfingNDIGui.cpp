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

	guiManager.setName("NDIGui");
	guiManager.setWindowsMode(IM_GUI_MODE_WINDOWS_SPECIAL_ORGANIZER);
	guiManager.setup();
}

//--------------------------------------------------------------
void SurfingNDIGui::startup()
{
	ofLogNotice(__FUNCTION__);

	guiManager.startup();
}

//--------------------------------------------------------------
void SurfingNDIGui::draw()
{
	//ofLogNotice(__FUNCTION__);

	//guiManager.begin();
	//{
	//	//IMGUI_SUGAR__WINDOWS_CONSTRAINTSW;
	//	//if (guiManager.beginWindowSpecial("ofApp"))
	//	//{
	//	//	guiManager.endWindowSpecial();
	//	//}
	//}
	//guiManager.end();
}