<!---
  category: AppSettings
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620486
--->

# Application data sample

Shows how to store and retrieve data that is specific to each user and app by using the Windows Runtime application data APIs 
([Windows.Storage.ApplicationData](http://msdn.microsoft.com/library/windows/apps/br241587) and so on). 

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Application data includes session state, user preferences, and other settings. It is created, read, updated, and deleted when the app is running. The operating system manages these data stores for your app: 

- local: Data that exists on the current device and is backed up in the cloud 
- roaming: Data that exists on all devices on which the user has installed the app 
- temporary: Data that could be removed by the system any time the app isn't running 
- localcache: Persistent data that exists only on the current device 

If you use roaming data in your app, your users can easily keep your app's application data in sync across multiple devices. The operating system replicates roaming data to the cloud when it is updated, and synchronizes the data to any other devices on which the app is installed, reducing the amount of setup work that the user needs to do to install your app on multiple devices. 

If you use local data in your app, your users can back up application data in the cloud. This application data can then be restored back on any other device while setting up the device with the same account.

The sample covers these key tasks:

- Reading and writing settings to an app data store 
- Reading and writing files to an app data store 
- Responding to roaming events 

## Guidelines 

[Guidelines for roaming application data](http://msdn.microsoft.com/library/windows/apps/hh465094)  

## Concepts 

[Store and retrieve settings and other app data](https://msdn.microsoft.com/library/windows/apps/mt299098)  

## Reference 

[Windows.Storage.ApplicationData](http://msdn.microsoft.com/library/windows/apps/br241587)  
[Windows.Storage.ApplicationDataCompositeValue](http://msdn.microsoft.com/library/windows/apps/br241588)  
[Windows.Storage.ApplicationDataContainer](http://msdn.microsoft.com/library/windows/apps/br241599)  
[Windows.Storage.ApplicationDataContainerSettings](http://msdn.microsoft.com/library/windows/apps/br241600)  
[WinJS.Application](http://msdn.microsoft.com/library/windows/apps/br229774)  

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

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 

