<!---
  category: ControlsLayoutAndText
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620632
--->

# Downloadable fonts (XAML) sample

Demonstrates XAML integration of DirectWrite downloadable fonts, a feature added in Windows 10.

The downloadable font mechanism used in this sample is directly integrated into XAML and can be leveraged in apps just by formatting XAML text elements using Windows fonts. No special APIs or markup are required.

##Sample project files
The sample is intended to demonstrate the runtime behavior of the DirectWrite downloadable mechanism that is integrated into XAML. The following project files are of particular interest for this sample:

* The Scenario\_Document1.\*, Scenario\_Document2.\* and Scenario\_Document3.\* files are the specific files in which the downloadable font mechanism is demonstrated. 
* The Scenario\_SampleOverview.\* files provide guidance for using the sample app.
* The Scenario\_CloudFontOverview.\* files provide more information about the downloadable font mechanism.

The ClearDownloadableFontCache.ps1 file is not part of the sample project itself, but is a PowerShell script that can be used to reset the state of the downloadable font mechanism (clearing cached data) before or after running the sample app. Instructions for using this script are given in the sample app.

Other files are boilerplate files used for UWP sample apps.

The downloadable font mechanism can be utilized just by formatting text elements using Windows fonts. In this sample app, the particular font choices are applied programmatically at runtime rather than directly in XAML markup. This is done so that the behavior of the mechanism will be experienced while the app is running, rather than when the XAML source files are viewed in Visual Studio.


##Related topics

[Guidelines for fonts](https://msdn.microsoft.com/library/windows/apps/hh700394.aspx)


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
