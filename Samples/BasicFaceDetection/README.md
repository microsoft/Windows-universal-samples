<!---
  category: AudioVideoAndCamera 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620512
--->

# Basic face detection sample

Shows how to use the [FaceDetector](https://msdn.microsoft.com/library/windows/apps/windows.media.faceanalysis.facedetector.aspx) 
class to find human faces within a still image.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample shows how to:

- **Initialize and manage** the FaceDetector object
- **Execute face detection** to find human faces within a photograph file or within a single frame of video
- **Acquire bitmap data** from a source file (jpeg, bmp, or png) and convert it to a format supported by FaceDetector
- **Acquire a video frame** from a video capture stream and convert it to a format supported by FaceDetector
- **Retrieve results** from FaceDetector and visualize them

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

The FaceDetector is intended to operate on a static image or a single frame of video and is not optimized for video playback or live camera streams. In order to track human faces in real-time, either through a live stream or a video clip, use the FaceTracker API instead.

### Samples

[BasicFaceTracking](/Samples/BasicFaceTracking)

[HolographicFaceTracking](/Samples/HolographicFaceTracking)

[FilePicker](/Samples/FilePicker)

[CameraStarterKit](/Samples/CameraStarterKit)

[SimpleImaging](/Samples/SimpleImaging)

[Media capture using capture device](https://code.msdn.microsoft.com/windowsapps/Media-Capture-Sample-adf87622)

### Reference

[Windows.Media.FaceAnalysis.FaceDetector](https://msdn.microsoft.com/library/windows/apps/windows.media.faceanalysis.facedetector.aspx)

[Windows.Media.FaceAnalysis namespace](https://msdn.microsoft.com/library/windows/apps/windows.media.faceanalysis.aspx)

[Windows.Graphics.Imaging namespace](https://msdn.microsoft.com/library/windows/apps/windows.graphics.imaging.aspx)

[Windows.Storage.Pickers namespace](https://msdn.microsoft.com/library/windows/apps/windows.storage.pickers.aspx)

[Windows.Storage.Streams namespace](https://msdn.microsoft.com/library/windows/apps/windows.storage.streams.aspx)

[Windows.Media.Capture.MediaCapture namespace](https://msdn.microsoft.com/library/windows/apps/windows.media.devices.aspx)

## System requirements

**Hardware:** Camera (for video capture scenario only) 

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++ or C#). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or select Debug > Start Without Debugging. 
