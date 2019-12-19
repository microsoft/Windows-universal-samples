---
page_type: sample
languages:
- cpp
- cppcx
products:
- windows
- windows-uwp
urlFragment: D2DAdvancedColorImages
extendedZipContent:
- path: SharedContent
  target: SharedContent
- path: LICENSE
  target: LICENSE
description: "Shows how to use Direct2D to load and draw color images with high dynamic range (HDR), wide color gamut (WCG) and/or high bit depth content."
---

<!---
  category: GraphicsAndAnimation
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=852875
--->

# Direct2D advanced color image rendering sample

Shows how to use [Direct2D](http://msdn.microsoft.com/library/windows/desktop/dd370990) to load and draw advanced color images. Advanced color images have high dynamic range (HDR), wide color gamut (WCG) and/or high bit depth content.

Advanced color content is best viewed on an advanced color-capable device, such as an HDR10 display. However, this sample shows how to render content on any display type.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> You can download this sample as a standalone ZIP file
> [from docs.microsoft.com](https://docs.microsoft.com/samples/microsoft/windows-universal-samples/d2dadvancedcolorimages/),
> or you can download the entire collection as a single
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample shows how to:

- Create and configure an advanced color (FP16) DirectX render pipeline.
- Use WIC to decode a variety of SDR, WCG and HDR image content, including extended-range floating point data.
- Use Direct2D's effect pipeline to correctly perform color management and HDR brightness adjustment.
- Use Windows.Graphics.Display to retrieve display color capabilities and adapt the image rendering pipeline to the display.
- Implement a custom Direct2D effect to perform HDR-to-SDR tonemapping.
- Calculate HDR metadata of an image to assist with tonemapping.

## Sample project files

The sample's project files fall into the following categories.

### Sample-specific files
The following files form the main educational content of the sample:

- **DirectXPage.xaml/.cpp/.h/:** Implements a XAML-based UI to drive the app. Hosts the DirectX content in a SwapChainPanel.
- **App.xaml/.cpp/.h:** The app's main entry point.
- **D2DAdvancedColorImagesRenderer.cpp/.h:** The core implementation of the Direct2D scene.
- **RenderOptions.h:** Helper classes for databinding rendering options to the XAML UI.
- ***Effect.cpp/.h/.hlsl:** Custom Direct2D effects implementing some example tonemappers.

### DirectX SDK sample common files
The following files provide common functionality needed by DirectX SDK samples:

- **DeviceResources.cpp/.h:** Manages creation and lifetime of the core Direct3D and Direct2D device-dependent resources. Handles cases such as device lost and window size and orientation changes.
- **DirectXHelper.h:** Common inline helper functions, including ThrowIfFailed which converts HRESULT-based APIs into an exception model.

All DirectX SDK samples and the Visual Studio template DirectX projects contain a version of these files. These common files demonstrate important best practices for DirectX UWP apps, and you are encouraged to use them in your own projects.

### C++ UWP common files
Variants of the following files are found in every UWP app written in C++:

- Package.appxmanifest
- pch.cpp/.h
- D2DAdvancedColorImagesRenderer.vcxproj
- D2DAdvancedColorImagesRenderer.vcxproj.filters
- D2DAdvancedColorImagesRenderer.sln

## Related topics

### Feature areas

[Direct2D](http://msdn.microsoft.com/library/windows/desktop/dd370990): used to render images, primitives, and text.  
[XAML overview](https://msdn.microsoft.com/library/windows/apps/mt185595): technology for building user interfaces in managed and C++ code.  

### Reference

Direct2D APIs:

[Color management effect](https://msdn.microsoft.com/en-us/library/windows/desktop/hh706318)  
[ID2D1DeviceContext2::CreateImageSourceFromWic method](https://msdn.microsoft.com/en-us/library/windows/desktop/dn890793)  

Windows.Graphics.Display:

[AdvancedColorInfo](https://docs.microsoft.com/uwp/api/windows.graphics.display.advancedcolorinfo) 

Related APIs:

[SwapChainPanel](https://msdn.microsoft.com/library/windows/apps/windows.ui.xaml.controls.swapchainpanel)  

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 
