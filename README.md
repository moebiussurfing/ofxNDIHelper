# ofxNDIHelper

## Overview
Helper add-on for **openFrameworks** to handle **NDI Input**/**Output** ports and input **Webcam** devices with **customizable layout of previews** and **auto store/recall settings**.

## Screenshot
![image](/readme_images/Capture.PNG?raw=true "Capture.PNG")

## Features
- List and **select NDI Devices** and **Webcam** devices.
- Auto **store and recall settings** of selected ports by name.
- **Mini Preview** channels with easy **customizable layout** using mouse.

```.cpp
// setup()
NDIHelper.setup();

// update()
NDIHelper.begin_NDI_OUT();
{
	//1. webcam
	NDIHelper.drawWebcamOut();

	//2. ndi input
	NDIHelper.draw_Preview_NDI_IN();
}
NDIHelper.end_NDI_OUT();

//draw()

////// webcam
////NDIHelper.draw_Preview_Webcam();

//// draw ndi out
//NDIHelper.draw_Preview_NDI_OUT();

//----

//draw monitor
NDIHelper.draw();

//gui
NDIHelper.drawGui();
```

## Dependencies
* [ofxSurfingHelpers](https://github.com/moebiussurfing/ofxSurfingHelpers) 
* OfxGui [ OF Core ]

## Tested Systems
- **Windows10** / **VS2017** / **OF 0.11**

## Author
moebiusSurfing, 2020.  
(ManuMolina) 

## License
MIT License.