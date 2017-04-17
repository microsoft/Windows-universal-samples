<!---
  category: FilesFoldersAndLibraries
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620545
--->

# File and folder thumbnail sample

Shows how to retrieve thumbnails for files and folders using [Windows.Storage.FileProperties](http://msdn.microsoft.com/library/windows/apps/br207831).

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

The sample demonstrates these tasks:

1.  **Retrieve a thumbnail for a picture**

2.  **Retrieve album art as the thumbnail for a song**

3.  **Retrieve an icon as the thumbnail for a document**

4.  **Retrieve a thumbnail for a folder in the file system**

    **Note**  You can't retrieve a thumbnail for the Pictures library itself because it is a virtual folder. You must choose a file system folder that has pictures in it in order to retrieve a thumbnail.

5.  **Retrieve a thumbnail for a file group**

    A file group is a virtual folder where all the files in the group have the criteria that you specify in common. For example, the sample shows a thumbnail for a file group wherein the files in the group all have the same month and year.

To learn about retrieving the appropriate thumbnail to display to the user, see [Guidelines and checklist for thumbnails](http://msdn.microsoft.com/library/windows/apps/hh465350).

Important APIs in this sample include:

-   [StorageItemThumbnail](http://msdn.microsoft.com/library/windows/apps/br207802) class
-   [ThumbnailMode](http://msdn.microsoft.com/library/windows/apps/br207809) enumeration
-   [StorageFile](http://msdn.microsoft.com/library/windows/apps/br227171).[GetThumbnailAsync](http://msdn.microsoft.com/library/windows/apps/br227210) methods
-   [StorageFolder](http://msdn.microsoft.com/library/windows/apps/br227230).[GetThumbnailAsync](http://msdn.microsoft.com/library/windows/apps/br227288) methods
-   [IStorageItemProperties](http://msdn.microsoft.com/library/windows/apps/hh701614).[GetThumbnailAsync](http://msdn.microsoft.com/library/windows/apps/hh701636) methods

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

## Samples

[File access sample](http://go.microsoft.com/fwlink/p/?linkid=231445)  
[File picker sample](http://go.microsoft.com/fwlink/p/?linkid=231464)  
[Folder enumeration sample](http://go.microsoft.com/fwlink/p/?linkid=231512)  
[Programmatic file search sample](http://go.microsoft.com/fwlink/p/?linkid=231532)  

## Reference

[Windows.Storage namespace](http://msdn.microsoft.com/library/windows/apps/br227346)  
[Windows.Storage.FileProperties namespace](http://msdn.microsoft.com/library/windows/apps/br207831)  

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
