<!---
  category: Holographic
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=808267
--->

# Tag-along hologram sample

Shows how to render a hologram that follows the user.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample uses a device-attached frame of reference with stereoscopic billboarding,
linear interpolation, velocity estimation, and image stabilization to render a holographic message 
that the user can always comfortably see.

This sample supports one operational mode: upon app launch, a flat billboard will fade in close to 
the center of the field-of-view (FOV) of the device. As the user wearing the device moves and looks
around, the billboard is adjusted to follow changes in device position and orientation so that it 
is always visible to the user. The user can affix the hologram to its current position using an air
tap gesture, and pick it back up by air tapping again.

## Holographic rendering best practices

This sample demonstrates best practices for user comfort. The motion of the billboard is dampened 
so that it is part of the world, not affixed to the eye, and image stabilization is configured to 
follow the billboard's position and velocity.

**Note** The image stabilization plane can be observed via the [Windows Device Portal] 
(https://developer.microsoft.com/windows/holographic/Using_the_Windows_Device_Portal.html).

## Holographic rendering of vector graphics using low-bandwidth distance function textures

Additionally, this sample demonstrates how to render high-resolution, anti-aliased text as a hologram
using less memory bandwidth by generating signed distance values that are used to recreate the shape of
the text in applicable pixels during the hologram render pass. This is a suggested method for enhancing 
holographic output.

This technique can be applied to any high-resolution, antialiased image representing a vector graphic; 
this sample uses DirectWrite to create the text at app startup, and generates the signed distance values 
after Direct3D device-based resource loading is complete.

**Reference** The method used is as described by Chris Green in the following text: 
[Improved Alpha-Tested Magniﬁcation for Vector Textures and Special Effects] (http://www.valvesoftware.com/publications/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf), 
Chris Green, Valve Software, 2007.


### Additional remarks

**Note** The Windows universal samples for Windows 10 Holographic require Visual Studio 2015 Update 2
to build, and a Windows Holographic device to execute. Windows Holographic devices include the 
Microsoft HoloLens and the Microsoft HoloLens Emulator.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421).

To obtain information about the tools used for Windows Holographic development, including Microsoft 
Visual Studio 2015 Update 2 and the Microsoft HoloLens Emulator, go to [Install the tools]
(https://developer.microsoft.com/windows/holographic/install_the_tools).


## Reference

The following Windows Universal APIs are used to demonstrate spatial locations, and holographic 
rendering in this code sample:

* [Windows.Perception](https://msdn.microsoft.com/library/windows/apps/windows.perception.aspx)  
* [Windows.Perception.Spatial](https://msdn.microsoft.com/library/windows/apps/windows.perception.spatial.aspx)  
  * [SpatialLocator class](https://msdn.microsoft.com/library/windows/apps/windows.perception.spatial.spatiallocator.aspx)  
    * [SpatialLocator.GetDefault method](https://msdn.microsoft.com/library/windows/apps/windows.perception.spatial.spatiallocator.getdefault.aspx)  
    * [SpatialLocator.CreateAttachedFrameOfReferenceAtCurrentHeading methods](https://msdn.microsoft.com/library/windows/apps/windows.perception.spatial.spatialLocator.createattachedframeofreferenceatcurrentheading.aspx)  
  * [SpatialLocatorAttachedFrameOfReference class](https://msdn.microsoft.com/library/windows/apps/windows.perception.spatial.spatiallocatorattachedframeofreference.aspx)  
    * [SpatialLocatorAttachedFrameOfReference.GetStationaryCoordinateSystemAtTimestamp method](https://msdn.microsoft.com/library/windows/apps/windows.perception.spatial.spatiallocatorattachedframeofreference.getstationarycoordinatesystemattimestamp.aspx)  
  * [SpatialCoordinateSystem class](https://msdn.microsoft.com/library/windows/apps/windows.perception.spatial.spatialcoordinatesystem.aspx)  
    * [SpatialCoordinateSystem.TryGetTransformTo method](https://msdn.microsoft.com/library/windows/apps/windows.perception.spatial.spatialcoordinatesystem.trygettransformto.aspx)  
* [Windows.Graphics.Holographic](https://msdn.microsoft.com/library/windows/apps/windows.graphics.holographic.aspx)  
  * [HolographicCamera class](https://msdn.microsoft.com/library/windows/apps/windows.graphics.holographic.holographiccamera.aspx)  
  * [HolographicCameraRenderingParameters class](https://msdn.microsoft.com/library/windows/apps/windows.graphics.holographic.holographiccamerarenderingparameters.aspx)  
    * [HolographicCameraRenderingParameters.SetFocusPoint methods](https://msdn.microsoft.com/library/windows/apps/windows.graphics.holographic.holographiccamerarenderingparameters.setfocuspoint.aspx)  

## System requirements

**Client:** Windows 10 Holographic

**Phone:** Not supported

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with 
   the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the 
   subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or 
   JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and 
run it.

### Deploying the sample to the Microsoft HoloLens emulator

- Click the debug target drop-down, and select **Microsoft HoloLens Emulator**.
- Select **Build** \> **Deploy** Solution.

### Deploying the sample to a Microsoft HoloLens

- Developer unlock your Microsoft HoloLens. For instructions, go to [Enable your device for development]
  (https://msdn.microsoft.com/windows/uwp/get-started/enable-your-device-for-development#enable-your-windows-10-devices).
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

### Deploying and running the sample

- To debug the sample and then run it, follow the steps listed above to connect your developer-
  unlocked Microsoft HoloLens, then press F5 or select **Debug** \> **Start Debugging**. To run the
  sample without debugging, press Ctrl+F5 or select **Debug** \> **Start Without Debugging**. 
