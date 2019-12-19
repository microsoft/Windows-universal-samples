---
page_type: sample
languages:
- cpp
- cppcx
products:
- windows
- windows-uwp
urlFragment: CameraStreamCoordinateMapper
extendedZipContent:
- path: SharedContent
  target: SharedContent
- path: LICENSE
  target: LICENSE
description: "Uses spatially correlated color and depth cameras and the depth frames to map image pixels from one camera to another in real time."
---

<!---
  category: AudioVideoAndCamera 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=870775
--->

# Camera stream coordinate mapper sample

Shows how to use spatially correlated color and depth cameras and the depth frames to map image pixels from one camera to another using [CameraIntrinsics](https://docs.microsoft.com/uwp/api/windows.media.devices.core.cameraintrinsics) and [SpatialCoordinateSystem](https://docs.microsoft.com/uwp/api/windows.perception.spatial.spatialcoordinatesystem) using DirectX and shaders for real time applications.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> You can download this sample as a standalone ZIP file
> [from docs.microsoft.com](https://docs.microsoft.com/samples/microsoft/windows-universal-samples/camerastreamcoordinatemapper/),
> or you can download the entire collection as a single
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample demonstrates how to:

- Find cameras which support color and depth formats.
- Create MediaFrameReaders to read frames from multiple sources concurrently.
- Map depth camera's pixels onto a correlated color camera using DirectX shaders.

### Correlation of multiple capture sources

This sample is intended to demonstrate how to map entire images from one camera to another using depth information from the depth camera, [CameraIntrinsics](https://docs.microsoft.com/uwp/api/windows.media.devices.core.cameraintrinsics), and [SpatialCoordinateSystem](https://docs.microsoft.com/uwp/api/windows.perception.spatial.spatialcoordinatesystem). This is similar to the functionality of the [DepthCorrelatedCoordinateMapper](https://docs.microsoft.com/uwp/api/windows.media.devices.core.depthcorrelatedcoordinatemapper) except this sample is designed for processing an entire image using DirectX and shaders. The [DepthCorrelatedCoordinateMapper](https://docs.microsoft.com/uwp/api/windows.media.devices.core.depthcorrelatedcoordinatemapper) has a member function "MapPoints" which will map an array of pixels from one camera to another using the depth data for correlation, but was not performant on lower-end hardware for a full image of pixels. The performance of the Gpu algorithm in this sample, running on an Xbox One running as a UWP app is < 2ms per frame. Using a desktop PC with a GTX 1060 resulted in < 0.25ms per frame.

There are multiple ways to interpret the result of the image mapping. This sample demonstrates how to visualize the resulting data as a 3D point cloud from different perspectives. This sample also demonstrates how to project the correlated 3D data back onto one of the cameras to correlate the results in 2D (i.e. use the depth data to discard pixels that are too far away from the camera). 

**Note** The Windows universal samples require Visual Studio to build and Windows 10 to execute.

To obtain information about Windows 10 development, go to the [Windows Dev Center](https://dev.windows.com).

## See also

### Samples

* [CameraFrames](/Samples/CameraFrames)
* [CameraStreamCorrelation](/Samples/CameraStreamCorrelation)

### Reference

* [Windows.Media.Capture.Frames namespace](https://docs.microsoft.com/uwp/api/windows.media.capture.frames)
* [Windows.Media.Devices.Core.CameraIntrinsics](https://docs.microsoft.com/uwp/api/windows.media.devices.core.cameraintrinsics)
* [Windows.Perception.Spatial.SpatialCoordinateSystem](https://docs.microsoft.com/uwp/api/windows.perception.spatial.spatialcoordinatesystem)

## System requirements

**Client:** Windows 10 build 15063

**Camera:** Correlated color and depth camera (e.g. Kinect V2 sensor)

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

### Deploying and running the sample

- To debug the sample and then run it, follow the steps listed above to connect your
  developer-unlocked Microsoft HoloLens, then press F5 or select **Debug** \> **Start Debugging**.
  To run the sample without debugging, press Ctrl+F5 or select **Debug** \> **Start Without Debugging**.
