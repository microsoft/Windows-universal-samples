<!---
  category: FilesFoldersAndLibraries
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620543
--->

# File picker provider sample

Shows how an app can provide files, a save location, and real-time file updates to other apps through the 
file picker by participating in the File Open Picker contract, File Save Picker contract, and Cached File 
Updater contract, respectively. This sample uses the [Windows.Storage.Pickers.Provider](http://msdn.microsoft.com/library/windows/apps/br207954) and 
[Windows.Storage.Provider](http://msdn.microsoft.com/library/windows/apps/hh747812) APIs. 

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

The sample demonstrates these tasks:

1. Provide files by integrating with the File Open Picker contract
Uses the following API:

- JavaScript: [WebUIFileOpenPickerActivatedEventArgs](http://msdn.microsoft.com/library/windows/apps/hh701800) class
C#/C++/VB: [FileOpenPickerActivatedEventArgs](http://msdn.microsoft.com/library/windows/apps/hh700467) class method 
- [FileOpenPickerUI](http://msdn.microsoft.com/library/windows/apps/hh738453) class 


2. Provide a save location by integrating with the File Save Picker contract
Uses the following API:

- JavaScript: [WebUIFileSavePickerActivatedEventArgs](http://msdn.microsoft.com/library/windows/apps/hh701822) class
C#/C++/VB: [FileSavePickerActivatedEventArgs](http://msdn.microsoft.com/library/windows/apps/hh700489) class method 
- [FileSavePickerUI](http://msdn.microsoft.com/library/windows/apps/hh738463) class 


3. Provide real-time file updates by integrating with the Cached File Updater contract
Uses the following API:

- JavaScript: [WebUICachedFileUpdaterActivatedEventArgs](http://msdn.microsoft.com/library/windows/apps/hh701752) class
C#/C++/VB: [CachedFileUpdaterActivatedEventArgs](http://msdn.microsoft.com/library/windows/apps/hh7004400 class method 
- [CachedFileUpdaterUI](http://msdn.microsoft.com/library/windows/apps/hh747794) class 
- [CachedFileUpdater](http://msdn.microsoft.com/library/windows/apps/hh747793) class 

**Note** This scenario requires the File picker sample.
To learn more about integrating with file picker contracts, see [Quickstart: Integrating with file picker contracts](http://msdn.microsoft.com/library/windows/apps/hh465192) 
and [Guidelines and checklist for file picker contracts](http://msdn.microsoft.com/library/windows/apps/jj150594).

## Related technologies

[Windows.Storage.Pickers namespace](http://msdn.microsoft.com/library/windows/apps/br207928)  
[Windows.Storage.Pickers.Provider namespace](http://msdn.microsoft.com/library/windows/apps/br207954)  

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

