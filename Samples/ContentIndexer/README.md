<!---
  category: Data
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620524
--->

# Content indexer sample

Shows how to add, update, and retrieve items and properties from the indexer. 

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Two methods of doing so are demonstrated. 
The first is using the ContentIndexer APIs to directly interface with the indexer. The second is writing .appcontent-ms files, 
which contain information to be indexed that the indexer will pick up once the file has been written.

The sample demonstrates these tasks:

1.  **Add an item to the system index**

    Uses the [AddAsync](http://msdn.microsoft.com/library/windows/apps/dn298342) method to make app content searchable in the system index.

2.  **Update and delete items in the index**

    Uses the [UpdateAsync](http://msdn.microsoft.com/library/windows/apps/dn298355) method to update an item and the [DeleteAsync](http://msdn.microsoft.com/library/windows/apps/dn298348), [DeleteMultipleAsync](http://msdn.microsoft.com/library/windows/apps/dn298349), and [DeleteAllAsync](http://msdn.microsoft.com/library/windows/apps/dn298347) methods to remove content from the system index.

3.  **Get items from the system index by using a query**

    Uses the [CreateQuery](http://msdn.microsoft.com/library/windows/apps/dn298343) and [GetAsync](http://msdn.microsoft.com/library/windows/apps/dn298334) methods to retrieve items from the system index.

4.  **Check the revision number of the indexer**

    Uses the [Revision](http://msdn.microsoft.com/library/windows/apps/dn298354) property and the app's [LocalSettings](http://msdn.microsoft.com/library/windows/apps/br241622) to check if the app's expected revision number matches the actual index revision number.

5.  **Add app content files to be indexed**

    Copies app-specific files to the app's LocalState\\Indexed folder to make app content searchable in the system index.

6.  **Remove file content from the system index**

    Deletes app-specific files from the app's LocalState\\Indexed folder to remove app content from the system index.

7.  **Get indexed file properties**

    Uses the [CreateFileQuery](http://msdn.microsoft.com/library/windows/apps/br227252) and [CreateFileQueryWithOptions](http://msdn.microsoft.com/library/windows/apps/br211591) methods to query the system index for file properties.

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Reference

[ContentIndexer](http://msdn.microsoft.com/library/windows/apps/dn298331)  
[ContentIndexerQuery](http://msdn.microsoft.com/library/windows/apps/dn298332)  
[CreateFileQuery](http://msdn.microsoft.com/library/windows/apps/br227252)  
[CreateFileQueryWithOptions](http://msdn.microsoft.com/library/windows/apps/br211591)  

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
