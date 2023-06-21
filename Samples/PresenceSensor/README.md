---
page_type: sample
languages:
- csharp
- cpp
- cppwinrt
products:
- windows
- windows-uwp
urlFragment: PresenceSensor
extendedZipContent:
- path: SharedContent
  target: SharedContent
- path: LICENSE
  target: LICENSE
description: "Shows how to use the Windows.Devices.Sensors.HumanPresenceSensor API."
---

<!---
  category: DevicesSensorsAndPower
--->

# Presence sensor sample

Shows how to use the [Windows.Devices.Sensors.HumanPresenceSensor](https://docs.microsoft.com/en-us/uwp/api/windows.devices.sensors.humanpresencesensor) API.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> You can download this sample as a standalone ZIP file
> [from docs.microsoft.com](https://docs.microsoft.com/samples/microsoft/windows-universal-samples/presencesensor/),
> or you can download the entire collection as a single
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample locates presence sensors and reads data from them.
It includes the following scenarios:

-   Obtaining the default presence sensor
-   Choosing a presence sensor from among the available sensors
-   Letting the user choose a presence sensor
-   Detecting the arrival and removal of presence sensors
-   Presence sensor data events
-   Polling presence sensor readings

**Presence Sensor Data Events**

When you click the "Get default sensor" button,
the app will display data from the default presence sensor,
updating it in real time.

**Polling Presence Sensor Readings**

When you choose the "Get a sensor" button,
the app will find all presence sensors and
choose one to use.
It will then read and display the current reading from that sensor.

***Choosing a Presence Sensor***

When you choose the "Use picker" button,
the app will use a DevicePicker to let the user
choose a presence sensor.
When one is chosen, the app displays the current reading from that sensor.

When you choose the "Start watcher" button,
the app will use a DeviceWatcher to identify all the
presence sensors and monitor their arrival and removal
in real time.
If you select a sensor from the list and click "Read selected sensor",
the app will read and display the current reading from that sensor.

## Related topics

### Related samples

* [ProximitySensor sample](/ProximitySensor)
* [DeviceEnumerationAndPairing sample](/DeviceEnumerationAndPairing) shows othe operations with devices

## System requirements

* Windows 11 build 22624 or higher

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
