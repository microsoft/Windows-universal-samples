<!---
  category: Communications
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620605&clcid=0x409
--->

# SMS send and receive sample

This sample shows how to use the SMS API (Windows.Devices.Sms) using following scenarios:
- Send text message.
- Set filter rules for registering with background infrastructure and then receiving the SMS based on filter rules.

## Prerequisites

This sample accesses privileged APIs and used by mobilebroadband operators.
Hence requires restricted capability to be defined in package manifest.

### Remark

- Since this sample uses restricted capability, this will fail WACK for restricted namespace test.
- This also requires phone devices to be OEM developer unlocked.

### Reference

[SMS API reference](https://msdn.microsoft.com/en-us/library/windows/apps/windows.devices.sms.aspx)

## System requirements

**Hardware:** Phone device or PC with mobilebroadband modem

**Client:** Windows 10 

**Phone:** Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 
