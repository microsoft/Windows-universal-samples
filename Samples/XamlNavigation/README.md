<!---
  category: Navigation
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=619902
--->

# Navigation menu (XAML) sample

Shows how to provide a top-level navigation menu to users (also known as a hamburger menu).

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample shows how to:

- **Utilize a SplitView control:** The SplitView is a control with two content areas: the Pane and the Content.  
The Pane can be opened or closed.  Using the DisplayMode property the app can switch the Pane to Overlay the Content or appear Inline.  
When the available space is constrained the Pane can be put into a CompactOverlay or CompactInline mode where it provides a visual hint to the user. 
- **Customize a ListView to present menu items with a single-selection experience:** This uses a ListView to present the default visuals for the 
menu items and customizes the keyboarding behavior to provide a single selection model where up/down/tab/shift+tab moves through each item and 
'Enter' and 'Space' select the item. 
- **Position the CommandBar based on the screen size:** This sample moves the CommandBar to the bottom on screens less than seven inches diagonal.
- **Adapt UI to the ten-foot experience:** This sample uses a style sheet and a state trigger to adapt the UI when running on a TV.

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples

[Master Detail](/Samples/XamlMasterDetail)  
[RSS reader sample](https://github.com/Microsoft/Windows-appsample-rssreader)  
[Lunch Scheduler app sample](https://github.com/Microsoft/Windows-appsample-lunch-scheduler)  
[Customers Orders Database sample](https://github.com/Microsoft/Windows-appsample-customers-orders-database)  
[Photosharing app sample](https://github.com/Microsoft/Appsample-Photosharing)  

### Reference

[SplitView class](https://msdn.microsoft.com/library/windows/apps/windows.ui.xaml.controls.splitview.aspx)  
[ListView class](https://msdn.microsoft.com/library/windows/apps/windows.ui.xaml.controls.listview.aspx)  
[Designing for Xbox and TV](https://msdn.microsoft.com/windows/uwp/input-and-devices/designing-for-tv)  

## System requirements

**Client:** Windows 10 build 14332

**Server:** Windows Server 2016 Technical Preview

**Phone:**  Windows 10 build 14332

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
