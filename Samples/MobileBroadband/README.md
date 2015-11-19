<!---
  category: NetworkingAndWebServices 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620572
--->

# Mobile broadband sample

This sample shows how to use the Mobile Broadband Networking API (Windows.Networking.NetworkOperators) using following scenarios:
- Displays device information by account
- Listening for changes on mobile broadband devices by account
- Displays device information by modem
- Enumerates device services
- Display SIM card information

## Prerequisites

This sample accesses privileged APIs used by mobile broadband operators.
Hence it requires a [special-use capability](https://msdn.microsoft.com/library/windows/apps/mt270968#special_and_restricted_capabilities)
to be declared in package manifest.

### Remarks

- This sample will not pass WACK because it uses a special-use capability.
- This samples requires phone devices to be OEM developer unlocked.

### Reference

[Mobile broadband API reference](https://msdn.microsoft.com/en-us/library/windows/apps/windows.networking.networkoperators.aspx)

## System requirements

**Hardware:** Phone device or PC with mobile broadband modem

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

