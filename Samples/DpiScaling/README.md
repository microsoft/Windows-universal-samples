<!---
  category: ControlsLayoutAndText
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620537
--->

# Scaling according to DPI sample

Shows how to build an app that scales according to the pixel density (dots per inch or dpi) of the screen by loading images 
of the right scale or by overriding default scaling. This sample uses the [Windows.Graphics.Display](http://msdn.microsoft.com/library/windows/apps/br226166) API.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

The sample demonstrates these scenarios:

**Loading images for different dpi scales**  
When a screen’s pixel density (dpi) and resolution are very high, Windows scales images and other UI elements to maintain physical sizing across devices. We recommend that you make your app scaling aware by providing multiple versions of these assets so that they retain quality across different scale factors. If you don’t provide multiple versions, Windows will stretch your assets by default.

**Overriding default scaling of UI elements**  
To preserve the physical size of UI, Windows automatically scales UI elements as the scale factor ([ResolutionScale](http://msdn.microsoft.com/library/windows/apps/br226165)) changes. You might not want this behavior especially if your app doesn’t have a high-res version of an element available. This scenario demonstrates how to override the automatic scaling of text and UI as the scale factor changes from 100% to 140%. You might want to use this scenario if you don’t want Windows to automatically scale your images or text.

Important APIs in this sample include:

-   [onresize](http://msdn.microsoft.com/library/windows/apps/hh466035) event
-   [DisplayInformation](http://msdn.microsoft.com/library/windows/apps/dn264258) class
-   [RawPixelsPerViewPixel](https://msdn.microsoft.com/library/windows.graphics.display.displayinformation.rawpixelsperviewpixel.aspx) property

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

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
