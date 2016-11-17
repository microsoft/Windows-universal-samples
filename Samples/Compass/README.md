<!---
  category: DevicesSensorsAndPower
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620521
--->

# Compass sample

Shows how to use the [Windows.Devices.Sensors.Compass](http://msdn.microsoft.com/library/windows/apps/br225705) API.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample allows the user to view the compass reading as a magnetic-north and, depending on the installed sensor, a true-north value. You can choose one of three scenarios:

-   Compass data events
-   Poll compass readings
-   Sensor calibration

### Compass Data Events

When you choose the **Enable** button for the **Data Events** option, the app begins streaming compass readings in real time.

### Poll Compass Readings

When you choose the **Enable** button for the **Polling** option, the app will retrieve the sensor readings at a fixed interval.

### Sensor Calibration

Allows the user to simulate sensor accuracy and demonstrates usage of the calibration bar.

## Related topics

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
