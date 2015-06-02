JavaScript Web Workers app multithreading sample
================================================

This sample demonstrates how to use multithreading with Web Workers in your Windows Store app using JavaScript.

Windows Store apps using JavaScript support standards-based Web Workers for multithreaded JavaScript. Web Workers can improve the responsiveness of your app by removing tasks from the UI thread. Web Workers use the [**postMessage**](http://msdn.microsoft.com/library/windows/apps/hh772821) function to communicate with the UI thread.

This sample covers the following scenarios:

-   Using two Web Workers to calculate prime numbers. Even though this task is CPU intensive, the UI remains responsive because the calculations run inside the Web Workers.
-   Importing WinJS into a Web Worker to use **XmlHttpRequest** to request a document.
-   Supporting Channel Messaging. Channel Messaging is used to enable direct communication between Web Workers. In this specific scenario, the UI thread creates two workers and a set of ports through which the Web Workers can directly communicate. The UI thread then passes the string "Hello World" to the first Web Worker, which passes it on to the second Web Worker via the specified port, which then passes it back to the UI thread.
-   Accessing the [**setTimeout**](http://msdn.microsoft.com/library/windows/apps/hh453406) and [**setInterval**](http://msdn.microsoft.com/library/windows/apps/hh453402) methods defined on a Web Worker. These methods can be used to throttle long running scripts or for other general timing purposes. Click the buttons to instruct the Worker to set a timeout or interval timer. At each callback of the timer, the Web Worker posts messages which are then displayed by the example.

Related topics
--------------

Operating system requirements
-----------------------------

Client

Windows 10

Build the sample
----------------

1.  Start Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2.  Go to the directory in which you unzipped the sample. Go to the directory named for the sample, and double-click the Visual Studio 2015 Solution (.sln) file.
3.  Press F7 or use **Build** \> **Build Solution** to build the sample.

Run the sample
--------------

To debug the app and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the app without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.

