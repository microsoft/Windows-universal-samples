<!---
  category: DevicesSensorsAndPower
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620548
--->

# Gyrometer sample

This sample shows how to use the [**Windows.Devices.Sensors.Gyrometer**](http://msdn.microsoft.com/library/windows/apps/br225718) API.

This sample allows the user to view the angular velocity along the X-, Y-, and Z-axis for a 3-axis gyrometer. You can choose one of three scenarios:

-   Gyrometer data events
-   Poll gyrometer readings
-   Porting gyrometer logic across platforms

### Gyrometer Data Events

When you click the **Enable** button for the **Data Events** option, the app begins streaming gyrometer readings in real time.

### Poll Gyrometer Readings

When you click the **Enable** button for the **Polling** option, the app will retrieve the sensor readings at a fixed interval.

### Porting Gyrometer Logic Across Platforms

When you click the **Get Sample** button, two sets of readings are displayed: raw gyrometer reading, based on the native orientation of the device, and transformed gyrometer reading based on the native orientation of the device your sensor logic was originally developed for.

### Related topics

[Windows.Devices.Sensors namespace](http://go.microsoft.com/fwlink/p/?linkid=241981)

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
