<!---
  category: DevicesSensorsAndPower
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620591
--->

# Relative inclinometer sample

Shows how to use the [Windows.Devices.Sensors.Inclinometer](https://msdn.microsoft.com/library/windows/apps/windows.devices.sensors.inclinometer.aspx) 
class for the Relative Inclinometer sensor.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample allows the user to view the angles of incline about the X-, Y-, and Z-axes for a 3-axis relative inclinometer. 
(The incline about the X-axis corresponds to the pitch value; the incline about the Y-axis correspond to the roll value; 
and the incline about the Z-axis corresponds to the yaw value.) You can choose one of two scenarios:

-   Relative inclinometer data events
-   Polling relative inclinometer readings

### Relative Inclinometer Data Events

When you choose the Enable button for the **Data Events** option, the app will begin streaming relative inclinometer readings in real time.

### Polling Relative Inclinometer Readings

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
