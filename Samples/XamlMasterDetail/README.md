<!---
  category: Navigation
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=619901
--->

# Master/detail sample

Shows how to implement a responsive master/detail experience in XAML. 

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

In the [master/detail pattern](https://msdn.microsoft.com/library/windows/apps/dn997765.aspx), a master list is used to select an item that will then appear in a detail view.
When the app view is sufficiently wide, the master list and detail view should appear side by side in the same app page. However, on smaller screen sizes, 
the two pieces of UI should appear on different pages, allowing the user to navigate between them. This sample shows how to implement these experiences and 
adaptively switch between them based on the size of the screen.

Specifically, this sample demonstrates:

- **Creating a side-by-side master/detail experience in XAML:** In MasterDetailPage.xaml, a master list is used to switch between detail items in a side-by-side view. This page will also responsively update to include just the master list when the view is narrow.
- **Navigating between the master list and a detail view:** At narrow screen sizes, the user can navigate between the master list in MasterDetailPage.xaml and detail view in DetailPage.xaml to view different items.
- **Changing the navigation model when the app is resized:** This sample contains the code necessary to adaptively switch between the two experiences described above at runtime based on screen size.

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples

[XAML Responsive Techniques](/Samples/XamlResponsiveTechniques)  
[Tailored Multiple Views](/Samples/XamlTailoredMultipleViews/)  
[RSS reader sample](https://github.com/Microsoft/Windows-appsample-rssreader)  

### Reference

[VisualState](https://msdn.microsoft.com/library/windows/apps/windows.ui.xaml.visualstate.aspx)  
[NavigationThemeTransition](https://msdn.microsoft.com/library/windows/apps/xaml/windows.ui.xaml.media.animation.navigationthemetransition.aspx)  

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
