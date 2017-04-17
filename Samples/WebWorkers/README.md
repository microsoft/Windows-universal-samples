<!---
  category: Threading
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620625
--->

# JavaScript Web Workers app multithreading sample

Shows how to use multithreading with Web Workers in your Windows Store app using JavaScript.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Windows Store apps using JavaScript support standards-based Web Workers for multithreaded JavaScript. Web Workers can improve the responsiveness 
of your app by removing tasks from the UI thread. Web Workers use the [postMessage](http://msdn.microsoft.com/library/windows/apps/hh772821) 
function to communicate with the UI thread.

This sample covers the following scenarios:

-   Using two Web Workers to calculate prime numbers. Even though this task is CPU intensive, the UI remains responsive because the calculations run inside the Web Workers.
-   Importing WinJS into a Web Worker to use **XmlHttpRequest** to request a document.
-   Supporting Channel Messaging. Channel Messaging is used to enable direct communication between Web Workers. In this specific scenario, the UI thread creates two workers and a set of ports through which the Web Workers can directly communicate. The UI thread then passes the string "Hello World" to the first Web Worker, which passes it on to the second Web Worker via the specified port, which then passes it back to the UI thread.
-   Accessing the [setTimeout](http://msdn.microsoft.com/library/windows/apps/hh453406) and [setInterval](http://msdn.microsoft.com/library/windows/apps/hh453402) methods defined on a Web Worker. These methods can be used to throttle long running scripts or for other general timing purposes. Click the buttons to instruct the Worker to set a timeout or interval timer. At each callback of the timer, the Web Worker posts messages which are then displayed by the example.

## Operating system requirements

Client

Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

To debug the app and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the app without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.

