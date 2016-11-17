<!---
  category: DeepLinksAndAppToAppCommunication
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=808708
-->

# Remote Systems sample

Shows how to use the [RemoteSystem](https://msdn.microsoft.com/library/windows/apps/Windows.System.RemoteSystems.RemoteSystem.aspx) 
class to discover and interact with other computers.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample shows:

- **Discovery**: Discover a user's systems through proximal connections such as Bluetooth and Wi-Fi
as well as discovery via the cloud.
The systems all must be signed in with the same Microsoft Account (MSA).
- **App Launch:** Use RemoteLauncher to launch applications on a discovered remote system.
- **App Services:** Use App Services with a discovered remote system to send messages between apps running on two systems.

**Note** The App Services scenario connects to the [AppServices Provider](/Samples/AppServices) sample.
In order to run the App Services scenario, the AppServices Provider sample must be installed
on the remote system.

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.

To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## System requirements

**Client:** Windows 10 Anniversary Update

**Server:** Windows Server 2016 Technical Preview w/ Desktop experience pack.

**Phone:** Windows 10 

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio?2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 
