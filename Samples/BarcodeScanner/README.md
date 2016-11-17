<!---
  category: DevicesSensorsAndPower
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620014
--->

# Barcode scanner sample

Shows how to obtain a barcode scanner, claim it for exclusive use, enable it to receive data, and read a barcode.
To use a camera to scan bar codes, see the [Camera Barcode Scanner sample](/Samples/CameraBarcodeScanner) instead. 

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample shows how to:

1.  **Obtain the barcode scanner**

    Uses [BarcodeScanner.GetDefaultAsync](http://msdn.microsoft.com/library/windows/apps/dn263790) to get the first available barcode scanner.

2.  **Claim the barcode scanner for exclusive use**

    Uses [ClaimScannerAsync](http://msdn.microsoft.com/library/windows/apps/dn297696) to claim the device.

3.  **Add event handlers**

    Uses [DataReceived](http://msdn.microsoft.com/library/windows/apps/dn278556) and [ReleaseDeviceRequested](http://msdn.microsoft.com/library/windows/apps/dn278578) events.

    When an application gets a request to release its exclusive claim to the barcode scanner, it must handle the request by retaining the device; otherwise, it will lose its claim. The second scenario in this sample shows the release and retain functionality. The event handler for [ReleaseDeviceRequested](http://msdn.microsoft.com/library/windows/apps/dn278578) shows how retain the device.

The app package manifest shows how to specify the device capability name for the Point of Service (POS) devices. All POS apps are required declare [DeviceCapability](http://msdn.microsoft.com/library/windows/apps/br211430) in the app package manifest, either by using "PointofService" as shown in this sample or by using a device specific GUID, such as "C243FFBD-3AFC-45E9-B3D3-2BA18BC7EBC5" for a barcode scanner.

The following list shows the barcode scanners that were used with this sample:

-   Honeywell 1900GSR-2
-   Honeywell 1200g-2
-   Intermec SG20

In addition to the devices listed, you can use barcode scanners from various manufacturers that adhere to the [USB HID POS Scanner specification](http://go.microsoft.com/fwlink/p/?linkid=309230).

**Note** The Universal Windows app samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples

[Camera Barcode Scanner sample](/Samples/CameraBarcodeScanner)  

### Reference

[Windows.Devices.PointOfService namespace](http://msdn.microsoft.com/library/windows/apps/dn298071)  
[USB HID POS Scanner specification](http://go.microsoft.com/fwlink/p/?linkid=309230)  
[Windows app samples](http://go.microsoft.com/fwlink/p/?LinkID=227694)  

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
