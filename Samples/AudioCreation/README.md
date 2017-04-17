<!---
  category: AudioVideoAndCamera
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=619481
--->

# AudioGraph sample

Shows how to use the **Windows.Media.Audio** namespace to create audio graphs for audio routing, mixing, and processing scenarios. 

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

An audio graph is a set of interconnected audio nodes through which audio data flows. Audio input nodes supply audio data to the graph 
from audio input devices, audio files, or from custom code. Audio output nodes are the destination for audio processed by the graph. 

Audio can be routed out of the graph to audio output devices, audio files, or custom code. The last type of node is a submix node which 
takes audio from one or more nodes and combines them into a single output that can be routed to other nodes in the graph. 

After all of the nodes have been created and the connections between them set up, you simply start the audio graph and the audio data flows from the 
input nodes, through any submix nodes, to the output nodes. This model makes scenarios like recording from a device's microphone to an 
audio file, playing audio from a file to a device's speaker, or mixing audio from multiple sources quick and easy to implement.  

Additional scenarios are enabled with the addition of audio effects to the audio graph. Every node in an audio graph can be populated with zero 
or more audio effects that perform audio processing on the audio passing through the node. There are several built-in effects such as echo, 
equalizer, limiting, and reverb that can be attached to an audio node with just a few lines of code. 
You can also create your own custom audio effects that work exactly the same as the built-in effects.  

This sample demonstrates several common scenarios for routing and processing audio with an audio graph:  

**Scenario 1: File Playback:**    
Press the *Load File* button and file picker is shown that lets you pick an audio file to play. After selecting a file, press *Start Graph* to start playback of the file. The *Loop* toggle switch lets you toggle looping of the file on and off. The *Playback Speed* slider lets you adjust the playback speed.  

In the code-behind for this scenario, an **AudioGraph** is created and two nodes are added: an **AudioDeviceOutputNode**, which represents the default audio output device, and an **AudioFileInputNode**, which represents the audio file that you select. A connection is added from the input node to the output node. When the audio graph is started, the audio data flows from the file input node to the device output node.  

**Scenario 2: Device Capture:**  
In this scenario, audio is recorded from an input device to an audio file while being monitored on an output device. First, select the audio device that will be used for monitoring from the list box. Press the *Pick Output File* to launch a file save picker that lets you choose the file to which audio will be recorded. Press *Create Graph* to create the audio graph with the selected inputs and outputs. Press the *Record* button to begin recording from the device input to the audio file. Press the *Stop* button to stop recording.  

In the code-behind for this scenario, an **AudioGraph** is created with three nodes: an **AudioDeviceInputNode**, an **AudioDeviceOutputNode**, and a **AudioFileOutputNode**. A connection is added from the **AudioDeviceInputNode** to the **AudioDeviceOutputNode**, and another connection is added from the **AudioDeviceInputNode** to the **AudioFileOutputNode**. When the *Record* button is clicked, audio flows from the input node to the two output nodes. Also of note in this scenario is that the quantum size, or the amount of audio data that is processed at one time, is set to **LowestLatency**, which means that the audio processing will have the lowest latency possible on the device. This also means that device disconnection errors must be handled.  

**Scenario 3: Frame Input Node:**  
This scenario shows how to generate and route audio data into an audio graph from custom code. Press the *Generate Audio* button to start generating audio from custom code.
Press *Stop* to stop the audio.   

In the code-behind for this scenario an **AudioGraph** is created with two nodes: an **AudioFrameInputNode** that represents the custom audio generation code and which is connected to an **AudioDeviceOutputNode** representing the default output device. The **AudioFrameInputNode** is created with the same encoding as the audio graph so that the generated audio data has the same format as the graph. Once the audio graph is started, the **QuantumStarted** event is raised by the audio graph whenever the custom code needs to provide more audio data. The custom code creates a new **AudioFrame** object in which the audio data is stored. The example accesses the underlying buffer of the **AudioFrame**, which requires an **unsafe** code block, and inserts values from a sine wav into the buffer. The **AudioFrame** containing the audio data is then added to the **AudioFrameInputNode** list of frames ready to be processed, which is then consumed by the audio graph and passed to the audio device output node.  

**Scenario 4: Submix Nodes:**  
In this scenario, audio from two audio files is mixed together and an audio echo effect is added to the mix before the audio is routed to the output device. Press *Load File 1* to select the first audio file and *Load File 2* to select the second file. Press *Start Graph* to start the flow of audio through the graph. Use the *Echo* button to toggle the echo effect on and off while the graph is running.  

The **AudioGraph** for this scenario has four nodes: two **AudioFileInputNode** objects, an **AudioDeviceOutputNode**, and an **AudioSubmixNode**. The file input nodes are connected to the submix node which is connected to the audio device output node. An *EchoEffectDefinition* object representing the built-in echo effect is created and added to the **EffectDefinitions** list of the submix node. The **EnableEffectsByDefinition** and **DisableEffectsByDefinition** methods are used to toggle the effect on and off. The effect model shown in this example is implemented by all audio node types, so you can add effects at any point in the audio graph. Also, submix nodes can be chained together to easily create mixes with complex layering of effects.  

**Scenario 5: In-box Effects:**  
This example demonstrates each of the effects that are built-in to the platform. These include:  
* Echo  
* Reverb  
* Equalizer  
* Limiter  
The UI for this scenario lets you load an audio file to play back and then toggle these effects on and off and adjust their parameters.

The code-behind for this scenario uses just two nodes. An **AudioFileInputNode** and a **AudioDeviceOutputNode**. The effects are initialized and then added to the **EffectDefinitions** list of the file input node.

**Scenario 6: Custom Effects:**  
This scenario demonstrates how to create an custom audio effect and then using it in an audio graph. Press the *Load File* button to select an audio file to play. Press *Start Graph* to begin playback of the file with the custom effect.  

The custom effect for this scenario is defined in a single file, CustomEffect.cs, that is included in its own project. The class implemented in this file, AudioEchoEffect, implements the **IBasicAudioEffect** interface which allows it to be used in an audio graph. The actual audio processing is implemented in the **ProcessFrame** method. The audio graph calls this method and passes in a **ProcessAudioFrameContext** object which provides access to **AudioFrame** objects representing the input to the effect and the output from the effect. The effect implements a simple echo by storing samples from the input frame in a buffer and then adding the samples previously stored in the buffer to the current input samples and then inserting thos values into the output frame buffer.  

The custom effect has a property set that can be modified by calling the **SetProperties** method. The custom effect exposes a *Mix* property through the property set that is used to control the amount of echo that is added back into the original signal.

Related topics
--------------

[Windows.Media.Audio namespace] (https://msdn.microsoft.com/library/windows/apps/windows.media.audio.aspx)


System requirements
-----------------------------

**Client:** Windows 10

**Phone:** Windows 10

Build the sample
----------------

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

Run the sample
--------------

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

**Deploying the sample**
1.  Select **Build** \> **Deploy Solution**.

**Deploying and running the sample**
1.  To debug the sample and then run it, press F5 or select **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or select**Debug** \> **Start Without Debugging**.

