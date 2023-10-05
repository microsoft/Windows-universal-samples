---
page_type: sample
languages:
- csharp
- cppwinrt
- cpp
products:
- windows
- windows-uwp
urlFragment: LampArray
extendedZipContent:
- path: SharedContent
  target: SharedContent
- path: LICENSE
  target: LICENSE
description: "Shows how to control RGB lighting of compatible peripheral devices."
---

<!---
  category: DevicesSensorsAndPower

-->

# LampArray sample

This sample shows how to control RGB lighting of peripheral devices using the [Windows.Devices.Lights](https://learn.microsoft.com/en-us/uwp/api/windows.devices.lights) and [Windows.Devices.Lights.Effects](https://learn.microsoft.com/en-us/uwp/api/windows.devices.lights.effects) APIs.

Supported devices conform to the [HID Lighting and Illumination Standard](https://www.usb.org/sites/default/files/hutrr84_-_lighting_and_illumination_page.pdf).

Specifically, this sample shows how to:

- Use [Windows.Devices.Enumeration.DeviceWatcher](https://learn.microsoft.com/en-us/uwp/api/windows.devices.enumeration.devicewatcher) with an AQS filter to register for LampArray attach and removal events.

- Obtain instances of [Windows.Devices.Lights.LampArray](https://learn.microsoft.com/en-us/uwp/api/windows.devices.lights.lamparray) and use them to query device properties and change RGB lighting colors and brightness.

- Create compelling RGB lighting effects using the [Windows.Devices.Lights.Effects](https://learn.microsoft.com/en-us/uwp/api/windows.devices.lights.effects) APIs.

- Create an app package with the "com.microsoft.windows.lighting" AppExtension. The sample will assume control of lighting when in the foreground. Declaring the "com.microsoft.windows.lighting" AppExtension will allow the sample to appear in the Dynamic Lighting page of Windows Settings, where it can be prioritized for lighting control when not in the foreground (supported on Windows build 23466 and above).

**Note** The Windows universal samples require Visual Studio to build and Windows 10 to execute.

To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421).

To obtain information about Microsoft Visual Studio and the tools for developing Windows apps, go to [Visual Studio](http://go.microsoft.com/fwlink/?LinkID=532422).

### Reference

[Windows.Devices.Lights](https://learn.microsoft.com/en-us/uwp/api/windows.devices.lights)

[Windows.Devices.Lights.Effects](https://learn.microsoft.com/en-us/uwp/api/windows.devices.lights.effects)

[Windows.Devices.Enumeration.DeviceWatcher](https://learn.microsoft.com/en-us/uwp/api/windows.devices.enumeration.devicewatcher)

### Conceptual

[HID Lighting and Illumination Standard](https://www.usb.org/sites/default/files/hutrr84_-_lighting_and_illumination_page.pdf)

[Dynamic Lighting](https://learn.microsoft.com/en-us/windows/uwp/devices-sensors/lighting-dynamic-lamparray) - This page also describes how to declare the "com.microsoft.windows.lighting" AppExtension for background (ambient) lighting control.

[Dynamic lighting devices](https://learn.microsoft.com/en-us/windows-hardware/design/component-guidelines/dynamic-lighting-devices) - This page contains a list of devices known to be compatible with the above HID Lighting standard.

## System requirements

**Client:** Windows 10, version 17763 or above.

**Server:** Not supported.

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++ or C#). Double-click the Visual Studio Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug > Start Debugging. To run the sample without debugging, press Ctrl+F5 or select Debug > Start Without Debugging.
