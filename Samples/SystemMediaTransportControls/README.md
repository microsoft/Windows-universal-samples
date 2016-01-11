<!---
  category: AudioVideoAndCamera
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=619488
--->

# System media transport controls sample

This sample shows how to use the APIs in the Windows.Media namespace to allow your app to respond to system media events as well as providing the system with metadata about the content that is playing. 

Scenario 1:
Press the "Select Files" button in order to select some media content from your device. 

In the code behind, when the file open picker is launched and a file is selected, the file/s are assigned to the playlist defined as a list of Storage Items and the first item is passed to SetNewMediaItem. 

SetNewMediaItem performs a number of interesting jobs. It enables or disables certain system controls, such as next and previous based on where we are in the playlist. It then opens the file and sets it 
on the MediaElement using SetSource. The MediaElement is set to AutoPlay so the file will begin to play on it's own. The final thing it does is attempt to read metadata from the file 
itself, using the CopyFromFileAsync method on the DisplayUpdater in order update the SystemMediaTransportControls. It's possible to set all of the metadata required manually by directly manipulating the properties on
the display updater.

The rest of the sample focuses on some core concepts of using the System Media Transport Controls. Firstly we keep the controls up to date with our state, such as our current playback status and the position of our current playback. 

The second thing we do is set up a number of event handlers in order to handle certain events from the system. One example of this is the ButtonPressed event which sends the app commands such as play, pause and skip etc.
This is so the user can still control the app, even if it isn't neccesarily on screen. In this event handler we take the appropriate action for the command, such as pausing our media and then updating the state back on the control.

Related topics
----------------------------

https://msdn.microsoft.com/en-us/library/windows/apps/windows.media.systemmediatransportcontrols.aspx

System requirements
----------------------------

**Client:** Windows 10

**Phone:** Windows 10

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



