<!---
  category: AudioVideoAndCamera
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=619488
--->

# System media transport controls sample

## Summary

Shows how to use the APIs in the [Windows.Media](https://msdn.microsoft.com/library/windows/apps/windows.media.aspx) namespace to allow
your app to respond to system media events as well as providing the system with
metadata about the content that is playing. 

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

In particular, this sample
demonstrates *manual* integration with SystemMediaTransportControls (SMTC).
MediaPlayer has *automatic* integration with SMTC, but since this sample
demonstrates *manual* integration with SMTC, it disables the MediaPlayer's
CommandManager which is what performs the automatic integration. Unless you have
a need to manually integrate with SMTC, such as using an audio API like WASAPI
or AudioGraph, it is recommended that you use the built-in integration through
CommandManager instead of the procedures demonstrated throughout this sample.

With automatic integration, timeline properties are synchronized from the
player, button state is inferred but can still be customized, and metadata is
updated through MediaPlaybackItem.ApplyDisplayProperties() instead of directly
updating the SMTC. In contrast, manual integration means that the app intends to
update all state and handle all events of the SMTC itself.

Note that the SMTC does not show data for the first time until the application
begins an audio playback session.

## Walkthrough

Scenario 1: Press the "Select Files" button in order to select some media
content from your device.

In the code behind, when the file open picker is launched and a file is
selected, the file/s are assigned to the playlist defined as a list of Storage
Items and the first item is passed to SetNewMediaItem.

SetNewMediaItem performs a number of interesting jobs. It enables or disables
certain system controls, such as next and previous based on where we are in the
playlist. It then opens the file and sets it on the MediaPlayer using
Source. The MediaPlayer is set to AutoPlay so the file will begin to play on
it's own. The final thing it does is attempt to read metadata from the file
itself, using the CopyFromFileAsync method on the DisplayUpdater in order update
the SystemMediaTransportControls. It's possible to set all of the metadata
required manually by directly manipulating the properties on the display
updater.

The rest of the sample focuses on some core concepts of using the System Media
Transport Controls. Firstly we keep the controls up to date with our state, such
as our current playback status and the position of our current playback.

The second thing we do is set up a number of event handlers in order to handle
certain events from the system. One example of this is the ButtonPressed event
which sends the app commands such as play, pause and skip etc. This is so the
user can still control the app, even if it isn't neccesarily on screen. In this
event handler we take the appropriate action for the command, such as pausing
our media and then updating the state back on the control.

## JavaScript

The <video> and <audio> elements do not integrate with SMTC.
The JavaScript sample shows how to connect SMTC events with the
HTML media elements.

Related topics
----------------------------

[SystemMediaTransportControls class](https://msdn.microsoft.com/library/windows/apps/windows.media.systemmediatransportcontrols.aspx)  

System requirements
----------------------------

**Client:** Windows 10 version 1607

**Phone:** Windows 10 version 1607

Build the sample
----------------------------

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

Run the sample
----------------------------

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

Deploying the sample
1. Select Build > Deploy Solution.

Deploying and running the sample
1. To debug the sample and then run it, press F5 or select Debug > Start Debugging. To run the sample without debugging, press Ctrl+F5 or select*Debug* > Start Without Debugging.



