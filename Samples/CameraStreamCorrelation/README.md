<!---
  category: AudioVideoAndCamera 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=825745
--->

# Camera stream correlation sample

Shows how to use spatially correlated color and depth cameras and the depth frames
to map image pixels from one camera to another using a [DepthCorrelatedCoordinateMapper]
(https://msdn.microsoft.com/library/windows/apps/windows.media.devices.core.depthcorrelatedcoordinatemapper.aspx),
as well as decoding a vendor-specific media frame layout with a [BufferMediaFrame]
(https://msdn.microsoft.com/library/windows/apps/Windows.Media.Capture.Frames.BufferMediaFrame.aspx).

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample demonstrates how to:

- Find cameras which support color, depth and pose tracking respectively.
- Create FrameReaders and read frames from multiple sources concurrently.
- Map depth camera's pixels onto correlated color camera.
- Process color and depth frames pixel by pixel to produce a background removal effect.
- Detect if a vendor-specific camera source exist using a vendor-specific sub type. In this sample, we look for a pose tracking stream.
- How to use vendor-specific buffer layout to decode a 1D BufferMediaFrame.
- Overlay skeletal tracking points to color camera coodinate system.

### Correlation of multiple capture sources

Use the DepthCorrelatedCoordinateMapper class to map depth space pixels to color
space pixels. 

Use the camera intrinsics of the color camera to project skeletal
tracking points on top of the color image.

### 1D camera frame with BufferMediaFrame

With new BufferMediaFrame in media capture APIs, cameras can also support 1D media frame format.
Using the "Perception" major media type and a vendor's custom sub media type, a camera
in this sample can expose a PoseTrackingFrame and the app can overlay skeletal tracking points.

###Vendor specific media frame format

This sample also demonstrate how camera vendors can use a WinRT library to wrap vendor-specific
camera frame layout into WinRT class so that apps can easily consume the vendor specific data
with same coding patterns as Windows APIs.

**Note** The Windows universal samples for Windows 10 require Visual Studio 2015 Update 2
and Windows SDK version 14393 or above to build.

To obtain information about Windows 10 development, go to the [Windows Dev Center](https://dev.windows.com).

## See also

### Samples

[CameraFrames](/Samples/CameraFrames)  

### Reference

[Windows.Media.Capture.Frames namespace](https://msdn.microsoft.com/library/windows/apps/windows.media.capture.frames.aspx)  
[Windows.Media.Devices.Core.DepthCorrelatedCoordinateMapper](https://msdn.microsoft.com/library/windows/apps/windows.media.devices.core.depthcorrelatedcoordinatemapper.aspx)  
[Windows.Media.Devices.Core.CameraIntrinsics](https://msdn.microsoft.com/library/windows/apps/windows.media.devices.core.cameraintrinsics.aspx)  

## System requirements

**Client:** Windows 10 build 14393

**Camera:** Correlated color and depth camera (For example a Kinect V2 sensor)

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
  To run the sample without debugging, press Ctrl+F5 or select **Debug** \> **Start Without Debugging**.
