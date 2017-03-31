<!---
  category: DevicesSensorsAndPower
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620584
--->

# Pedometer sample

Shows how to use the [Windows.Devices.Sensors.Pedometer](https://msdn.microsoft.com/library/windows/apps/windows.devices.sensors.pedometer.aspx) API.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample allows the user to view the default pedometer events and history available on the system.
It includes the following scenarios:

-   Events
-   History
-   Current steps count
-   Pedometer Background

**Events**

It demonstrates getting the default pedometer asynchronously.
When you click on the 'Register ReadingChanged' button, it registers to the 'ReadingChanged' event on the default pedometer and displays pedometer readings as they are notified.

**History**

This demonstrates usage of History APIs for pedometer.
When you click on the 'Get History' button, pedometer history for the requested timespan will be displayed. Two ways to choose the timespan is provided, which demonstrate the two overloaded ways of getting history.

**Current steps count**

When you click the 'Get steps count' button, it displays the last known step counts. This illustrates usage of GetCurrentReadings API.

**Background Pedometer**

This demonstrates using a Pedometer's step goal as a background trigger. 
When you click the 'Register Task' button, it gets the current step count from the default Pedometer and sets a step goal of 50 additional steps.
When the step goal is met, the registered background task gets fired. Pedometer readings are then retrieved from the trigger to update the scenario UI.

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

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
