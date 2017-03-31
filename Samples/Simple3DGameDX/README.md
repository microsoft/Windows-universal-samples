<!---
  category: Gaming
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620598
--->

# Direct3D game sample

Shows the basic end-to-end implementation of a simple first person 3-D game using DirectX (Direct3D 11.2, Direct2D, Windows.Gaming.Input, and XAudio2) in a C++ app.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample supports:

- A touch input model
- A mouse and keyboard input model
- A game controller input model
- Stereoscopic 3-D display
- A Direct2D heads-up display
- A persistent game state
- Sound effect playback

There are two versions of this sample:

- **Simple3DGameDX**: Uses DirectX to render game UI and implements its own layout.
- **Simple3DGameXaml**: Uses XAML to render game UI and handle layout.

Both versions share significant amounts of logic: the Common and GameContent folders are identical. In addition, to reduce disk space usage, Simple3DGameXaml references some its assets from the Simple3DGameDX\Cpp\Assets folder.

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Reference

This sample is written in C++ and requires some experience with graphics programming. A code walkthrough for this sample is available here:

- [Create a simple  game with DirectX](https://msdn.microsoft.com/library/windows/apps/mt210793)

These topics provide info about the APIs used in this sample:

- [DirectX Graphics and Gaming](http://msdn.microsoft.com/library/windows/apps/ee663274)
- [Direct3D 11 Overview](http://msdn.microsoft.com/library/windows/apps/ff476345)
- [Direct3D 11 Reference](http://msdn.microsoft.com/library/windows/apps/ff476147)
- [DXGI reference](http://msdn.microsoft.com/library/windows/apps/bb205169)
- [XAudio2](http://msdn.microsoft.com/library/windows/apps/hh405049)
- [Windows.Gaming.Input](https://msdn.microsoft.com/library/windows/apps/windows.gaming.input.aspx)

## See also

[DirectX marble maze game sample](https://github.com/Microsoft/Windows-appsample-marble-maze)  

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