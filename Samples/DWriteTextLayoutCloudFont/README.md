<!---
  category: ControlsLayoutAndText
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620539
--->

# Downloadable fonts (DirectWrite) sample

Shows how to use DirectWrite downloadable fonts, a feature added in Windows 10, together with the DirectWrite text layout API.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

A new capability in DirectWrite for Windows 10 allows an app to format text content using fonts that may not be installed on a device, and for the font to be downloaded on demand from a Microsoft service. DirectWrite now includes low-level APIs for using downloadable fonts. An easy way to leverage the downloadable font mechanism is to use DirectWrite's text layout (IDWriteTextLayout3), which integrates the lower-level APIs and does part of the work for you. When a text layout is created with a Windows font that is not locally installed, the text layout will automatically add requests for the font data to a font download queue. You need to add code that initiates the download and that responds when the download is completed.

## Sample project files
The sample is intended to demonstrate how to use the DirectWrite downloadable font mechanism together with DirectWrite's text layout API. The app is comprised of two projects:

* The DWriteTextLayoutCloudFontImplementation project provides the implementation of DirectWrite APIs for text layout and downloadable fonts that are the main focus of this sample. It is implemented as a Windows Runtime Component that wraps around DirectWrite and enables interop with the sample client app. It also uses other DirectX APIs to implement a XAML SurfaceImageSource as a means of displaying the text layout.
* The DWriteTextLayoutCloudFont project provides the sample app shell, implemented using XAML, that functions as a client of the the DirectWrite APIs being demonstrated.

This organization suits the needs of this sample app, utilizing the simplicity of XAML for UI to navigate between scenarios while keeping the native DirectWrite code that's of primary interest separate and easier for you to focus on.

### DWriteTextLayoutCloudFontImplementation project
Within the DWriteTextLayoutCloudFontImplementation project, the following files are significant:

* The TextLayout.h/.cpp files wrap the DirectWrite text layout API.
* The FontDownloadListener.h/.cpp files wrap around DirectWrite lower-level APIs for interacting with the font download mechanism, and provide an implementation of the IDWriteFontDownloadListener interface, needed for responding to the download mechanism.
* The FontNameCollector.h/.cpp files wrap additional DirectWrite APIs to determine which fonts are actually used in text layout. They are not needed to use the font download mechanism but provide you more insight, while the sample app is running, into how the text layout and font download mechanisms are interacting.
* The TextLayoutImageSource.h/.cpp files implement a XAML SurfaceImageSource using DirectX APIs for rendering the text layout.

### DWriteTextLayoutCloudFont project
Within the DWriteTextLayoutCloudFont project, the following files are significant:

* The Scenario\_Document1.\*, Scenario\_Document2.\* and Scenario\_Document3.\* files each invoke text layout using a different downloadable font, and then invoke and respond to the downloadable font mechanism.
* The Scenario\_SampleOverview.\* files provide guidance for using the sample app.
* The Scenario\_CloudFontOverview.\* files provide more information about the downloadable font mechanism.

Other files are boilerplate files used for UWP sample apps.

### Other files
The ClearDownloadableFontCache.ps1 file is not part of the sample project itself, but is a PowerShell script that can be used to reset the state of the downloadable font mechanism (clearing cached data) before or after running the sample app. Instructions for using this script are given in the sample app.

## Related topics

[IDWriteFactory3::GetSystemFontCollection method](https://msdn.microsoft.com/library/windows/desktop/dn890761)  
[IDWriteTextLayout3 interface](https://msdn.microsoft.com/library/windows/desktop/dn900405)  
[IDWriteFontDownloadQueue interface](https://msdn.microsoft.com/library/windows/desktop/dn890778)  
[IDWriteFontDownloadListener interface](https://msdn.microsoft.com/library/windows/desktop/dn890775)  
[Guidelines for fonts](https://msdn.microsoft.com/library/windows/apps/hh700394)  

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

- To run the sample with debugging, press F5 or select Debug > Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 
