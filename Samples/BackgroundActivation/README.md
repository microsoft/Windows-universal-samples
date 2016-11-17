<!---
  category: LaunchingAndBackgroundTasks 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=798686
--->

# Background activation sample

Shows how to create and register background tasks that will run in the main process using the Windows Runtime background task API.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

**Note** This Universal Windows app sample requires Visual Studio 2015 to build and Windows 10 Anniversay Update to execute.

A background task is triggered by a system or time event and can be constrained by one or more conditions. When a background task is triggered, OnBackgroundActivated is invoked and performs the work of the background task. A background task can run even when the app that registered the background task is suspended or not running.

This sample demonstrates the following:

-   Creating and registering background tasks for the Single Process Model written in C++, C\#, or Visual Basic.
-   Creating a background task that is triggered by a system event.
-   Requesting access to run in the background.
-   Creating a background task that is triggered by a time trigger.
-   Adding a condition that constrains the background task to run only when the condition is in effect.
-   Creating and signaling an application trigger.
-   Reporting background task progress and completion to the app.
-   Using a deferral object to include asynchronous code in your background task.
-   Handling the cancellation of a background task, and ensuring the task is cancelled when required conditions are no longer met.
-   Initializing background task progress and completion handlers when the app is launched.

This sample uses the Single Process Model method for background activity. Applications can also use the Multiple Process Model method for running Background Tasks in a separate process from the foreground application.

## Related topics

### Samples

[Background Tasks](/Samples/BackgroundTasks)  
[Traffic App sample](https://github.com/microsoft/windows-appsample-trafficapp/)  

### Reference

[Launching, resuming, and multitasking](http://msdn.microsoft.com/library/windows/apps/hh770837)  
[Managing background tasks](http://msdn.microsoft.com/library/windows/apps/hh977053)  
[Support your app with background tasks](https://msdn.microsoft.com/library/windows/apps/mt299103)  

**API reference**

[Windows.ApplicationModel.Background (XAML)](http://msdn.microsoft.com/library/windows/apps/br224847)  
[Windows.UI.WebUI.WebUIBackgroundTaskInstance (HTML)](http://msdn.microsoft.com/library/windows/apps/hh701740)  

### Related technologies

[Windows.ApplicationModel.Background](http://msdn.microsoft.com/library/windows/apps/br224847)  

## Operating system requirements

Client

Windows 10 Anniversary Update

Server

None supported

Phone

Windows Phone 10 Anniversary Update

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

**Deploying the sample**

1.  Select **Build** \> **Deploy Solution**.

**Deploying and running the sample**

1.  To debug the sample and then run it, press F5 or select **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or select**Debug** \> **Start Without Debugging**.

**Triggering background tasks**

To trigger the background tasks associated with the TimeZoneChange event:

1.  Change date and time settings.
2.  Click **Change time zone...**
3.  Select a time zone that has a UTC offset different from the currently selected time zone.
4.  Click **OK**.

**Note**  The minimum delay for creating TimeTrigger events is 15 minutes. The first timer event, however, might not occur until 15 minutes after it is expected to expire (30 minutes after the app registers the event).

## Read more

See the following topics for step-by-step information about using background tasks:

-   [Create and register a background task](https://msdn.microsoft.com/library/windows/apps/mt299100)
-   [Respond to system events with background tasks](https://msdn.microsoft.com/library/windows/apps/mt185414)
-   [Set conditions for running a background task](https://msdn.microsoft.com/library/windows/apps/mt185620)
-   [Handle a cancelled background task](https://msdn.microsoft.com/library/windows/apps/mt187312)
-   [Monitor background task progress and completion](https://msdn.microsoft.com/library/windows/apps/mt186457)
-   [Run a background task on a timer ](https://msdn.microsoft.com/library/windows/apps/mt186458)
-   [Use a maintenance trigger](https://msdn.microsoft.com/library/windows/apps/mt185632)
-   [Declare background tasks in the application manifest](https://msdn.microsoft.com/library/windows/apps/mt185412)
-   [Guidelines for background tasks](https://msdn.microsoft.com/library/windows/apps/mt187310)
-   [Debug a background task](https://msdn.microsoft.com/library/windows/apps/mt299101)

