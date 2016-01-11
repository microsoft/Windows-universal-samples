<!---
  category: DevicesSensorsAndPower
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620603
--->

# Simple orientation sensor sample

This sample demonstrates the use of the Windows.Devices.Sensors.SimpleOrientationSensor API for a simple device orientation sensor.

This sample allows the user to view the orientation values that reflect the simple device orientation (e.g. not rotated, face up, rotated 90 degrees counter-clockwise). You can choose one of two scenarios:

-   Orientation sensor data events
-   Polling orientation sensor readings

**Simple Orientation Sensor Data Events**

When you choose the Enable button for the **Data Events** option, the app will begin displaying sensor readings in real time.

**Polling Simple Orientation Sensor Readings**

When you choose the Enable button for the **Polling** option, the app will retrieve the current sensor readings.

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
