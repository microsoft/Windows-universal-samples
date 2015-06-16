# x:DeferLoadStrategy sample

This sample shows how to reduce your app's startup time by deferring the creation of elements defined in your markup until you really need them.

Specifically, this sample shows how to:

- **Create incidental UI when needed:** By responding to an event and calling FindName, you can realize your elements based on user interaction.
- **Create adaptive UIs that are less expensive:** By deferring elements and then targeting them with the VisualStateManager, you can reduce the footprint of your adaptive UIs in form factors that don't match the specified conditions of the VisualState.
- **Defer control template parts:** If you are creating custom controls that has pieces that you expect to not be used as often, you can defer those pieces of the control and only create them if the conditions under which those template parts are used is met.

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10, go to [Windows 10](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics


### Reference

<!-- Add links to related API -->

[x:DeferLoadStrategy](http://msdn.microsoft.com/library/windows/apps/)

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
