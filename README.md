

# ofxNDIHelper


## Overview

Helper add-on for **openFrameworks** to handle [NDI](https://www.ndi.tv/tools/) 2x**Inputs** / 1x**Output** ports and input **Webcam** devices with **customizable Canvas Layout**.  
Distribute your NDI and Webcam Input, draggable and scalable by mouse.  
Feed your oF drawing easy into a network broadcasted NDI Output.

## Screenshots


### 4_Example_Input

A received NDI Input drawing into oF.  


![](/1_Examples_ofxGui/4_Example_Input/Capture.PNG)


### 5_Example_Full

All the incoming channels are drawn into a Canvas Layout, and sent to an NDI Output.  
Broadcasted to the network and previewed into an NDI Monitor App.  
Also receiving an NDI Pattern generator as an NDI Input, to be drawn into oF. 

![](/1_Examples_ofxGui/5_Example_Full/Capture.PNG)


## Features

- Add an **NDI Manager** to your apps **fast**.

- 1 Webcam INPUT + 2 NDI INPUTS + 1 NDI OUTPUT.

- List, select and draw **NDI INPUT Devices** from, feed to an OUTPUT, and draw or route a **Webcam** device too.

- Draggable and scalable **Mini Previews Layout**.

- Auto **store and recall** all the **settings**.


## Usage:

Look Examples for **ofxGui** or **ImGui** modes.


## Dependencies

* [ofxNDI](https://github.com/leadedge/ofxNDI)

* [ofxSurfingHelpers](https://github.com/moebiussurfing/ofxSurfingHelpers)

* [ofxSurfingBox](https://github.com/moebiussurfing/ofxSurfingBox)

* [ofxChildFrame](https://github.com/nariakiiwatani/ofxChildFrame) / Optional. Can be disabled.

* ofxGui / oF Core  


<details>

  <summary>For examples only</summary>

  <p>


* [ofxWindowApp](https://github.com/moebiussurfing/ofxWindowApp)

* [ofxSceneTEST](https://github.com/moebiussurfing/ofxSceneTEST)

  </p>

</details>


<details>

  <summary>HowTo. New to NDI?</summary>

  <p>


1. You should install the [NDI Tools](https://www.ndi.tv/tools/). It's a bundle of apps.

2. Run an **NDI Studio Monitor** app to preview what is being sent through the **NDI OUTPUT**.

3. You can run an **NDI Test Pattern** to feed a signal into the **NDI INPUT** of the add-on.

4. Have fun with **sources** toggles on the add-on GUI to explore combinations,

while looking at the **NDI OUT Preview** or the **NDI Studio Monitor** video.

5. NDI works locally or on local or remote networks.

6. **NDI Screen Capture** app does live desktop capture to NDI.

7. **NDI Webcam Input** converts NDI ports to a virtual camera to use on **OBS**/**Zoom**/**Skype**.

  </p>

</details>


## Tested Systems

- **Windows 10** / **VS 2022** / **oF 0.12**


## Author

moebiusSurfing, 2020-2022.  

( ManuMolina ) 


## License

MIT License
