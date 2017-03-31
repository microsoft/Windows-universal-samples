<!---
  category: PlatformArchitecture
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=808663
-->

# Version adaptive code sample

Shows ways of writing an app which
adapts to the version of Windows it is running on,
taking advantage of new features if available,
and falling back to a simpler version if not.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample shows:

- Detecting whether a specific runtime class, property, event, method, or enumerated value is available.
- Executing code conditionally based on whether an API is present.
- Choosing between two versions of a control based on whether an API is present.
- Using a visual state trigger to set a property based on whether an API is present. (XAML only)

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.

To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Reference

[ApiInformation class](https://msdn.microsoft.com/library/windows/apps/windows.foundation.metadata.apiinformation.aspx)  
[HardwareButtons.CameraPressed event](https://msdn.microsoft.com/library/windows/apps/windows.phone.ui.input.hardwarebuttons.camerapressed.aspx)  
[BackgroundAccessStatus enumeration](https://msdn.microsoft.com/library/windows/apps/windows.applicationmodel.background.backgroundaccessstatus.aspx)  
[OrientationSensor.GetDefault method](https://msdn.microsoft.com/library/windows/apps/windows.devices.sensors.orientationSensor.getdefault.aspx)  
[MediaElement](https://msdn.microsoft.com/library/windows/apps/windows.ui.xaml.controls.mediaelement.aspx)  
[CreateBackdropBrush](https://msdn.microsoft.com/library/windows/apps/windows.ui.composition.compositor.createbackdropbrush.aspx)  
[AllowFocusOnInteraction property](https://msdn.microsoft.com/library/windows/apps/windows.ui.xaml.controls.primitives.flyoutbase.allowfocusoninteraction.aspx)  
[MediaPlayerElement](https://msdn.microsoft.com/library/windows/apps/windows.ui.xaml.controls.mediaplayerelement.aspx)  

### Conceptual

[Version adaptive code](https://msdn.microsoft.com/windows/uwp/debug-test-perf/version-adaptive-code)  
[Guide to UWP apps](https://msdn.microsoft.com/windows/uwp/get-started/universal-application-platform-guide)  

#### Samples

[OrientationSensor sample](/Samples/OrientationSensor)  
[BackgroundActivation sample](/Samples/BackgroundActivation)  
[XamlStateTriggers sample](/Samples/XamlStateTriggers)  

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build.
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio?2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution.

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging.
