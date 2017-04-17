<!---
  category: DevicesSensorsAndPower
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=624151
--->

# Serial Peripheral Interface (SPI) sample

Shows how to use the [Windows.Devices.Spi](http://msdn.microsoft.com/library/windows.devices.spi.aspx) namespace
to allow apps to communicate with Serial Peripheral Interface (SPI) devices
on a Windows IoT (Internet of Things) device.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

SPI is a four-wire bus used to interface devices such as analog to digital converters,
digital to analog converters, IO expanders, displays, and embedded subsystems.
This sample shows how to access the SPI bus.  

The sample shows the following techniques:

- Reading data from an SPI device.

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 IoT Core to execute.

To obtain information about Windows 10 IoT Core, go to [Windows on Devices](http://windowsondevices.com).

To see more samples specifically describing how to leverage Windows 10 IoT Core, click on the [Samples tab here](http://ms-iot.github.io/content/win10/StartCoding.htm).

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## System requirements

**IoT:** Windows 10 IoT Core

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select the appropriate architecture for the device you want to deploy to (i.e. ARM for Raspberry Pi 2 or x86 for MinnowBoard Max)
- For C#, select Remote Machine from Debug > Target device in your Project properties.  For C++ and JavaScript, select Remote Machine from Debugger to launch dropdown in the Debugging tab of your Project properties.
- For C#, enter the target device name or IP address in Debug > Remote Machine in your Project properties.  For C++ and JavaScript, enter the target device name or IP address in Debugging > Machine Name in your Project properties.
- For C#, deselect the Use Athentication checkbox in the Debug tab of your Project properties.  For C++ and JavaScript, select No for Debugging > Require Authentication in your Project properties.
- Select Build > Deploy Solution. 

### Deploying and running the sample

- Select the appropriate architecture for the device you want to deploy to (i.e. ARM for Raspberry Pi 2 or x86 for MinnowBoard Max)
- For C#, select Remote Machine from Debug > Target device in your Project properties.  For C++ and JavaScript, select Remote Machine from Debugger to launch dropdown in the Debugging tab of your Project properties.
- For C#, enter the target device name or IP address in Debug > Remote Machine in your Project properties.  For C++ and JavaScript, enter the target device name or IP address in Debugging > Machine Name in your Project properties.
- For C#, deselect the Use Athentication checkbox in the Debug tab of your Project properties.  For C++ and JavaScript, select No for Debugging > Require Authentication in your Project properties.
- Press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or select Debug > Start Without Debugging. 
