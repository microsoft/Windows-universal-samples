<!---
  category: ControlsLayoutAndText
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620019
--->

# Commanding sample

Shows the changes and new features for the CommandBar.  On Windows 10, both the AppBar and the CommandBar are visible by default and can be either 
"closed" or "opened". While in the closed state, an app bar can provide either a "minimal" or "compact" hint or no hint at all.  
The AppBar/CommandBar transition to an opened state when the user taps the More button that appears as an ellipsis or the control is opened programmatically. 

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample shows:

- **Buttons for the CommandBar:** The AppBarButton, AppBarToggleButton, and AppBarSeparator can be used as any other button, but are 
optimized for use on the CommandBar control.  A soft-hyphen character can be used to hint at where the label should wrap. 
- **Default Icons:** The default set of icons for Windows 10 comes from the Segoe MDL2 Assets font.  The font metrics are such that the 
glyphs can be used similar to scalable vector images where they fill the specified size.
- **Opening/Closing events:** This sample demonstrates the new Opening and Closing events and how the existing Opened and Closed events 
fire after the animation completes.
- **Styling:** The CommandBar includes an overflow menu which can be styled separate from the bar which displays the PrimaryCommands.
- **Hosting Custom Content:** The CommandBar now provides support for displaying UI set as the Content.  This enables more customized 
commanding scenarios while still utilizing the structured commanding surface for Primary or Secondary commands in addition to attached Flyouts.
- **Adapting to Screen Sizes:** The default control does not reflow commands to/from the overflow as the available space changes.  
This demonstrates how it can be achieved by subclassing the CommandBar and including additionl logic in the control's MeasureOverride.  

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples

[Context Menus](/Samples/XamlContextMenu)  
[UI Basics](/Samples/XamlUIBasics)  
[Navigation](/Samples/XamlNavigation)  

### Reference

[AppBar](https://msdn.microsoft.com/library/windows/apps/windows.ui.xaml.controls.appbar.aspx)  
[AppBarButton](https://msdn.microsoft.com/library/windows/apps/windows.ui.xaml.controls.appbarbutton.aspx)  
[AppBarToggleButton](https://msdn.microsoft.com/library/windows/apps/windows.ui.xaml.controls.appbartogglebutton.aspx)  
[AppBarSeparator](https://msdn.microsoft.com/library/windows/apps/windows.ui.xaml.controls.appbarseparator.aspx)  
[CommandBar](https://msdn.microsoft.com/library/windows/apps/windows.ui.xaml.controls.commandbar.aspx)  
[Flyout](https://msdn.microsoft.com/library/windows/apps/windows.ui.xaml.controls.flyout.aspx)  
[MenuFlyout](https://msdn.microsoft.com/library/windows/apps/windows.ui.xaml.controls.menuflyout.aspx)  

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
