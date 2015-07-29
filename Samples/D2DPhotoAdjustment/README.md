<!---
  category: AudioVideoAndCamera
--->

# Direct2D photo adjustment sample

This sample demonstrates how to build a photo viewer and editor using [**Direct2D**](http://msdn.microsoft.com/library/windows/desktop/dd370990) and [**Direct2D Effects**](http://msdn.microsoft.com/library/windows/desktop/hh706327).

Specifically, this sample shows how to:

- Efficiently load images into Direct2D
- Setup and control a photo photo editing pipeline
- Hook up UI in XAML

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10, go to [Windows 10](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Feature areas

- [**Direct2D Effects**](http://msdn.microsoft.com/library/windows/desktop/hh706327): provides built-in effects including a rich library of photo editing operations.
- [**Direct2D**](http://msdn.microsoft.com/library/windows/desktop/dd370990): used to render images, primitives, and text.
- [**Windows Imaging Component (WIC)**](http://msdn.microsoft.com/library/windows/desktop/ee719655): used to decode images from disk.
- [**XAML**](http://msdn.microsoft.com/library/windows/apps/hh700354): technology for building user interfaces in managed and C++ code.

### Reference

Direct2D APIs:

- [ID2D1ImageSourceFromWic](http://msdn.microsoft.com/library/windows/desktop/dn900414)
- [ID2D1DeviceContext](http://msdn.microsoft.com/library/windows/desktop/hh404479)
- [Direct2D built-in effects](https://msdn.microsoft.com/library/windows/desktop/hh706316)

Related APIs:

- [SwapChainPanel](https://msdn.microsoft.com/library/windows/apps/windows.ui.xaml.controls.swapchainpanel)

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

## Build the sample

1. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2. Go to the directory to which you unzipped the sample. Then go to the subdirectory containing the sample in the language you desire - either C++, C#, or JavaScript. Double-click the Visual Studio 2015 Solution (.sln) file. 
3. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**. 

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 