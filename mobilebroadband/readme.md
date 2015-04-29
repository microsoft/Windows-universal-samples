# Mobilebroadband Sample

This sample shows how to use the Mobile Broadband Networking API (Windows.Networking.NetworkOperators) using following scenarios:
- Displays device information by account
- Listening for changes on mobilebroadband devices by account
- Displays device information by modem
- Enumerates device services
- Display SIM card information

## Prerequisites

This sample accesses privileged APIs and used by mobilebroadband operators.
Hence requires restricted capability to be defined in package manifest.

### Remark

- Since this sample uses restricted capability, this will fail WACK for restricted namespace test.
- This also requires phone devices to be OEM developer unlocked.
- This sample is not supported on Phone emulator.

### Reference

[Mobilebroadband API reference](https://msdn.microsoft.com/en-us/library/windows/apps/windows.networking.networkoperators.aspx)

## System requirements

**Hardware:** Phone device or PC with mobilebroadband modem

**Client:** Windows 10 Technical Preview 

**Phone:**  Windows 10 Technical Preview

## Build the sample

1. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**. 

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 

