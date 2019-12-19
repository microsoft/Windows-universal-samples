---
page_type: sample
languages:
- csharp
- cpp
- cppcx
products:
- windows
- windows-uwp
urlFragment: BarcodeScannerProvider
extendedZipContent:
- path: SharedContent
  target: SharedContent
- path: LICENSE
  target: LICENSE
description: "Demonstrates a basic OCR-based barcode scanner provider."
---

<!---
  category: DevicesSensorsAndPower
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=2033958
--->

# Barcode scanner provider sample

Demonstrates a basic OCR-based barcode scanner provider.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> You can download this sample as a standalone ZIP file
> [from docs.microsoft.com](https://docs.microsoft.com/samples/microsoft/windows-universal-samples/barcodescannerprovider/),
> or you can download the entire collection as a single
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample shows how to:

1.  **Register as a barcode scanner provider**

    The sample declares the `windows.barcodeScannerProvider` extension in the application manifest.
    It also declares the `cameraProcessingExtension` restricted capability
    which permits the app to process images captured from the camera without direct camera control.

2.  **Respond to inbound connections**

    The `BarcodeDecodeTask` runs when an inbound connection is created.
    The `Provider` registers for the necessary events in order to provide barcode scanning services
    to the connection.
    When the software trigger starts, the provider performs OCR on captured frames
    and reports the results.

3. **Configure the barcode scanner provider**

    When the user runs the app, it allows the user to configure the
    language used for OCR.

You can use the [Barcode Scanner](../BarcodeScanner) sample to test the sample barcode sample provider.
Go to the "Displaying a Barcode Preview" scenario and select the Barcode Scanner Provider Sample
as your barcode scanner.

**Note** The Universal Windows app samples require Visual Studio to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio and the tools for developing Windows apps, go to [Visual Studio](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples

[Barcode Scanner](../BarcodeScanner)

### Reference

[Windows.Devices.PointOfService namespace](http://msdn.microsoft.com/library/windows/apps/dn298071)  
[Windows app samples](http://go.microsoft.com/fwlink/p/?LinkID=227694)  

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

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
