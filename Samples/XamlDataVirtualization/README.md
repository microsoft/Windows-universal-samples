<!---
  category: ControlsLayoutAndText
  samplefwlink: http://go.microsoft.com/fwlink/?LinkID=703784
--->

# Data virtualization sample

Shows how to implement a data source for XAML list controls that implements data virtualization.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample shows how to:

- **Implement IItemsRangeInfo:** Use the IItemsRangeInfo interface to understand which items are in view or the buffer area for a list view, and to manage a cache containing those items.
- **Implement ISelectionInfo:** Use the ISelectionInfo interface to manage the selection of items in the list, accounting for the data virtualization.

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10, go to [Windows 10](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics
[ListView and GridView UI optimization](https://msdn.microsoft.com/library/windows/apps/mt204776.aspx)

### Samples

[XAML data binding](https://code.msdn.microsoft.com/windowsapps/Data-Binding-7b1d67b5/)  
[XAML data binding with x:Bind](../XamlBind)  

### Reference

[IItemsRangeInfo](https://msdn.microsoft.com/library/windows/apps/windows.ui.xaml.data.iitemsrangeinfo.aspx)  
[ISelectionInfo](https://msdn.microsoft.com/library/windows/apps/windows.ui.xaml.data.iselectioninfo.aspx)  

## System requirements

**Client:** Windows 10 

**Server:** Windows Server 2016 Technical Preview

**Phone:**  Windows 10 

## Build the sample

1. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2. Go to the directory to which you unzipped the sample. Double-click the Visual Studio 2015 Solution (.sln) file. 
3. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**. 

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 
- The sample uses the StorageFolder API to read the contents of the Pictures folder, if you don't see any results in the sample, its likely that no pictures could be found. 