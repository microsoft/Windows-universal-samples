<!---
  category: AudioVideoAndCamera 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=809050
--->

# Camera frames sample

Shows how to perform various tasks related to individual frames captured through a camera.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample shows how to:

- Find cameras which support color, depth and/or infrared
- Access cameras in shared mode
- Create and read frames from a FrameReader
- Process input frames pixel by pixel
- Rendering frames to an image element
- Use a DeviceWatcher to monitor when cameras are connected and disconnected

### Source groups

Use the MediaFrameSourceGroup.FindAllAsync method to identify all capture source groups.
Each group can offer multiple sources, such as color, depth and infrared.
You can inspect the groups to find one that meets your needs,
then initialize a MediaCapture with that group.

### Shared mode

Set SharingMode to SharedReadOnly when initializing the MediaCapture to access the camera
at the same time as another app but without the ability to modify camera settings.

### Create and read frames from a FrameReader

Use the FrameReader class to read frames from a frame source as they arrive.

### Additional remarks

**Note** The Windows universal samples for Windows 10 require Visual Studio 2015 Update 2
and Windows SDK version 14332 or above to build.

To obtain information about Windows 10 development, go to the [Windows Dev Center](https://dev.windows.com).

## See also

### Samples

[HolographicFaceTracking](/Samples/HolographicFaceTracking)

### Reference

[Windows.Media.Capture.Frames namespace](https://msdn.microsoft.com/library/windows/apps/windows.media.capture.frames.aspx)

## System requirements

**Client:** Windows 10

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

### Deploying and running the sample

- To debug the sample and then run it, follow the steps listed above to connect your
  developer-unlocked Microsoft HoloLens, then press F5 or select **Debug** \> **Start Debugging**.
  To run  the sample without debugging, press Ctrl+F5 or select **Debug** \> **Start Without Debugging**.
