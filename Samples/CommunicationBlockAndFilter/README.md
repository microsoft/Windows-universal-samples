<!---
  category: Communications
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=624164
--->

# Communication blocking and filtering sample

Shows how to make a block and filter application for call and text message.
Uses the
[CommunicationBlockingAppManager](https://msdn.microsoft.com/library/windows/apps/windows.applicationmodel.communicationblocking.communicationblockingappmanager.aspx)
class to set itself as the active communication blocking app and uses the
[PhoneCallBlocking](https://msdn.microsoft.com/library/windows/apps/windows.applicationmodel.calls.phonecallblocking.aspx) class
to set the phone numbers which are blocked.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample demonstrates how to

- Register background tasks for blocking and filtering calls and text messages.
  - A task which runs when the session starts. The task sets the blocked number list.
  - A task which runs when the app becomes the active communication blocking app. The task sets the blocked number list.
  - A task which runs when a call is blocked.
  - A task which runs when a text message is blocked.
  - A task which runs when a text message is received. The task evaluates the text message and decide whether to block it.
- Determine whether the app is the the active communication blocking app.
- Display the system UI to let the user choose a communication blocking app.
- Set the phone numbers which are blocked.

See the Package.appxmanifest file for the extensions and capabilities
a communication blocking and filtering app must declare.

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Reference

[Windows.ApplicationModel.CommunicationBlocking namespace](https://msdn.microsoft.com/library/windows/apps/windows.applicationmodel.communicationblocking.aspx)  
[Windows.ApplicationModel.Calls namespace](https://msdn.microsoft.com/library/windows/apps/windows.applicationmodel.calls.aspx)  
[CommunicationBlockingAppManager class](https://msdn.microsoft.com/library/windows/apps/windows.applicationmodel.communicationblocking.communicationblockingappmanager.aspx)  
[PhoneCallBlocking class](https://msdn.microsoft.com/library/windows/apps/windows.applicationmodel.calls.phonecallblocking.aspx)  
[SystemTrigger class](https://msdn.microsoft.com/library/windows/apps/windows.applicationmodel.background.systemtrigger.aspx)  
[CommunicationBlockingAppSetAsActiveTrigger class](https://msdn.microsoft.com/library/windows/apps/windows.applicationmodel.background.communicationblockingappsetasactivetrigger.aspx)  
[PhoneTrigger class](https://msdn.microsoft.com/library/windows/apps/windows.applicationmodel.background.phonetrigger.aspx)  
[ChatMessageNotificationTrigger class](https://msdn.microsoft.com/library/windows/apps/windows.applicationmodel.background.chatmessagenotificationtrigger.aspx)  
[ChatMessageReceivedNotificationTrigger class](https://msdn.microsoft.com/library/windows/apps/windows.applicationmodel.background.chatmessagereceivednotificationtrigger.aspx)  

## System requirements

**Client:** Not supported

**Server:** Not supported

**Phone:**  Windows 10

## Build the sample

1.  Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2.  Go to the directory to which you unzipped the sample. Then go to the subdirectory containing the sample in the C\# language. Double-click the Visual Studio 2015 Solution (.sln) file.
3.  Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution.

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging.
