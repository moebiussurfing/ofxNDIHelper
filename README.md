# ofxNDIHelper

## Overview
Helper add-on for **openFrameworks** to handle **NDI Input**/**Output** ports and input **Webcam** devices with **customizable layout of previews** and **auto store settings**.

## Screenshot
![image](/readme_images/Capture.PNG?raw=true "Capture.PNG")

## Features
- Add a NDI manager to your apps.
- List, select and feed **NDI Devices** and route **Webcam** devices.
- Auto **store and recall settings** of selected ports by name.
- Draggable **Mini Previews**  with easy **customizable layout** mouse.

## Example
```.cpp
void ofApp::setup()
{
	NDIHelper.setup();
}

void ofApp::update()
{
	// Draw inside to feed the NDI Video Output.
	// that's the signal we will send!

	NDIHelper.begin_NDI_OUT();
	{
		// Layers one upper others:

		// 0. Draw a simple and animated scene:
		drawScene();

		// 1. Draw the connected Webcam
		NDIHelper.draw_Preview_Webcam();
		//NDIHelper.draw_WebcamOut(); //-> Another useful method

		// 2. Draw the receiving from NDI Input
		NDIHelper.draw_Preview_NDI_IN();
	}
	NDIHelper.end_NDI_OUT();

	NDIHelper.update();
}

void ofApp::draw()
{
	// Draw monitor
	NDIHelper.draw();

	// Gui
	NDIHelper.draw_Gui();

/*
	// Other useful methods:

	// Preview Webcam
	NDIHelper.draw_Preview_Webcam();

	// Preview NDI Out
	NDIHelper.draw_Preview_NDI_OUT();
*/
}
```

## Dependencies
* [ofxNDI](https://github.com/leadedge/ofxNDI)
* [ofxSurfingHelpers](https://github.com/moebiussurfing/ofxSurfingHelpers)
* ofxGui [ OF Core ]
* [ofxWindowApp](https://github.com/moebiussurfing/ofxWindowApp) [ Optional ]

## Tested Systems
- **Windows 10** / **VS 2017** / **OF 0.11**

## Author
moebiusSurfing, 2020.  
(ManuMolina) 

## License
MIT License.