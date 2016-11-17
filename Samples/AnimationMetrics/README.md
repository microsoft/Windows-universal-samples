<!---
  category: GraphicsAndAnimation
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620484
--->

# Animation metrics sample

Shows how to use the Animation Metrics APIs ([Windows.UI.Core.AnimationMetrics](http://msdn.microsoft.com/library/windows/apps/br241916) 
to access the raw parameters that define the animations in the Windows [Animation Library](http://msdn.microsoft.com/library/windows/apps/hh465165). 
This info can help developers of applications and application frameworks to create animations that are consistent with Windows and other 
apps that use the Windows Animation Library.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

The sample shows the metrics involved in the following scenarios:

- Adding an item to a list
- Bringing a new page on the screen

**Note** The Universal Windows app samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples

[AnimationLibrary](/Samples/AnimationLibrary)

### Reference

[AnimationMetrics namespace](https://msdn.microsoft.com/library/windows/apps/windows.ui.core.animationmetrics.aspx)

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Not supported

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

## How to use the sample

The sample provides three situations for which you retrieve the raw parameters that define the animations: the item being added in an AddToList animation, the item being affected in an AddToList animation, and a page of content being brought onto the screen with EnterPage. None of these animations themselves are shown; instead, the parameters are retrieved and displayed.

