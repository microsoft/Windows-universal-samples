---
page_type: sample
languages:
- cpp
- cppcx
products:
- windows
- windows-uwp
urlFragment: HolographicSpatialStage
extendedZipContent:
- path: SharedContent
  target: SharedContent
- path: LICENSE
  target: LICENSE
description: "Shows how to acquire a SpatialStage, and make use of its coordinate system and properties."
---

<!---
  category: Holographic
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=851304
--->

# Holographic spatial stage sample

Shows how to acquire a SpatialStage, and make use of its coordinate system and properties.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> You can download this sample as a standalone ZIP file
> [from docs.microsoft.com](https://docs.microsoft.com/samples/microsoft/windows-universal-samples/holographicspatialstage/),
> or you can download the entire collection as a single
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample obtains a spatial stage, inspects the properties of the stage, and renders
a visualization of the stage based on its properties.

The spatial stage includes a 2D bounding shape indicating the area where the user can walk around. This
sample acquires that shape, and uses it to draw the spatial stage with the walkable area on it. This 
sample also draws an indicator for the look direction range.


### Additional remarks

**Note** The Windows universal samples for Windows Mixed Reality require Visual Studio 
to build, and a Windows Holographic device to execute. Windows Mixed Reality devices include the
Microsoft HoloLens, the Microsoft HoloLens Emulator, and PCs with immersive headset devices attached.

To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421).

To obtain information about the tools used for Windows Mixed Reality development, including
Microsoft Visual Studio and the Microsoft HoloLens Emulator, go to
[Install the tools](https://developer.microsoft.com/windows/holographic/install_the_tools).

## Reference

The following APIs are used in this code sample:

* [Windows.Perception namespace](https://msdn.microsoft.com/library/windows/apps/windows.perception.aspx)
* [Windows.Perception.Spatial namespace](https://msdn.microsoft.com/library/windows/apps/windows.perception.spatial.aspx)
  * [SpatialStageFrameOfReference class](https://msdn.microsoft.com/library/windows/apps/windows.perception.spatial.SpatialStageFrameOfReference.aspx)

## System requirements

**Client:** Windows 10 Holographic or Windows Mixed Reality

**Phone:** Not supported

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

- If your Windows Mixed Reality target device is separate from your development machine, click the debug target drop-down
  and select the appropriate target. If needed, use the properties panel of the project to set the remote computer IP and
  authentication method.
- Select **Build** \> **Deploy** Solution.

### Deploying the sample to a Microsoft HoloLens

- Developer unlock your Microsoft HoloLens. For instructions, go to [Enable your device for development]
  (https://msdn.microsoft.com/windows/uwp/get-started/enable-your-device-for-development#enable-your-windows-10-devices).
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
