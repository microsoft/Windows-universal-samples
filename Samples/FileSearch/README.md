<!---
  category: FilesFoldersAndLibraries
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620544
--->

# File search sample

Shows how to query files in a location (like a folder, library, device, or network location). 
Uses the [Windows.Storage.Search](http://msdn.microsoft.com/library/windows/apps/br208106) API.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Important APIs in this sample include:

-   [QueryOptions](http://msdn.microsoft.com/library/windows/apps/br207995) class
-   [StorageFileQueryResult](http://msdn.microsoft.com/library/windows/apps/br208046) class
-   [StorageFolder](http://msdn.microsoft.com/library/windows/apps/br227230).[CreateFileQueryWithOptions](http://msdn.microsoft.com/library/windows/apps/br211591) method

For a walkthrough, see [Quickstart: Accessing files programmatically](http://msdn.microsoft.com/library/windows/apps/jj150596).

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples

[File access sample](http://go.microsoft.com/fwlink/p/?linkid=231445)  
[Folder enumeration sample](http://go.microsoft.com/fwlink/p/?linkid=231512)  

### Reference

[Windows.Storage namespace](http://msdn.microsoft.com/library/windows/apps/br227346)  
[Windows.Storage.Search namespace](http://msdn.microsoft.com/library/windows/apps/br208106)  

## System requirements

**Client:** Windows 10 build 10500 or higher

**Server:** Windows Server 2016 Technical Preview build 10500 or higher

**Phone:** Windows 10 build 10500 or higher

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
