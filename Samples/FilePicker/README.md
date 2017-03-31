<!---
  category: FilesFoldersAndLibraries
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=619994
--->

# File picker sample

Shows how to access files and folders by letting the user choose them through the file pickers and 
how to save a file so that the user can specify the name, file type, and location of a file to save. 
This sample uses the [Windows.Storage.Pickers](http://msdn.microsoft.com/library/windows/apps/br207928) and 
[Windows.Storage](http://msdn.microsoft.com/library/windows/apps/br227346) APIs.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

The sample demonstrates these tasks:

1.  **Let the user pick one file to access**

    Uses the [FileOpenPicker](http://msdn.microsoft.com/library/windows/apps/br207847).[PickSingleFileAsync](http://msdn.microsoft.com/library/windows/apps/br207852) method (or [PickSingleFileAndContinue](http://msdn.microsoft.com/library/windows/apps/dn652521)) to call a file picker window and let the user pick a single file. For more info, see [Open files and folders with a picker](https://msdn.microsoft.com/library/windows/apps/mt186456).

2.  **Let the user pick multiple files to access**

    Uses the [FileOpenPicker](http://msdn.microsoft.com/library/windows/apps/br207847).[PickMultipleFilesAsync](http://msdn.microsoft.com/library/windows/apps/br207851) method (or [PickMultipleFilesAndContinue](http://msdn.microsoft.com/library/windows/apps/dn652520)) to call a file picker window and let the user pick multiple files. For more info, see [Open files and folders with a picker](https://msdn.microsoft.com/library/windows/apps/mt186456).

3.  **Let the user pick one folder to access**

    Uses the [FolderPicker](http://msdn.microsoft.com/library/windows/apps/br207881).[PickSingleFolderAsync](http://msdn.microsoft.com/library/windows/apps/br207885) method (or [PickFolderAndContinue](http://msdn.microsoft.com/library/windows/apps/dn652525)) to call a file picker window and let the user pick multiple files. For more info, see [Open files and folders with a picker](https://msdn.microsoft.com/library/windows/apps/mt186456).

4.  **Let the user save a file and specify the name, file type, and/or save location**

    Uses the [FileSavePicker](http://msdn.microsoft.com/library/windows/apps/br207871).[PickSaveFileAsync](http://msdn.microsoft.com/library/windows/apps/br207876) method (or [PickSaveFileAndContinue](http://msdn.microsoft.com/library/windows/apps/dn652523)) to call a file picker window and let the user pick multiple files. For more info, see [Save a file with a picker](https://msdn.microsoft.com/library/windows/apps/mt186455).

5.  **Let the user pick a locally cached file to access**

    The user may choose access a file that is provided by another app (the providing app) that participates in the Cached File Updater contract. Like the first scenario, this scenario uses the [FileOpenPicker](http://msdn.microsoft.com/library/windows/apps/br207847) to access these files and to show how the providing app (the [File picker contracts sample](http://go.microsoft.com/fwlink/p/?linkid=231536)) can interact with the user through the file picker if necessary, for example if credentials are required to access the file.

    **Note** This scenario requires the [File picker contracts sample](http://go.microsoft.com/fwlink/p/?linkid=231536).

6.  **Let the user save a locally cached file**

    The user may choose save a file that was provided by another app (the providing app) that participates in the Cached File Updater contract. This scenario uses the [FileSavePicker](http://msdn.microsoft.com/library/windows/apps/br207871) and the [CachedFileManager](http://msdn.microsoft.com/library/windows/apps/hh701431) to let the user save a file to another app (the [File picker contracts sample](http://go.microsoft.com/fwlink/p/?linkid=231536)) and how the providing app can interact with the user through the file picker if necessary, for example if there is a version conflict.

    **Note** This scenario requires the [File picker contracts sample](http://go.microsoft.com/fwlink/p/?linkid=231536).

To learn more about accessing and saving files and folders through file pickers, see [Guidelines and checklist for file pickers](http://msdn.microsoft.com/library/windows/apps/hh465182).

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

## Samples

[File access sample](http://go.microsoft.com/fwlink/p/?linkid=231445)  
[Using a Blob to save and load content sample](http://go.microsoft.com/fwlink/p/?linkid=231615)  
[File and folder thumbnail sample](http://go.microsoft.com/fwlink/p/?linkid=231522)  
[File picker contracts sample](http://go.microsoft.com/fwlink/p/?linkid=231536)  

## Reference

[Windows.Storage.Pickers namespace](http://msdn.microsoft.com/library/windows/apps/br207928)  

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
