<!---
  category: GraphicsAndAnimation
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620531
--->

# Direct2D custom image effects sample

Shows how to implement [custom Direct2D effects](http://msdn.microsoft.com/library/windows/desktop/jj710194) using HLSL pixel, vertex, and compute shaders.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample shows how to:

- Define the effect's properties, metadata and implement required interfaces
- Author HLSL that conforms to Direct2D requirements, including opting into effect shader linking
- Register and use a custom effect in an app

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Sample project files

This sample contains three separate projects, demonstrating a custom pixel, vertex, and compute shader effect.
Each project has a similar set of files that follow the same pattern.

### Custom Direct2D effect implementation
The following files are the core effect implementation, i.e. the main educational content of the sample:

- **\*.hlsl:** The core shader routines that operate on image data. These are written using standard HLSL with some helper Direct2D intrinsics that enable the custom effect to take advantage of shader linking.
- **\*Effect.cpp/.h,** **\*Transform.cpp/.h:** Implementation of the Direct2D effect interfaces, e.g. ID2D1EffectImpl. This code is called by the Direct2D renderer to setup and control the effect shader operation.

### Demo app
A custom Direct2D effect does nothing on its own, as it must be loaded and executed by a calling app. The following files provide a simple demo environment to exercise the effect:

- App.cpp/.h
- *Main.cpp/.h, *Renderer.cpp/.h

### DirectX SDK sample common files
The following files provide common functionality needed by DirectX SDK samples:

- **DeviceResources.cpp/.h:** Manages creation and lifetime of the core Direct3D and Direct2D device-dependent resources. Handles cases such as device lost and window size and orientation changes.
- **DirectXHelper.h:** Common inline helper functions, including ThrowIfFailed which converts HRESULT-based APIs into an exception model.
- **BasicReaderWriter.cpp/.h:** Basic file I/O functionality, needed for things like loading shaders, textures and geometry.
- **SampleOverlay.cpp/.h:** Renders the Windows SDK overlay badge on top of sample content.
- **BasicTimer.cpp/.h:** Wraps QueryPerformanceCounter to provide an accurate, low-overhead timer.

All DX SDK samples and the Visual Studio template DX project contain a version of these files (some are not always needed). These common files demonstrate important best practices for DX UWP apps, and you are encouraged to use them in your own projects.

### C++ UWP common files
Variants of the following files are found in every UWP app written in C++:

- Package.appxmanifest
- pch.cpp/.h
- *.vcxproj
- *.vcxproj.filters
- *.sln

## Related topics

### Feature areas

[Direct2D effects](http://msdn.microsoft.com/library/windows/desktop/hh706327): provides built-in effects and the ability to create custom effects.  
[Direct2D](http://msdn.microsoft.com/library/windows/desktop/dd370990): used to render images, primitives, and text.  
[Windows Imaging Component (WIC)](http://msdn.microsoft.com/library/windows/desktop/ee719655): used to load, scale, and convert the images.  

### Reference

Custom effect interfaces:

[ID2D1EffectContext](http://msdn.microsoft.com/library/windows/desktop/hh404459)  
[ID2D1EffectImpl](http://msdn.microsoft.com/library/windows/desktop/hh404568)  
[ID2D1Transform](http://msdn.microsoft.com/library/windows/desktop/hh446919)  

Related DirectX app APIs:

[ID2D1DeviceContext](http://msdn.microsoft.com/library/windows/desktop/hh404479)  
[IDXGIFactory2::CreateSwapChainForCoreWindow](http://msdn.microsoft.com/library/windows/desktop/hh404559)  
[CoreWindow](http://msdn.microsoft.com/library/windows/apps/br208225)  

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 