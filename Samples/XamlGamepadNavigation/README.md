<!---
  category: ControlsLayoutAndText
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=798524
--->

# Gamepad-style navigation (XAML) sample

Shows ways of improving directional navigation in your app.
Controllers such as remote controls,
gamepad joystick, or gamepad D-pad allow the user to navigate in the
four cardinal directions.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

The sample shows the following techniques:

- Opt the app into using directional navigation by setting Application.RequiresPointerMode to WhenRequested.
  Without this step, the app defaults to using pointer mode everywhere.
- Customize directional navigation to improve navigation targeting.
- Customize directional navigation to prevent navigation in a particular direction.
- A custom control which requires the user to press the "A" button on the game controller in order to engage the control.
- Mitigations for custom controls which are not aware of directional navigation.

When the IsFocusEngagementEnabled property is set to True,
the control is not given focus upon navigation.
The user must press
the "A" button on the game controller
in order to give the control focus,
and press
the "B" button to remove focus
from the control and allow the directional navigation
buttons to be used for navigation.

For information about directional navigation in JavaScript apps,
see [WinJS.UI.XYFocus](https://msdn.microsoft.com/library/windows/apps/dn904246.aspx).

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Reference

[XYFocusDown property](https://msdn.microsoft.com/library/windows/apps/xaml/windows.ui.xaml.controls.control.xyfocusdown.aspx)  
[XYFocusLeft property](https://msdn.microsoft.com/library/windows/apps/xaml/windows.ui.xaml.controls.control.xyfocusleft.aspx)  
[XYFocusRight property](https://msdn.microsoft.com/library/windows/apps/xaml/windows.ui.xaml.controls.control.xyfocusright.aspx)  
[XYFocusUp property](https://msdn.microsoft.com/library/windows/apps/xaml/windows.ui.xaml.controls.control.xyfocusup.aspx)  
[IsFocusEngaged property](https://msdn.microsoft.com/library/windows/apps/xaml/windows.ui.xaml.controls.control.isfocusengaged.aspx)  
[IsFocusEngagementEnabled property](https://msdn.microsoft.com/library/windows/apps/xaml/windows.ui.xaml.controls.control.isfocusengagementenabled.aspx)  

[Designing for Xbox and TV](https://msdn.microsoft.com/windows/uwp/input-and-devices/designing-for-tv)  
* [XY focus navigation and interaction](https://msdn.microsoft.com/windows/uwp/input-and-devices/designing-for-tv#xy-focus-navigation-and-interaction)  
* [Mouse mode](https://msdn.microsoft.com/windows/uwp/input-and-devices/designing-for-tv#mouse-mode)  
* [Focus engagement](https://msdn.microsoft.com/windows/uwp/input-and-devices/designing-for-tv#focus-engagement)  

## System requirements

**Client:** Windows 10 build 14332

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10 build 14332

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
