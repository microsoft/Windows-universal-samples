<!---
  category: DevicesSensorsAndPower
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620529
--->

# Custom serial device sample

Shows how to use the [Windows.Devices.SerialCommunication](https://msdn.microsoft.com/library/windows/apps/windows.devices.serialcommunication.aspx) 
namespace to communicate with a serial device.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample allows the user to configure and communicate with a Serial device. You can choose one of four scenarios:

-   Connect/Disconnect using Device Selection list
-   Configure the Serial device
-   Communicate with the Serial device
-   Register for Events on the Serial device

**Connect/Disconnect using Device Selection list**

When the application starts, a list of available Serial devices matching the search criterion specified by the application is displayer along with options to Connect/Disconnect to/from a selected device.

**Configure the Serial device**

This scenario demonstrates the use of various Get/Set property APIs in order to query for/alter Serial device properties such as Baud Rate, Stop Bits etc.

**Communicate with the Serial device**

This scenario demonstrates the use of Input and Output streams on the SerialDevice object in order to communicate with the Serial device.

**Register for Events on the Serial device**

This scenario demonstrates the use of event notification APIs provided by Windows.Devices.SerialCommunication for **Pin Changed** and **Error Received** event types.

## Operating system requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

To run this sample after building it, press F5 (run with debugging enabled) or Ctrl+F5 (run without debugging enabled) from Visual Studio. (Or select the corresponding options from the Debug menu.)
