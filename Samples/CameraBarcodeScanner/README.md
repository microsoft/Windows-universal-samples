<!---
  category: DevicesSensorsAndPower
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=799154
--->

# Camera Barcode Scanner sample

This sample uses the [**Windows.Devices.PointOfService.BarcodeScanner**](http://msdn.microsoft.com/library/windows/apps/dn298071) class
to use a camera to scan bar codes.

Specifically, this sample covers the following:
**Camera Barcode Scanner basic operation**
- "Start Scanning" to create the scanner for the default camera lens.
- "Start Software Trigger" to press the trigger of scanner to start to scan the barcode through camera preview window.
- "Stop Software Trigger" to release the trigger of scanner.
- "End Scanning" to release the scanner.

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples

[Barcode Scanner sample](/Samples/BarcodeScanner)

### Reference

[Windows.Devices.PointOfService](http://msdn.microsoft.com/library/windows/apps/dn298071)

## System requirements

**Client:** Windows 10 build 14332

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10 build 14332

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
