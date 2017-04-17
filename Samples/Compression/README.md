<!---
  category: Data
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620523
--->

# Compression sample

Shows how to read structured data from a file and write compressed data to a new file and how to read compressed data and write decompressed data to a new file.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Many apps need to compress and decompress data. The [Windows.Storage.Compression](http://msdn.microsoft.com/library/windows/apps/br207698) 
namespace simplifies this task by providing a unified interface that exposes the MSZIP, XPRESS, XPRESS\_HUFF, and LZMS compression algorithms. 
This lets you manage versions, service, and extend the exposed compression algorithms and frees you from responsibility for managing block sizes, 
compression parameters, and other details that the native [Compression API](http://msdn.microsoft.com/library/windows/apps/hh437596) requires. 
A subset of [Win32 and COM for apps](http://go.microsoft.com/fwlink/p/?linkid=246262) can be used by apps to support scenarios not already covered 
by the Windows Runtime, HTML/Cascading Style Sheets (CSS), or other supported languages or standards. For this purpose, you can also use the native 
Compression API to develop apps.

Specifically, this sample shows the following:

-   Read uncompressed data from an existing file
-   Specify the compression algorithm to use.
-   Compress the data using the selected compression algorithm.
-   Write the compressed data to a new file.
-   Read the compressed data from a file.
-   Decompress the data.

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Operating system requirements

Client

Windows 10

Server

Windows Server 2012 R2

Phone

Windows 10 Mobile

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

**Deploying the sample**

-   To deploy the built sample:

    1.  Select **Compression** in **Solution Explorer**.
    2.  Use **Build** \> **Deploy Solution** or **Build** \> **Deploy Compression**.

**Deploying and running the sample**

-   To deploy and run the sample:

    1.  Right-click **Compression** in **Solution Explorer** and select **Set as StartUp Project**.
    2.  To debug the sample and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.
-   To deploy and run the Windows Phone version of the sample:

