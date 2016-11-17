<!---
  category: FilesFoldersAndLibraries
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=619993
--->

# Folder enumeration sample

Shows how to enumerate the top-level files and folders inside a location (like a folder, device, or network location), 
and how to use queries to enumerate all files inside a location by sorting them into file groups.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample uses [Windows.Storage](http://msdn.microsoft.com/library/windows/apps/br227346) and [Windows.Storage.Search](http://msdn.microsoft.com/library/windows/apps/br208106) APIs, including [StorageFolder](http://msdn.microsoft.com/library/windows/apps/br227230) and [StorageFolderQueryResult](http://msdn.microsoft.com/library/windows/apps/br208066).

The sample demonstrates these tasks:

1.  **Enumerate top-level files and subfolders of a folder**

    Uses the [StorageFolder](http://msdn.microsoft.com/library/windows/apps/br227230).[GetFilesAsync](http://msdn.microsoft.com/library/windows/apps/br227273) 
and [StorageFolder.GetFoldersAsync](http://msdn.microsoft.com/library/windows/apps/br227279) methods to enumerate only the top-level files and folders (the immediate children) of the location (in this case, the Pictures library). For a walkthrough of this task, see [Quickstart: Accessing files programmatically](http://msdn.microsoft.com/library/windows/apps/jj150596).

2.  **Query all the files in a folder (and its subfolders) and create groups of files to enumerate**

    Uses the [StorageFolder](http://msdn.microsoft.com/library/windows/apps/br227230).[CreateFolderQueryWithOptions](http://msdn.microsoft.com/library/windows/apps/br211592) method to sort all files in the location (in this case, the Pictures library) into groups based on the criteria that you specify and uses a [StorageFolderQueryResult](http://msdn.microsoft.com/library/windows/apps/br208066).[GetFoldersAsync](http://msdn.microsoft.com/library/windows/apps/br208072) method to retrieve the resulting file groups.

    File groups are virtual folders that are represented by [StorageFolder](http://msdn.microsoft.com/library/windows/apps/br227230) objects. The files in a file group have the criteria that you specify in common. For example, as the sample shows, the files in a group might have the same rating.

    For a walkthrough of this task, see [Quickstart: Accessing files programmatically](http://msdn.microsoft.com/library/windows/apps/jj150596).

3.  **Query all the files in a folder (and its subfolders) and retrieve file properties as a part of getting results for the query**

    Uses [QueryOptions](http://msdn.microsoft.com/library/windows/apps/br207995).[SetPropertyPrefetch](http://msdn.microsoft.com/library/windows/apps/hh973319) to specify properties to retrieve when the query is created. [StorageFolder](http://msdn.microsoft.com/library/windows/apps/br227230).[CreateFolderQueryWithOptions](http://msdn.microsoft.com/library/windows/apps/br211592) and [GetFilesAsync](http://msdn.microsoft.com/library/windows/apps/br227273) are used to create the query and enumerate results. Similarly, you can use [SetThumbnailPrefetch](http://msdn.microsoft.com/library/windows/apps/hh973320) to retrieve thumbnails as a part of creating the query.

    Using [SetPropertyPrefetch](http://msdn.microsoft.com/library/windows/apps/hh973319) and [SetThumbnailPrefetch](http://msdn.microsoft.com/library/windows/apps/hh973320) might make the query take longer to execute, but will make accessing large amounts of file information more efficient.

4.  **Query all the files in a folder and show file provider and availability**

    Uses the [Provider](https://msdn.microsoft.com/library/windows/apps/windows.storage.storagefile.provider.aspx) and [IsAvailable*]() properties to obtain the provider and whether the file is available offline.

Additional important APIs in this sample include:

-   [CommonFolderQuery](http://msdn.microsoft.com/library/windows/apps/br207957) enumeration
-   [CommonFileQuery](http://msdn.microsoft.com/library/windows/apps/br207956) enumeration
-   [PropertyPrefetchOptions](http://msdn.microsoft.com/library/windows/apps/hh973317) enumeration

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples

[Programmatic file search sample](http://go.microsoft.com/fwlink/p/?linkid=231532)  
[File access sample](http://go.microsoft.com/fwlink/p/?linkid=231445)  
[File and folder thumbnail sample](http://go.microsoft.com/fwlink/p/?linkid=231522)  

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
