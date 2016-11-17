<!---
  category: Communications
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620600
--->

# Real-time communication sample

Shows how to use the low latency feature to enable real-time communication applications.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample contains:

-   A simple end-to-end video call client that demonstrates the low latency mode of the Windows Runtime capture engine. 
This is enabled using the [msRealTime](http://msdn.microsoft.com/library/windows/apps/hh767377) the 
[video](http://msdn.microsoft.com/library/windows/apps/hh767390) tag or [RealTimePlayback](http://msdn.microsoft.com/library/windows/apps/br227414) 
on the [MediaElement](http://msdn.microsoft.com/library/windows/apps/br242926). The sample uses a custom network source and a custom sink extension to 
send and receive captured audio and video data between two computers.

-   A demonstration of the end-to-end latency of video captured using the [Media Capture](http://msdn.microsoft.com/library/windows/apps/br226738) API 
and displayed using a [video](http://msdn.microsoft.com/library/windows/apps/hh767390) and [MediaElement](http://msdn.microsoft.com/library/windows/apps/br242926) 
with low latency mode enabled. Two output windows are displayed. The first shows a camera preview window of the raw output from your camera. The second is a local host 
client window that shows the video from the camera when compressed, streamed, and received over machine's loopback network interface. This window demonstrates the 
end-to-end latency of video captured, streamed to, and displayed by a remote client minus network latency.

**Important**  

This sample uses the Media Extension feature of Windows 8.1 to add functionality to the Microsoft Media Foundation pipeline. A Media Extension consists of a 
hybrid object that implements both Component Object Model (COM) and Windows Runtime interfaces. The COM interfaces interact with the Media Foundation pipeline. 
The Windows Runtime interfaces activate the component and interact with the Windows Store app.

In most situations, it is recommended that you use Visual C++ with Component Extensions (C++/CX ) to interact with the Windows Runtime. But in the case of hybrid 
components that implement both COM and Windows Runtime interfaces, such as Media Extensions, this is not possible. C++/CX can only create Windows Runtime objects. 
So, for hybrid objects it is recommended that you use [Windows Runtime C++ Template Library](http://go.microsoft.com/fwlink/p/?linkid=243149) to interact with the 
Windows Runtime. Be aware that Windows Runtime C++ Template Library has limited support for implementing COM interfaces.

For more info on creating a Media Foundation media extension in Windows Store app, see Walkthrough: 
[Creating a Windows Store app using WRL and Media Foundation](http://go.microsoft.com/fwlink/p/?LinkID=309355) and the 
[Media extension sample](http://go.microsoft.com/fwlink/p/?linkid=241427).

**Important**  The binaries used by this sample have been included for proof of concept purposes only. They might have significant performance, reliability, 
and security issues and should not be used outside of a test environment. They are not licensed for use in a production environment or for use with sensitive data.

**Important**  The URL passed to the code is not validated or authenticated. The application must perform these actions.

To obtain an evaluation copy of Windows 8.1, go to [Windows 8.1](http://go.microsoft.com/fwlink/p/?linkid=301696).

To obtain an evaluation copy of Microsoft Visual Studio 2013, go to [Visual Studio 2013](http://go.microsoft.com/fwlink/p/?linkid=301697).

Related topics
--------------

[Windows 8 app samples](http://go.microsoft.com/fwlink/p/?LinkID=227694)

**Roadmaps**

[Roadmap for apps using C\# and Visual Basic](http://msdn.microsoft.com/library/windows/apps/br229583)  
[Roadmap for apps using C++](http://msdn.microsoft.com/library/windows/apps/hh700360)  
[Roadmap for apps using JavaScript](http://msdn.microsoft.com/library/windows/apps/hh465037)  
[Designing UX for apps](http://msdn.microsoft.com/library/windows/apps/hh767284)  
[Audio, video, and camera](https://msdn.microsoft.com/library/windows/apps/mt203788)  

**Tasks**

[How to enable low-latency playback](http://msdn.microsoft.com/library/windows/apps/hh452742)  
[Creating a Windows Store app using WRL and Media Foundation](http://go.microsoft.com/fwlink/p/?LinkID=309355)  

**Reference**

[Windows.Media.Capture.MediaCapture](http://msdn.microsoft.com/library/windows/apps/br226738)  
[MediaCapture](http://msdn.microsoft.com/library/windows/apps/br241124)  
[msRealTime](http://msdn.microsoft.com/library/windows/apps/hh767377)  
[MediaExtensionManager](http://msdn.microsoft.com/library/windows/apps/br240987)  
[Windows.Media.MediaProperties](http://msdn.microsoft.com/library/windows/apps/hh701296)  

Operating system requirements
-----------------------------

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

Build the sample
----------------

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

Run the sample
--------------

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

**Deploying the sample**

- Select Build > Deploy Solution. 

**Deploying and running the sample**

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or select Debug > Start Without Debugging.
