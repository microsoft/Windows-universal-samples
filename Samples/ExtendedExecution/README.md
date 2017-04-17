<!---
  category: LaunchingAndBackgroundTasks
  samplefwlink:  http://go.microsoft.com/fwlink/?LinkId=723509
--->

# Extended execution sample

Shows how to create extended execution sessions using the Windows Runtime extended execution API.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

An app requests an extended execution session when a task requires time to complete before the application is suspended or terminated.
Different types of extended execution can be requested during the Suspending state or the Resumed state of the application.

This sample demonstrates the following:

-   Creating an extended execution session to extend suspending time and complete saving data.
-   Creating an extended execution session to extend foreground time and continue location tracking.
-   Creating an extended execution session to extend foreground time and continue an unspecified task.
-   Sharing an extended execution session among multiple tasks.
-   Handling a denied request for extended execution.
-   Handling the revocation of an extended execution session.

See **Deploying and running the sample** below for instructions on using the sample.

## Related topics

### Samples

[Background audio](/Samples/BackgroundAudio)  
[Geolocation](/Samples/Geolocation)  

### Other resources

[Background Tasks and Extended Execution](https://msdn.microsoft.com/magazine/mt590969)  
[Launching, resuming, and background tasks](https://msdn.microsoft.com/library/windows/apps/xaml/mt227652.aspx)  
[Support your app with background tasks](https://msdn.microsoft.com/library/windows/apps/mt299103)  

### Reference

[Windows.ApplicationModel.ExtendedExecution](https://msdn.microsoft.com/library/windows/apps/windows.applicationmodel.extendedexecution.aspx)  

### Related technologies

[Windows.ApplicationModel.Background](http://msdn.microsoft.com/library/windows/apps/br224847)  

## Operating system requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

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
Since the system will not suspend an app that is being debugged,
some scenarios require you to run the sample without debugging.

**Unspecified Extended Execution**:

1. Run the sample without debugging and go to the **Unspecified Reason** scenario.
2. Click **Begin Extended Execution**.
3. Send the program to the background:
   On Phone, switch to another app.
   On PC, minimize the app.
4. The app continues to display toast notifications.
5. Bring the app back to the foreground and the session will continue running.
6. Run the sample with debugging.
7. Repeat steps 2 through 4 above.
8. Use the Lifecycle events menu in the debugger to suspend the app.
   This simulates a revoke due to SystemPolicy.

**Saving Data Extended Execution**:

1. Run the sample without debugging and go to the **Saving Data Reason** scenario.
2. Send the program to the background:
   On Phone, switch to another app.
   On PC, minimize the app.
3. The app displays toast notifications while the save operation proceeds,
   demonstrating that the app was given a longer period of time to save data while suspending.

**Location Tracking Extended Execution**:

1. Run the sample without debugging and go to the **Location Tracking Reason** scenario.
2. Click **Begin Extended Execution**.
3. Send the program to the background:
   On Phone, switch to another app.
   On PC, minimize the app.
4. The app continues to report your location every 10 seconds.
5. Bring the app back to the foreground and the session will continue running.
6. Run the sample with debugging.
7. Repeat steps 2 through 4 above.
8. Use the Lifecycle events menu in the debugger to suspend the app.
   This simulates a revoke due to SystemPolicy.

**Using Multiple Tasks**:

1. Run the sample without debugging and go to the **Using Multiple Tasks** scenario.
2. Click **Begin Extended Execution and Tasks**.
3. Send the program to the background:
   On Phone, switch to another app.
   On PC, minimize the app.
4. The app displays toast notifications as each of three Tasks complete.
5. Bring the app back to the foreground and the session will continue running.
6. Run the sample with debugging.
7. Repeat steps 2 through 4 above.
8. Use the Lifecycle events menu in the debugger to suspend the app.
   This simulates a revoke due to SystemPolicy.
