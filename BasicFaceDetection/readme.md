# Basic Face Detection sample

This sample shows how to use the Windows.Media.FaceAnalysis.FaceDetector WinRT class to find human faces within an image.

Specifically, this sample shows how to:

- Select, open, and read an image file (jpeg, bmp, or png)
- Start a live webcam capture and take snap a frame from the stream
- Convert image data from the source medium into a SoftwareBitmap that's compatible with FaceDetector
- Initialize and execute FaceDetector
- Retrieve FaceDetector results and visualize them

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10, go to [Windows 10](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

The FaceDetector only operates on a static image or a single frame of video. In order to track human faces in real-time, either through a live stream or a video clip, the FaceTracker API must be used instead.

### Samples

[Sample 1](https://github.com/Microsoft/Windows-universal-samples/tree/master/<sample>)

### Reference

<!-- Add links to related API -->

[API 1](http://msdn.microsoft.com/library/windows/apps/)

[API 2](http://msdn.microsoft.com/library/windows/apps/)

## System requirements

**Client:** Windows 10 Technical Preview

**Server:** Windows 10 Technical Preview

**Phone:**  Windows 10 Technical Preview

## Build the sample

1. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2. Go to the directory to which you unzipped the sample. Then go to the subdirectory containing the sample in the language you desire - either C++, C#, or JavaScript. Double-click the Visual Studio 2015 Solution (.sln) file. 
3. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**. 

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 
