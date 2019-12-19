---
page_type: sample
languages:
- csharp
products:
- windows
- windows-uwp
urlFragment: Magnetometer
extendedZipContent:
- path: SharedContent
  target: SharedContent
- path: LICENSE
  target: LICENSE
description: "Shows how to use the Magnetometer class."
---

<!---
  category: DevicesSensorsAndPower
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=869539
--->

# Magnetometer sample

Shows how to use the [Magnetometer](http://msdn.microsoft.com/library/windows/apps/br225687) class.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> You can download this sample as a standalone ZIP file
> [from docs.microsoft.com](https://docs.microsoft.com/samples/microsoft/windows-universal-samples/magnetometer/),
> or you can download the entire collection as a single
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

The sample asks you to choose an magnetometer.

After choosing your magnetometer, you can choose one of these scenarios:

-   Magnetometer data events
-   Poll magnetometer readings

### Magnetometer Data Events

When you choose the **Enable** button for the **Data Events** option, the app begins streaming magnetometer readings in real time.

### Poll Magnetometer Readings

When you choose the **Enable** button for the **Polling** option, the app will retrieve the sensor readings at a fixed interval.

### Magnetometer data events batching

When you choose the **Enable** button for the **Data Events** option, the app begins streaming magnetometer readings. The readings may be delivered in batches if the device supports data batching.

## Related topics

### Reference

[Magnetometer Class](https://docs.microsoft.com/en-us/uwp/api/Windows.Devices.Sensors.Magnetometer)

[Magnetometer.ReadingChanged event handler](https://docs.microsoft.com/en-us/uwp/api/windows.devices.sensors.magnetometerreadingchangedeventargs)

[Calibrate sensors](https://docs.microsoft.com/en-us/windows/uwp/devices-sensors/calibrate-sensors)

[Windows.Devices.Sensors namespace](http://go.microsoft.com/fwlink/p/?linkid=241981)

## System requirements

**Client:** Windows 10 build 14295

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10 build 14295

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language. Double-click the Visual Studio Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 

