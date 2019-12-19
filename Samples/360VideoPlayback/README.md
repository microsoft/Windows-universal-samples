---
page_type: sample
languages:
- csharp
- cpp
- cppcx
products:
- windows
- windows-uwp
urlFragment: 360VideoPlayback
extendedZipContent:
- path: SharedContent
  target: SharedContent
- path: LICENSE
  target: LICENSE
description: "Shows how to play 360-degree video."
---

<!---
  category: AudioVideoAndCamera
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=853072
--->

# 360-degree Video Playback sample

Shows how to play 360-degree video.

The sample uses the MediaPlayer class for 360-degree playback.
For flat devices (PC, Xbox, Mobile) the MediaPlayer handles all aspects of the rendering.
The developer need only provide the look direction.
For Mixed Reality, this sample shows how to use
the MediaPlayer to obtain individual video frames, and render them to a head-mounted display.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> You can download this sample as a standalone ZIP file
> [from docs.microsoft.com](https://docs.microsoft.com/samples/microsoft/windows-universal-samples/360videoplayback/),
> or you can download the entire collection as a single
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this samples covers:

- Creating a hybrid application which works across flat devices (PC, Xbox, Mobile) and Mixed Reality devices (MR headset, Hololens).
- Handling various forms of user input including mouse, keyboard and gamepad.
- Selecting the playback mode (flat or immersive) based on the environment the app is running in.
- Creating multiple windows or views of the app and switching between the XAML (flat) and DirectX (immersive) Windows.
- The C++ version of the sample also implements some transport controls during immersive playback.

## Additional remarks

**Note** The Windows universal samples for Windows 10 Holographic require Visual Studio
to build, and a Windows Holographic device to execute. Windows Holographic devices include the
Microsoft HoloLens and the Microsoft HoloLens Emulator.

To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421).

To obtain information about the tools used for Windows Holographic development, including
Visual Studio and the Microsoft HoloLens Emulator, go to
[Install the tools](https://developer.microsoft.com/windows/mixed-reality/install_the_tools).

### Reference
[MediaPlayer](https://docs.microsoft.com/en-us/uwp/api/windows.media.playback.mediaplayer)  
[Mixed Reality Development](https://developer.microsoft.com/en-us/windows/mixed-reality/development)  
[Creating a Holographic DirextX Project](https://developer.microsoft.com/en-us/windows/mixed-reality/creating_a_holographic_directx_project)  
[Using XAML with Holographic DirectX Apps](https://developer.microsoft.com/en-us/windows/mixed-reality/using_xaml_with_holographic_directx_apps)  
[ApplicationViewSwitcher](http://msdn.microsoft.com/library/windows/apps/dn281094)  

## System requirements

**Client:** Windows 10 build 15063, Windows 10 Holographic

**Phone:** Windows 10 build 15063

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with
   the sample you want to build.
2. Start Microsoft Visual Studio and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the
   subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or
   JavaScript). Double-click the Visual Studio Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and
run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 

### Deploying the sample to the Microsoft HoloLens emulator

- Click the debug target drop-down, and select **Microsoft HoloLens Emulator**.
- Select **Build** \> **Deploy** Solution.

### Deploying the sample to a Microsoft HoloLens

- Developer unlock your Microsoft HoloLens. For instructions, go to
  [Enable your device for development](https://msdn.microsoft.com/windows/uwp/get-started/enable-your-device-for-development#enable-your-windows-10-devices).
- Find the IP address of your Microsoft HoloLens. The IP address can be found in **Settings**
  \> **Network & Internet** \> **Wi-Fi** \> **Advanced options**. Or, you can ask Cortana for this
  information by saying: "Hey Cortana, what's my IP address?"
- Right-click on your project in Visual Studio, and then select **Properties**.
- In the Debugging pane, click the drop-down and select **Remote Machine**.
- Enter the IP address of your Microsoft HoloLens into the field labelled **Machine Name**.
- Click **OK**.
- Select **Build** \> **Deploy** Solution.

### Pairing your developer-unlocked Microsoft HoloLens with Visual Studio

The first time you deploy from your development PC to your developer-unlocked Microsoft HoloLens,
you will need to use a PIN to pair your PC with the Microsoft HoloLens.
- When you select **Build** \> **Deploy Solution**, a dialog box will appear for Visual Studio to
  accept the PIN.
- On your Microsoft HoloLens, go to **Settings** \> **Update** \> **For developers**, and click on
  **Pair**.
- Type the PIN displayed by your Microsoft HoloLens into the Visual Studio dialog box and click
  **OK**.
- On your Microsoft HoloLens, select **Done** to accept the pairing.
- The solution will then start to deploy.

### Deploying and running the sample

- To debug the sample and then run it, follow the steps listed above to connect your
  developer-unlocked Microsoft HoloLens, then press F5 or select **Debug** \> **Start Debugging**.
  To run the sample without debugging, press Ctrl+F5 or select **Debug** \> **Start Without Debugging**.
