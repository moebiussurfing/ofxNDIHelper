# ofxNDIHelper

## Overview
a helper addon for **openFrameworks** to handle **NDI input/output** ports and **webcam** input devices with **previews** and **settings**.

## Screenshot
![Alt text](/readme_images/Capture.PNG?raw=true "Capture.PNG")

## Features
- List and **select NDI channels** and **webcam** devices.
- **Store and recall settings** of selected ports by name.
- **Mini preview** channels with **resizeable layout**.

```.cpp
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
```

## TODO / IDEAS
-

## Dependencies
* https://github.com/leadedge/ofxNDI  
  https://github.com/roymacdonald/ofxScaleDragRect  Already included into `/libs`, no need to add into your project. Thanks **@roymacdonald**!  

## Tested systems
- **Windows10** / **VS2017** / **OF 0.11**

## Author
moebiusSurfing, 2020.  
(ManuMolina) 

## License
MIT License.