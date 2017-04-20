<!---
  category: GraphicsAndAnimation
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620483
--->

# Composition visual without framework sample

Provides an introduction to the [Windows.UI.Composition](https://msdn.microsoft.com/library/windows.ui.composition.aspx) namespace.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Windows 10 introduces a unified compositor and rendering engine for universal applications.
Application can use the
[Windows.UI.Composition](https://msdn.microsoft.com/library/windows.ui.composition.aspx) namespace
to create composition objects and apply animation, effects and manipulations on those objects.

The sample demonstrates the following:

* Relying entirely on composition objects, rather than using XAML, HTML, or DirectX.
* Setting up a [Compositor](https://msdn.microsoft.com/library/windows.ui.composition.compositor.aspx) for creating composition objects.
* Creating a [ContainerVisual](https://msdn.microsoft.com/library/windows.ui.composition.containervisual.aspx)
  to hold all of the composition objects.
* Constructing and walking a simple tree of [Visual](https://msdn.microsoft.com/library/windows.ui.composition.visual.aspx) objects.
* Using a [SolidColorVisual](https://msdn.microsoft.com/library/windows.ui.composition.solidcolorvisual.aspx)
* Receiving input and
  changing opacity of a [Visual](https://msdn.microsoft.com/library/windows.ui.composition.visual.aspx) based on that input.

**Note** The Universal Windows app samples require Visual Studio 2015 to build and Windows 10 to execute.
(See additional discussion below under **Prerequisites**.)

To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421).

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422).

## Prerequisites

- This sample will not pass WACK because it uses the
  previewUiComposition capability in order to access the
  [Windows.UI.Composition](https://msdn.microsoft.com/library/windows.ui.composition.aspx) preview namespace.
- This sample requires Windows 10 build 10240 to run. It will not run on Insider Builds after 10240 due to updates as noted in the Known Issues section on MSDN.
(See **Other resources** below.)

## Related topics

### Reference

[Windows.UI.Composition](https://msdn.microsoft.com/library/windows.ui.composition.aspx) namespace  
[Compositor](https://msdn.microsoft.com/library/windows.ui.composition.compositor.aspx) class  
[ContainerVisual](https://msdn.microsoft.com/library/windows.ui.composition.containervisual.aspx) class  
[SolidColorVisual](https://msdn.microsoft.com/library/windows.ui.composition.solidcolorvisual.aspx) class  
[Visual](https://msdn.microsoft.com/library/windows.ui.composition.visual.aspx) class  

### Other resources

For more samples using Windows.UI.Composition please visit the [Windows.UI.Composition repo on GitHub](https://github.com/Microsoft/composition).

For an overview of the Windows.UI.Composition namespace,
see [our presentation at the 2015 //build conference](https://channel9.msdn.com/Events/Build/2015/2-672).

Stay current on all of the latest issues for the most recent SDK by reviewing our list of
[Known Issues on MSDN](https://social.msdn.microsoft.com/Forums/home?forum=Win10SDKToolsIssues&sort=relevancedesc&brandIgnore=True&searchTerm=Windows.UI.Composition).

## System requirements

**Client:** Windows 10 build 10240 exactly

**Server:** Not supported

**Phone:** Windows 10 build 10240 exactly

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
