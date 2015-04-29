Background task sample
======================

This sample shows you how to create and register background tasks using the Windows Runtime background task API.

**Note** The Universal Windows app samples require Visual Studio 2015 to build and Windows 10 to execute.

A background task is triggered by a system or time event and can be constrained by one or more conditions. When a background task is triggered, its associated handler runs and performs the work of the background task. A background task can run even when the app that registered the background task is suspended.

This sample demonstrates the following:

-   Creating and registering background tasks written in C++, C\#, or JavaScript.
-   Creating a background task that is triggered by a system event.
-   Requesting the user's permission to add the app to the lock screen.
-   Creating a background task that is triggered by a time trigger.
-   Adding a condition that constrains the background task to run only when the condition is in effect.
-   Creating and signaling an application trigger.
-   Reporting background task progress and completion to the app.
-   Using a deferral object to include asynchronous code in your background task.
-   Handling the cancellation of a background task, and ensuring the task is cancelled when required conditions are no longer met.
-   Initializing background task progress and completion handlers when the app is launched.

To obtain an evaluation copy of Windows 8.1, go to [Windows 8.1](http://go.microsoft.com/fwlink/p/?linkid=301696).

To obtain an evaluation copy of Microsoft Visual Studio 2013, go to [Visual Studio 2013](http://go.microsoft.com/fwlink/p/?linkid=301697).

**Note**  For Windows 8 app samples, download the [Windows 8 app samples pack](http://go.microsoft.com/fwlink/p/?LinkId=301698). The samples in the Windows 8 app samples pack will build and run only on Microsoft Visual Studio 2012.

Related topics
--------------

****

[Displaying tiles on the lock screen](http://msdn.microsoft.com/library/windows/apps/hh868260)

[Launching, resuming, and multitasking](http://msdn.microsoft.com/library/windows/apps/hh770837)

[Managing background tasks](http://msdn.microsoft.com/library/windows/apps/hh977053)

[Supporting your app with background tasks](http://msdn.microsoft.com/library/windows/apps/hh977056)

[Windows 8 app samples](http://go.microsoft.com/fwlink/p/?LinkID=227694)

**API reference**

[**Windows.ApplicationModel.Background (XAML)**](http://msdn.microsoft.com/library/windows/apps/br224847)

[**Windows.UI.WebUI.WebUIBackgroundTaskInstance (HTML)**](http://msdn.microsoft.com/library/windows/apps/hh701740)

Related technologies
--------------------

[**Windows.ApplicationModel.Background**](http://msdn.microsoft.com/library/windows/apps/br224847)

Operating system requirements
-----------------------------

Client

Windows 10

Server

None supported

Phone

Windows Phone 10

Build the sample
----------------

1.  Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2.  Go to the directory to which you unzipped the sample. Then go to the subdirectory named for the sample and double-click the Visual Studio 2015 Solution (.sln) file.
3.  Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

Run the sample
--------------

**Deploying the sample**

1.  Select **Build** \> **Deploy Solution**.

**Deploying and running the sample**

1.  To debug the sample and then run it, press F5 or select **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or select**Debug** \> **Start Without Debugging**.

**Triggering background tasks**

To trigger the background tasks associated with the `TimeZoneChange` event:

1.  Change date and time settings.
2.  Click **Change time zone...**
3.  Select a time zone that has a UTC offset different from the currently selected time zone.
4.  Click **OK**.

Background tasks associated with the `TimeTrigger` event will only fire if the app is currently on the lock screen. There are two ways to accomplish this.

Accept the initial request to add the BackgroundTaskSample app to the lock screen:

1.  Launch the BackgroundTaskSample app for the first time.
2.  Register the TimeTrigger event.
3.  Accept the request to add the BackgroundTaskSample app to the lock screen.

Add the BackgroundTaskSample app to the lock screen manually:

1.  From the Start screen, go to **Settings** \> **Customize your lock screen**.
2.  Choose the BackgroundTaskSample app for the lock screen.
3.  Launch the BackgroundTaskSample app and register the TimeTrigger event.

**Note**  The minimum delay for creating TimeTrigger events is 15 minutes. The first timer event, however, might not occur until 15 minutes after it is expected to expire (30 minutes after the app registers the event).

Read more
---------

See the following topics for step-by-step information about using background tasks:

-   [Quickstart: Create and register a background task](http://msdn.microsoft.com/library/windows/apps/hh977055)
-   [How to respond to system events with background tasks](http://msdn.microsoft.com/library/windows/apps/hh977058)
-   [How to set conditions for running a background task](http://msdn.microsoft.com/library/windows/apps/hh977057)
-   [How to handle a cancelled background task](http://msdn.microsoft.com/library/windows/apps/hh977052)
-   [How to monitor background task progress and completion](http://msdn.microsoft.com/library/windows/apps/hh977054)
-   [How to run a background task on a timer](http://msdn.microsoft.com/library/windows/apps/hh977059)
-   [How to use maintenance triggers](http://msdn.microsoft.com/library/windows/apps/jj883699)
-   [How to declare background tasks in the application manifest](http://msdn.microsoft.com/library/windows/apps/hh977049)
-   [Guidelines and checklists for background tasks](http://msdn.microsoft.com/library/windows/apps/hh977051)
-   [**How to debug a background task**](http://msdn.microsoft.com/library/windows/apps/jj542416)

