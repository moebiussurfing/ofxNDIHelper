# ofxNDIHelper

## Overview
Helper add-on for **openFrameworks** to handle [NDI](https://www.ndi.tv/tools/) **Input**/**Output** ports and input **Webcam** devices with **customizable layout**.

## Screenshot
![image](/readme_images/Capture.PNG?raw=true "Capture.PNG")

## Features
- Add an **NDI Manager** to your apps **fast**.
- 1 Webcam Input + 1 NDI Input + 1 NDI Output.
- List, select and feed **NDI Devices** and route a **Webcam** device.
- Auto **store and recall** all the settings and layout.
- Draggable **Mini Previews**.

## Example
```.cpp
#include "ofxNDIHelper.h"
ofxNDIHelper NDIHelper;

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
        /* Draw Your Scene! */

        //--

        // 1. Draw the receiving signal from NDI Input from the add-on:
        
        //-> The Full screen of NDI Input
        NDIHelper.draw_NDI_IN_Full();

        //-> The Mini screen of NDI Input
        NDIHelper.draw_NDI_IN_MiniPreview();

        //--

        // 2. Draw the connected Webcam from the add-on:

        //-> The full preview of the camera.
        NDIHelper.draw_Webcam_Full();

        //-> The mini preview of the camera.
        NDIHelper.draw_Webcam_MiniPreview(); 
    }
    NDIHelper.end_NDI_OUT();

    NDIHelper.update();
}

void ofApp::draw()
{
    // Draw Preview Monitors
    NDIHelper.draw();

    // Gui
    NDIHelper.draw_Gui();
}
```

## Dependencies
* [ofxNDI](https://github.com/leadedge/ofxNDI)
* [ofxSurfingHelpers](https://github.com/moebiussurfing/ofxSurfingHelpers)
* ofxGui / oF Core
* [ofxWindowApp](https://github.com/moebiussurfing/ofxWindowApp) / Optional

## HowTo
To Test and understand how it works:  
1. You should install the [NDI Tools](https://www.ndi.tv/tools/).
2. Run an NDI Monitor to preview what is being sent through the output.
3. You can run an NDI Test Patterns to feed a signal into the NDI Addon.
4. Have fun with Sources toggles and the addon GUI to explore combinations,
while looking to the NDI Out Preview GUI or the NDI Monitor video.

## Tested Systems
- **Windows 10** / **VS 2017** / **OF 0.11**

## Author
moebiusSurfing, 2020-2022.  
( ManuMolina ) 

## License
MIT License