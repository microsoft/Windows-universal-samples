<!---
  category: Holographic
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=824113
--->

# Holographic face tracking sample

Shows how to acquire video frames from the photo/video (PV) camera and use the FaceAnalysis
API to determine if there are any faces in front of the HoloLens. We display a cube on top of the
detected face and a video stream inside the user's view.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

If multiple faces are detected, then the sample will pick the face closest to the center of the
user's gaze. If no faces are detected then the text "No faces detected" is displayed.

This sample uses the webcam and microphone app capability in order to access the MediaCapture
object and stream video images to the FaceAnalysis API and to the user.

### FaceAnalysis and HoloLens

The FaceTracker class processes images and returns a list of rectangles inside the image where the
faces are detected. This is a very intensive process to run in real-time, and could reduce rendering
performance if it were to slow down the main render thread. It is very important that Holographic
applications maintain 60fps for rendering to reduce user discomfort.

In this sample, we are rendering holograms at 60fps, receiving video frames at 30fps, and running
the FaceTracker as fast as it can process new frames (about 10-15fps). In order to keep rendering
performance at 60fps, we run the FaceAnalysis on an explicit background thread. This adds a bit of
complexity to sychronize between the FaceAnalysis worker thread and the main render thread.

### Handling NV12 images

The HoloLens PV camera provides NV12 video images by default, which can't be directly
used in the shaders for rendering to a RGB backbuffer. Converting the full image from NV12 to RGB
is possible, but it's an expensive operation. We only need to render the portion of the frame where
the face was detected. So instead of converting the full image, we can convert NV12 to RGB in the
pixel shader itself.

NV12 is a 8-bit YUV format for video rendering. To obtain more information about YUV and video formats,
see [Recommended 8-Bit YUV Formats for Video Rendering](https://msdn.microsoft.com/library/windows/desktop/dd206750.aspx).

### Additional remarks

**Note** The Windows universal samples for Windows 10 Holographic require Visual Studio 2015 Update 3
to build, and a Windows Holographic device to execute. Windows Holographic devices include the
Microsoft HoloLens and the Microsoft HoloLens Emulator.

To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421).

To obtain information about the tools used for Windows Holographic development, including
Microsoft Visual Studio 2015 Update 3 and the Microsoft HoloLens Emulator, go to
[Install the tools](https://developer.microsoft.com/windows/holographic/install_the_tools).

## Related topics

### Samples

[BasicFaceDetection](/Samples/BasicFaceDetection)

[BasicFaceTracking](/Samples/BasicFaceTracking)

[CameraFrames](/Sample/CameraFrames)

### Reference

The following types are used in this code sample:
* [Windows.Media.Capture namespace](https://msdn.microsoft.com/library/windows/apps/windows.media.capture.aspx)
  * [MediaCapture class](https://msdn.microsoft.com/library/windows/apps/windows.media.capture.mediacapture.aspx)
* [Windows.Media.Capture.Frames namespace](https://msdn.microsoft.com/library/windows/apps/windows.media.capture.frames.aspx)
  * [MediaFrameReference class](https://msdn.microsoft.com/library/windows/apps/windows.media.capture.frames.mediaframereference.aspx)
  * [MediaFrameReference.CoordinateSystem property](https://msdn.microsoft.com/library/windows/apps/windows.media.capture.frames.mediaframereference.coordinatesystem.aspx)
  * [MediaFrameSource class](https://msdn.microsoft.com/library/windows/apps/windows.media.capture.frames.mediaframesource.aspx)
  * [VideoMediaFrame class](https://msdn.microsoft.com/library/windows/apps/windows.media.capture.frames.videomediaframe.aspx)
  * [VideoMediaFrame.CameraIntrinsics](https://msdn.microsoft.com/library/windows/apps/windows.media.capture.frames.videomediaframe.cameraintrinsics.aspx)
* [Windows.Media.Devices.Core namespace](https://msdn.microsoft.com/library/windows/apps/windows.media.devices.core.aspx)
  * [CameraIntrinsics class](https://msdn.microsoft.com/library/windows/apps/windows.media.devices.core.cameraintrinsics.aspx)
  * [CameraIntrinsics.UnprojectAtUnitDepth method](https://msdn.microsoft.com/library/windows/apps/windows.media.devices.core.cameraintrinsics.unprojectatunitdepth.aspx)
* [Windows.Media.FaceAnalysis namespace](https://msdn.microsoft.com/library/windows/apps/windows.media.faceanalysis.aspx)
  * [FaceTracker class](https://msdn.microsoft.com/library/windows/apps/windows.media.faceanalysis.facetracker.aspx)
  * [FaceTracker.ProcessNextFrameAsync method](https://msdn.microsoft.com/library/windows/apps/windows.media.faceanalysis.facetracker.processnextframeasync.aspx)

## System requirements

**Client:** Windows 10 Holographic build 14393

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
  To run  the sample without debugging, press Ctrl+F5 or select **Debug** \> **Start Without Debugging**.
