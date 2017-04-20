<!---
  category: AudioVideoAndCamera
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620571
--->

# MIDI sample

Shows how to use the [Windows.Devices.Midi](https://msdn.microsoft.com/library/windows/apps/windows.devices.midi.aspx) API in a Windows Runtime app.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample demonstrates the following features:
-   Enumerate MIDI In and MIDI Out ports.
-   Attach a device watcher to monitor port arrival and removal.
-   Query the properties of a MIDI port.
-   Open MIDI In ports.
-   Open MIDI Out ports.
-   Create MIDI messages and send them to a MIDI Out ports.

## Related topics

[AudioGraph class](https://msdn.microsoft.com/library/windows/apps/windows.media.audio.audiograph.aspx)  
[Media compositions and editing](https://msdn.microsoft.com/windows/uwp/audio-video-camera/media-compositions-and-editing)  

## Operating system requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

To debug the app and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the app without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.

## Sounds for the MIDI synth

The sounds for the inbox MIDI GS synth are available when the Microsoft.Midi.GmDls framework package is added to the package. This SDK sample has the reference added to the project already. Note that this requires Visual Studio 2015 Update 1 [https://www.visualstudio.com/news/vs2015-update1-vs] to be installed for the appx to deploy successfully.