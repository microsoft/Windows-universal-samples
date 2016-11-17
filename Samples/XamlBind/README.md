<!---
  category: Data
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=619989
--->

# x:Bind sample

Shows how to use x:Bind for data binding in XAML apps. x:Bind is a new compile time binding mechanism for XAML in windows 10, 
which is faster and provides more developer feedback in the form of compile errors and generated code that can be inspected.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample shows how to:

- **Use x:Bind:** It includes a wide range of bindings that cover most usage patterns.
- **Use x:Bind in Data Templates:** Data Templates need to have the model type that will be used in the bindings specified on the template definition using the x;DataType attribute
- **Use x:Bind with x:Phase:** x:Phase can be used with x:Bind for list scenarios to enable incremental binding and rendering of data templates to improve the panning experience on low end devices.
- **Use x:Bind to bind event handlers:** x:Bind can be used in markup to specify event handlers as part of the data model, rather than requiring them to be in the code behind. 

New for Windows 10 Anniversary Update:
* Indexing dictionaries/maps with a string. (See Basic Bindings scenario.)
* Implicit bool-to-Visibility conversion. (See Other Bindings scenario.)
* C-style casts. (See Other Bindings scenario.)
* Function binding. (See Function Binding scenario.)

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Reference

[{x:Bind} markup extension](https://msdn.microsoft.com/windows/uwp/xaml-platform/x-bind-markup-extension)  

### Other samples

[Traffic App sample](https://github.com/microsoft/windows-appsample-trafficapp/)  
[RSS reader sample](https://github.com/Microsoft/Windows-appsample-rssreader)  
[Lunch Scheduler app sample](https://github.com/Microsoft/Windows-appsample-lunch-scheduler)  
[Customers Orders Database sample](https://github.com/Microsoft/Windows-appsample-customers-orders-database)  
[Hue Lights sample](https://github.com/Microsoft/Windows-appsample-huelightcontroller)  
[Network Helper sample library](https://github.com/Microsoft/Windows-appsample-networkhelper)  

## System requirements

**Client:** Windows 10 build 14366

**Server:** Windows Server 2016 Technical Preview

**Phone:**  Windows 10 build 14366

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
