<!--
  category: Data
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620633
--->

# x:DeferLoadStrategy sample

Shows how to reduce your app's startup time by deferring the creation of elements defined in your markup until you really need them.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample shows how to:

- **Create incidental UI when needed:** By responding to an event and calling FindName, you can realize your elements based on user interaction.
- **Create adaptive UIs that are less expensive:** By deferring elements and then targeting them with the VisualStateManager, you can reduce the footprint of your adaptive UIs in form factors that don't match the specified conditions of the VisualState.
- **Defer control template parts:** If you are creating custom controls that has pieces that you expect to not be used as often, you can defer those pieces of the control and only create them if the conditions under which those template parts are used is met.

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Reference

[x:DeferLoadStrategy](https://msdn.microsoft.com/library/windows/apps/xaml/mt204785.aspx)  

### Samples

[RSS reader sample](https://github.com/Microsoft/Windows-appsample-rssreader)  

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:**  Windows 10

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
