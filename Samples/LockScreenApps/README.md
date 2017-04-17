<!---
  category: ControlsLayoutAndText 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620564
--->

# Lock screen apps sample

Shows how an app can have a presence on the lock screen—the screen that is shown when the computer is locked—with a badge to provide basic status information 
or a tile to provide more detailed status. An app can also send toast notifications to the lock screen. All apps that are granted a lock screen presence also 
have the ability to perform background tasks. 

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

The sample demonstrates the following scenarios:

- Requesting lock screen access for the app 
- Querying for the current lock screen access status of the app 
- Removing the app's lock screen access at its request 
- Sending a badge update notification to an app's badge on the lock screen 
- Sending a text-only tile update notification to an app's tile on the lock screen 
- Using secondary tiles with the lock screen 

The app that has a lock screen presence must declare one or more of the following types of background tasks:

- Control channel
- Timer
- Push notification

In Microsoft Visual Studio 2013, this value is set in the **Declarations** page of the manifest editor, which sets the [BackgroundTasks](http://msdn.microsoft.com/library/windows/apps/br211421) element in the package.appxmanifest file. This value has been set for this sample.

For an app to send a toast notification, the developer must have declared that the app is toast-capable in its app manifest file (package.appxmanifest) as has been done in this sample app. Normally, you do this by using the Visual Studio 2013 manifest editor, where you'll find the setting in the **Application UI** tab, under the **Notifications** section. For more info, see [How to opt in for toast notifications](http://msdn.microsoft.com/library/windows/apps/hh781238).


To obtain an insider copy of Windows 10, go to [Windows 10](http://insider.windows.com). 

**Note**  For Windows 10 app samples, go to  [Windows 10 Samples](https://github.com/Microsoft/Windows-universal-samples). The samples for Windows 10 can be built and run using Windows developer [tools](https://developer.windows.com).

## Related topics

[Guidelines and checklist for tiles and badges](http://msdn.microsoft.com/library/windows/apps/hh465403)  
[Quickstart: Showing tile and badge updates on the lock screen](http://msdn.microsoft.com/library/windows/apps/hh700416)  
[Lock screen overview](http://msdn.microsoft.com/library/windows/apps/hh779720)  
[Windows 8 Windows Store app samples](http://go.microsoft.com/fwlink/p/?LinkID=227694)  

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or select Debug > Start Without Debugging. 
