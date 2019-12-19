---
page_type: sample
languages:
- csharp
- cpp
- cppcx
- cppwinrt
products:
- windows
- windows-uwp
urlFragment: BasicHologram
extendedZipContent:
- path: SharedContent
  target: SharedContent
- path: LICENSE
  target: LICENSE
description: "Shows how to render a simple hologram that is a spinning cube."
---

<!---
  category: Holographic
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=864208
--->

# Basic hologram sample

Shows how to render a simple hologram that is a spinning cube.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> You can download this sample as a standalone ZIP file
> [from docs.microsoft.com](https://docs.microsoft.com/samples/microsoft/windows-universal-samples/basichologram/),
> or you can download the entire collection as a single
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample runs on Windows Mixed Reality and renders a spinning cube. You can
interact with the cube by placing it in a new position, and a variety of input methods are allowed.
This sample works on PCs with headset devices attached, and it works on Microsoft HoloLens.

This sample supports one operational mode: upon app launch, a spinning cube will appear in front
of the user. To move the spinning cube, look somewhere else, and then use any of these input 
methods to place it in front of you:
  * Air tap gesture, or "Select" speech command, when running on a Microsoft HoloLens
  * Controller trigger button, or "Select" speech command, when running on a PC with headset
  * The "A" button on an attached XBox 360 or XBox One controller
  * Left mouse click

This sample also shows how to use the CommitDirect3D11DepthBuffer API. This API allows the app to 
submit a depth buffer for per-pixel image stabilization.


### Additional remarks

**Note** The Windows universal samples for Windows Mixed Reality require Visual Studio
to build, and a Windows Holographic or Windows Mixed Reality device to execute. Windows Holographic 
devices include the Microsoft HoloLens and the Microsoft HoloLens Emulator. Windows Mixed Reality
headsets are available for use with Windows 10 PCs.

To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421).

To obtain information about the tools used for Windows Holographic development, including Microsoft 
Visual Studio and the Microsoft HoloLens Emulator, go to
[Install the tools](https://developer.microsoft.com/windows/mixed-reality/install_the_tools).


## Reference

The following Windows Universal API namespaces are used in this code sample:

* [Windows.Perception](https://msdn.microsoft.com/library/windows/apps/windows.perception.aspx)  
* [Windows.Perception.Spatial](https://msdn.microsoft.com/library/windows/apps/windows.perception.spatial.aspx)  
* [Windows.Graphics.Holographic](https://msdn.microsoft.com/library/windows/apps/windows.graphics.holographic.aspx)  

## System requirements

**Client:** Windows 10 Holographic or Windows Mixed Reality

**Phone:** Not supported

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with 
   the sample you want to build. 
2. If you intend to build the C++/WinRT sample,
   install the [C++/WinRT Visual Studio Extension](https://aka.ms/cppwinrt/vsix)
   if you haven't already.
2. Start Microsoft Visual Studio and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the 
   subfolder for this specific sample, then the subfolder for your preferred language.
   Double-click the Visual Studio Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and 
run it.

### Deploying the sample on a Windows Mixed Reality PC

- Ensure the Windows Mixed Reality headset is correctly plugged in, and that the Mixed Reality Portal is running.
- In Visual Studio, click the debug target drop-down and select **Local Machine**, then press F5 or select **Debug** \> **Start Debugging**.

### Deploying the sample to the Microsoft HoloLens emulator

- Click the debug target drop-down, and select **Microsoft HoloLens Emulator**.
- Select **Build** \> **Deploy** Solution.

### Deploying the sample to a Microsoft HoloLens

- Developer unlock your Microsoft HoloLens. For instructions, go to
  [Enable your device for development](https://msdn.microsoft.com/windows/uwp/get-started/enable-your-device-for-development#enable-your-windows-10-devices).
- Find the IP address of your Microsoft HoloLens. The IP address can be found in **Settings** \> 
  **Network & Internet** \> **Wi-Fi** \> **Advanced options**. Or, you can ask Cortana for this 
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
- On your Microsoft HoloLens, go to **Settings** \> **Update** \> **For developers**, and click on **Pair**.
- Type the PIN displayed by your Microsoft HoloLens into the Visual Studio dialog box and click **OK**.
- On your Microsoft HoloLens, select **Done** to accept the pairing.
- The solution will then start to deploy.

### Deploying and running the sample on the HoloLens

- To debug the sample and then run it, follow the steps listed above to connect your developer-
  unlocked Microsoft HoloLens, then press F5 or select **Debug** \> **Start Debugging**. To run the
  sample without debugging, press Ctrl+F5 or select **Debug** \> **Start Without Debugging**. 
