<!---
  category: AudioVideoAndCamera
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620571
--->

# MIDI sample

This sample demonstrates how to use the Windows.Devices.Midi API in a Windows Runtime app.

This sample demonstrates the following features:
-   Enumerate MIDI In and MIDI Out ports.
-   Attach a device watcher to monitor port arrival and removal.
-   Query the properties of a MIDI port.
-   Open MIDI In ports.
-   Open MIDI Out ports.
-   Create MIDI messages and send them to a MIDI Out ports.

## Related topics

AudioGraph, Video editing

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

## Known Issues

There is currently a problem with deploying the Microsoft.Midi.GmDls framework package within the SDK. Because of this, the GS Synth is not currently usable. The code required for the reference framework is in MIDI.csproj, but has been commented out until the issue with deploying the framework package is resolved.