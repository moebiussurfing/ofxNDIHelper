# ofxNDIHelper

## Overview
Helper add-on for **openFrameworks** to handle [NDI](https://www.ndi.tv/tools/) 2x**Inputs** / 1x**Output** ports and input **Webcam** devices with **customizable layout**.

## Screenshot
![image](/readme_images/Capture.PNG?raw=true "Capture.PNG")

## Features
- Add an **NDI Manager** to your apps **fast**.
- 1 Webcam INPUT + 2 NDI INPUTS + 1 NDI OUTPUT.
- List, select and draw **NDI INPUT Devices** from, feed to an OUTPUT, and draw or route a **Webcam** device too.
- Draggable and scalable **Mini Previews Layout**.
- Auto **store and recall** all the **settings**.

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
    // Draw and feed the signals from the NDI Manager inside the NDI OUTPUT too.
    // That's the video signal we will send to the network!

    NDIHelper.begin_NDI_OUT();
    {
        /* Draw Your Scene! */

        //--

        /* Feed channels from the addon */

        // 1. Draw the receiving signal from NDI Input from the add-on:
        //-> The Mini screen of NDI Input
        NDIHelper.draw_NDI_IN_MiniPreview();

        //-

        // 2. Draw the connected Webcam from the add-on:
        //-> The mini preview of the camera
        NDIHelper.draw_Webcam_MiniPreview(); 
    }
    NDIHelper.end_NDI_OUT();
}

void ofApp::draw()
{
    // Draw Preview Monitors
    NDIHelper.draw();

    /* You can draw from the add-on here to you "local" scene. */

    // Gui
    NDIHelper.draw_Gui();
}
```

## Dependencies
* [ofxNDI](https://github.com/leadedge/ofxNDI)
* [ofxSurfingHelpers](https://github.com/moebiussurfing/ofxSurfingHelpers)
* [ofxSurfingBox](https://github.com/moebiussurfing/ofxSurfingBox)
* ofxGui / From oF Core  

<details>
  <summary>For examples only</summary>
  <p>

* [ofxWindowApp](https://github.com/moebiussurfing/ofxWindowApp)
* [ofxSceneTEST](https://github.com/moebiussurfing/ofxSceneTEST)
  </p>
</details>

Notice that must be present into ```/bin/data/``` :  

  _Processing.NDI.Lib.x64.dll_  
  _Processing.NDI.Lib.x864.dll_  

<details>
  <summary>HowTo. New to NDI?</summary>
  <p>

1. You should install the [NDI Tools](https://www.ndi.tv/tools/). It's a bundle of apps.
2. Run an **NDI Studio Monitor** app to preview what is being sent through the **NDI OUTPUT**.
3. You can run an **NDI Test Pattern** to feed a signal into the **NDI INPUT** of the add-on.
4. Have fun with **sources** toggles on the add-on GUI to explore combinations,
while looking to the **NDI OUT Preview** or the **NDI Studio Monitor** video.
5. NDI works locally or on a local or remote networks.
6. **NDI Screen Capture** app does live desktop capture to NDI.
7. **NDI Webcam Input** converts NDI ports to a virtual camera to use on **OBS**/**Zoom**/**Skype**.
  </p>
</details>

## Tested Systems
- **Windows 10** / **VS 2017** / **oF 0.11**
- **Windows 10** / **VS 2022** / **oF 0.12**

## Author
moebiusSurfing, 2020-2022.  
( ManuMolina ) 

## License
MIT License
