<!---
  category: GraphicsAndAnimation
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=832476
--->

# DirectWrite colored glyph sample

Shows how to implement a custom text renderer to handle different kinds of color glyph runs in 
[DirectWrite](https://msdn.microsoft.com/library/windows/desktop/dd368038) and 
[Direct2D](http://msdn.microsoft.com/library/windows/desktop/dd370990). This sample uses the custom renderer to draw a string of text with color glyphs. 

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Some important APIs used in this sample are:

- The [IDWriteFactory4::TranslateColorGlyphRun](http://msdn.microsoft.com/library/windows/desktop/mt761992) method, which translates a glyph run to a sequence of color glyph runs.
- The [ID2D1DeviceContext4::DrawSvgGlyphRun](http://msdn.microsoft.com/library/windows/desktop/mt750185) method, which draws a color glyph run that's defined in the SVG format.
- The [ID2D1DeviceContext4::DrawColorBitmapGlyphRun](http://msdn.microsoft.com/library/windows/desktop/mt750184) method, which draws a color glyph run that's defined in one of the bitmap formats.

## Sample project files

The sample's project files fall into the following categories:

### Sample-specific files

The following files implement and use the custom renderer, forming the main educational content of the sample:

- App.cpp/.h
- CustomTextRenderer.cpp/.h
- DWriteColorTextRendererMain.cpp/.h
- DWriteColorTextRendererRenderer.cpp/.h

CustomTextRenderer.cpp contains all of the color glyph handling code.

### DirectX SDK sample common files

The following files provide common functionality needed by DirectX SDK samples:

- **DeviceResources.cpp/.h:** Manages creation and lifetime of the core Direct3D and Direct2D device-dependent resources. Handles cases such as device lost and window size and orientation changes.
- **DirectXHelper.h:** Common inline helper functions, including ThrowIfFailed which converts HRESULT-based APIs into an exception model.
- **SampleOverlay.cpp/.h:** Renders the Windows SDK overlay badge on top of sample content.

All DX SDK samples and the Visual Studio template DX project contain a version of these files. These common files demonstrate important best practices for DX UWP apps, and you are encouraged to use them in your own projects.

### C++ UWP common files

Variants of the following files are found in every UWP app written in C++:

- Package.appxmanifest
- pch.cpp/.h
- DWriteColorTextRenderer.vcxproj
- DWriteColorTextRenderer.vcxproj.filters
- DWriteColorTextRenderer.sln

## Related topics

[Color fonts](http://msdn.microsoft.com/library/windows/desktop/mt765165)  

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
