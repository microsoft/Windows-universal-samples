<!---
  category: DevicesSensorsAndPower
--->

# Background Sensors sample

This sample shows how to use background tasks with a device use trigger targetting sensors APIs. For the sake of the example, the [**Accelerometer**](http://msdn.microsoft.com/library/windows/apps/br225687) API is used in this sample.

**Note** The Universal Windows app samples require Visual Studio 2015 to build and Windows 10 to execute.

This sample allows the user to view the event count. Only one scenario is available:

-   Accelerometer Device Use Trigger

**Accelerometer Device Use Trigger**

When you choose the **Enable** button for the **Accelerometer DeviceUse** option, the app registers a DeviceUseTrigger background task that listens for accelerometer readings and counts them.

Related topics
--------------

[Quickstart: Responding to user movement with the accelerometer](http://msdn.microsoft.com/library/windows/apps/hh465265)

[Windows.Devices.Sensors namespace](http://go.microsoft.com/fwlink/p/?linkid=241981)

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

## Build the sample

1. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2. Go to the directory to which you unzipped the sample. Then go to the subdirectory containing the sample in the language you desire - either C++, or C#. Double-click the Visual Studio 2015 Solution (.sln) file. 
3. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**. 

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 
