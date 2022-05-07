# ofxNDIHelper

## Overview
Helper add-on for **openFrameworks** to handle [NDI](https://www.ndi.tv/tools/) **Input**/**Output** ports and input **Webcam** devices with **customizable layout of previews** and **auto store settings**.

## Screenshot
![image](/readme_images/Capture.PNG?raw=true "Capture.PNG")

## Features
- Add an **NDI Manager** to your apps **fast**: send and receive.
- List, select and feed **NDI Devices** and route a **Webcam** device.
- Auto **store and recall** settings and layout.
- Draggable **Mini Previews**  with easy **customizable**.

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
        /* Draw your scene */

        // Draw the connected Webcam
        NDIHelper.draw_Preview_Webcam();
        //NDIHelper.draw_WebcamOut(); //-> Another useful method

        // Draw the video from the NDI Input
        NDIHelper.draw_Preview_NDI_IN();
    }
    NDIHelper.end_NDI_OUT();

    NDIHelper.update();
}

void ofApp::draw()
{
    // Draw Previews
    NDIHelper.draw();

    // Gui
    NDIHelper.draw_Gui();
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
moebiusSurfing, 2020-2022.  
( ManuMolina ) 

## License
MIT License