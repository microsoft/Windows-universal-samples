<!---
  category: LaunchingAndBackgroundTasks
  samplefwlink: http://go.microsoft.com/fwlink/?LinkID=761251
--->

# Basic Suspension Sample

Shows how to suspend, shut down and resume your application using the Suspension Manager.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample covers:

- Saving state when the app is suspended or shut down.
- Restoring state when the app is resumed from suspension or reactivated after being shut down.

From the Visual Studio debugging toolbar, use the Lifecycle Events menu to trigger suspend and resume events.

**Note** The Universal Windows app samples require Visual Studio 2015 to build and Windows 10 to execute.

To obtain an insider copy of Windows 10, go to [Windows 10](http://insider.windows.com).

**Note**  For Windows 10 app samples, go to  [Windows 10 Samples](https://github.com/Microsoft/Windows-universal-samples). The samples for Windows 10 can be built and run using Windows developer [tools](https://developer.windows.com).

## Related topics

[Guidelines for app suspend and resume](https://msdn.microsoft.com/library/windows/apps/hh465088.aspx)

[How to trigger suspend, resume, and background events for Windows Store apps in Visual Studio](https://msdn.microsoft.com/library/hh974425.aspx)


## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution.

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or select Debug > Start Without Debugging.
- On the Debug Location toolbar, choose the event that you want to fire: Suspend, Resume, Suspend and Shutdown

