<!---
  category: AudioVideoAndCamera
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620563
--->

# DASH streaming sample

How to create and use the MSEStreamSource API to playback Live DASH content.

Live DASH will be supported at a later date within the Adaptive Streaming classes (see AdaptiveStreaming sample).

This sample only supports number-based DASH manifests that are dynamic with a is-offlive DASH profile. 

Specifically, this sample covers:

-   Parsing a Live DASH manifest
-   Downloading segments for streaming
-   Using MSEStreamSource to playback those segments

Related topics
--------------

MediaStreamSource Sample (https://code.msdn.microsoft.com/windowsapps/MediaStreamSource-media-dfd55dff)

[Windows.Media.Core namespace] (https://msdn.microsoft.com/en-us/library/windows/apps/windows.media.core.msesourcebuffer.aspx)


System requirements
-----------------------------

**Client:** Windows 10

Build the sample
----------------

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

Run the sample
--------------

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

**Deploying the sample**
1.  Select **Build** \> **Deploy Solution**.

**Deploying and running the sample**
1.  To debug the sample and then run it, press F5 or select **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or select**Debug** \> **Start Without Debugging**.




