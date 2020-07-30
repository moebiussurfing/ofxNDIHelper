# ofxNDIHelper

## Overview
ofxNDIHelper is a helper addon for openFrameworks to handle NDI input/output and webcam input.

## Screenshot
![Alt text](/Capture.PNG?raw=true "Capture.PNG")

## Features
- List and select channels and webcam devices.
- Store and recall settings: used ports by name.
- Preview channels.

´´´.cpp
//setup()
NDIHelper.setup();

//update()
NDIHelper.begin_NDI_OUT();
{
	//1. webcam
	NDIHelper.drawWebcamOut();

	//2. ndi input
	NDIHelper.draw_Preview_NDI_IN();
}
NDIHelper.end_NDI_OUT();

//draw()

//////webcam
////NDIHelper.draw_Preview_Webcam();

////draw ndi out
//NDIHelper.draw_Preview_NDI_OUT();

//----

//draw monitor
NDIHelper.draw();

//gui
NDIHelper.drawGui();

´´´

### TODO
-

## Dependencies
* https://github.com/leadedge/ofxNDI
*  

## Tested systems
- Windows10 / VS2017 / OF 0.11.0

## Author
MoebiusSurfing, July 2020. 

## License
MIT License.