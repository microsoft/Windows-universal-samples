---
page_type: sample
languages:
- csharp
- cpp
- cppcx
products:
- windows
- windows-uwp
urlFragment: LightSensor
extendedZipContent:
- path: SharedContent
  target: SharedContent
- path: LICENSE
  target: LICENSE
description: "Shows how to use the Windows.Devices.Sensors.LightSensor API."
---

<!---
  category: DevicesSensorsAndPower
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620561
--->

# Light sensor sample

Shows how to use the [Windows.Devices.Sensors.LightSensor](http://msdn.microsoft.com/library/windows/apps/br225790) API.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> You can download this sample as a standalone ZIP file
> [from docs.microsoft.com](https://docs.microsoft.com/samples/microsoft/windows-universal-samples/lightsensor/),
> or you can download the entire collection as a single
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample allows the user to view the ambient light reading as a LUX value. You can choose one of two scenarios:

-   LightSensor data events
-   Poll light sensor readings

### LightSensor Data Events

When you click the **Enable** button for the **Data Events** option, the app begins streaming light-sensor readings in real time.

### Poll LightSensor Readings

When you click the **Enable** button for the **Polling** option, the app will retrieve the sensor readings at a fixed interval.

## Related topics

### Reference

[Windows.Devices.Sensors namespace](http://go.microsoft.com/fwlink/p/?linkid=241981)  

### Related samples

* [LightSensor sample](/archived/LightSensor/) for JavaScript (archived)

## System requirements

* Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 
