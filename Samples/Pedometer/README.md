<!---
  category: DevicesSensorsAndPower
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620584&clcid=0x409
--->

# Pedometer sample

This sample shows how to use the  Windows.Devices.Sensors.Pedometer API.

This sample allows the user to view the default pedometer events and history available on the system. You can choose one of three scenarios:

-   Events
-   History
-   Current steps count

**Events**

It demonstrates getting the default pedometer asynchronously.
When you click on the 'Register ReadingChanged' button, it registers to the 'ReadingChanged' event on the default pedometer and displays pedometer readings as they are notified.

**History**

This demonstrates usage of History APIs for pedometer.
When you click on the 'Get History' button, pedometer history for the requested timespan will be displayed. Two ways to choose the timespan is provided, which demonstrate the two overloaded ways of getting history.

**Current steps count**

When you click the 'Get steps count' button, it displays the last known step counts. Since 'ReadingChanged' may not be fired when there is no activity, this relies on History to get the last known step counts.

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
