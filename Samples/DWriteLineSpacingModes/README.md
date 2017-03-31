<!---
  category: ControlsLayoutAndText
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620538
--->

# Line spacing (DirectWrite) sample

Shows how to use different line spacing options that are provided for the DirectWrite text layout API.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

In previous versions, DirectWrite supported two different line spacing methods: default and uniform. The uniform method gives the app exact control over line spacing distances, but could not support some common scenarios such as "double" spacing. In Windows 10, a new "proportional" line spacing method is added, with associated APIs for you to specify various input parameters. 

Line metrics sound straightforward: line height, ascender and descender. But when you get into details of various scenarios, it's actually somewhat difficult to understand in the abstract. For that reason, in addition to demonstrating use of the DirectWrite APIs, this sample also allows you to explore and see visually how different input parameters affect the computed results.

## Sample project files
The sample is intended to demonstrate how to use the DirectWrite text layout line spacing APIs and the effects of the various input parameters. The app is comprised of two projects:

* The DWriteTextLayoutImplementation project provides the implementation of DirectWrite APIs for text layout and line spacing that are the main focus of this sample. It is implemented as a Windows Runtime Component that wraps around DirectWrite and enables interop with the sample client app. It also uses other DirectX APIs to implement a XAML SurfaceImageSource as a means of displaying the text layout.
* The DWriteLineSpacing Modes project provides the sample app shell, implemented using XAML, that functions as a client of the the DirectWrite APIs being demonstrated.

This organization suits the needs of this sample app, utilizing the simplicity of XAML for UI to navigate between scenarios while keeping the native DirectWrite code that's of primary interest separate and easier for you to focus on.

### DWriteTextLayoutImplementation project
Within the DWriteTextLayoutImplementation project, the following files are significant:

* The TextLayout.h/.cpp files wrap the DirectWrite text layout and related line spacing APIs. 
* The TextLayoutImageSource.h/.cpp files implement a XAML SurfaceImageSource using DirectX APIs for rendering the text layout.

### DWriteTextLayoutCloudFont project
Within the DWriteTextLayoutCloudFont project, the following files are significant:

* The Scenario1\_DefaultSpacing.\*, Scenario2\_UniformSpacing.\* and Scenario3\_ProportionalSpacing.\* files each demonstrate a different line spacing method.

Other files are boilerplate files used for UWP sample apps.

## Related topics

[IDWriteTextLayout3 interface](https://msdn.microsoft.com/library/windows/desktop/dn900405)  
[IDWriteTextLayout3::SetLineSpacing method](https://msdn.microsoft.com/library/windows/desktop/dn900409)  
[DWRITE\_LINE\_SPACING structure](https://msdn.microsoft.com/library/windows/desktop/dn933216)  
[DWRITE\_LINE\_SPACING_METHOD enumeration](https://msdn.microsoft.com/library/windows/desktop/dd368101)  
[DWRITE\_FONT\_LINE_GAP\_USAGE enumeration](https://msdn.microsoft.com/library/windows/desktop/dn933211)  
[IDWriteTextLayout3::GetLineMetrics method](https://msdn.microsoft.com/library/windows/desktop/dn900406)  
[DWRITE\_LINE\_METRICS1 structure](https://msdn.microsoft.com/library/windows/desktop/dn933215)  

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
