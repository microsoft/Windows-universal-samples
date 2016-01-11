<!---
  category: AudioVideoAndCamera
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620023
--->

# Media transport controls sample

This sample shows how to create customized media transport controls for the Media Element in your XAML Windows App.

Specifically, this sample shows how to:

- **Set up the custom template:** We can add a new template in our Themes/generic.xaml folder and create a custom template. We also added a class called CustomControls.cs 
which show how to use the custom template in your app
- **Add a custom button:** In generic.xaml we added a custom button in our CommandBar and show how to reference it and make it usable in CustomControls.cs
- **Change the color of the slider:** We added references in app.xaml for a custom brush which we then used to change the color of our media slider

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](https://dev.windows.com)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Reference

<!-- Add links to related API -->

[Media Transport Controls](https://msdn.microsoft.com/en-us/library/windows/apps/windows.ui.xaml.controls.mediatransportcontrols.aspx)

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
