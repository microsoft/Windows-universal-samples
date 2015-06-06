# x:Bind Sample

This sample shows how to use x:Bind for data binding in XAML apps. x:Bind is a new compile time binding mechanism for XAML in windows 10, which is faster and provides more developer feedback in the form of compile errors and generated code that can be inspected.

Specifically, this sample shows how to:

- **Use x:Bind:** It includes a wide range of bindings that cover most usage patterns.
- **Use x:Bind in Data Templates:** Data Templates need to have the model type that will be used in the bindings specified on the template definition using the x;DataType attribute
- **Use x:Bind with x:Phase:** x:Phase can be used with x:Bind for list scenarios to enable incremental binding and rendering of data templates to improve the panning experience on low end devices.
- **Use x:Bind to bind event handlers:** x:Bind can be used in markup to specify event handlers as part of the data model, rather than requiring them to be in the code behind. 

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10, go to [Windows 10](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples

[x:Bind](https://github.com/Microsoft/Windows-universal-samples/tree/master/xaml_xBind)


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
