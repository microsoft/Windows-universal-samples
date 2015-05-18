# Media Transport Controls sample

This sample shows how to create customized media transport controls for the Media Element in your XAML Windows App.

Specifically, this sample shows how to:

- **Set up the custom template:** We can add a new template in our Themes/generic.xaml folder and create a custom template. We also added a class called CustomControls.cs 
which show how to use the custom template in your app
- **Add a custom button:** In generic.xaml we added a custom button in our CommandBar and show how to reference it and make it usable in CustomControls.cs
- **Change the color of the slider:** We added references in app.xaml for a custom brush which we then used to change the color of our media slider

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10, go to [Windows 10](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples

[Sample 1](https://github.com/Microsoft/Windows-universal-samples/tree/master/<sample>)

### Reference

<!-- Add links to related API -->

[Media Transport Controls](https://msdn.microsoft.com/en-us/library/windows/apps/windows.ui.xaml.controls.mediatransportcontrols.aspx)

## System requirements

**Client:** Windows 10 Insider Preview

**Server:** Windows 10 Insider Preview

**Phone:**  Windows 10 Insider Preview

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
