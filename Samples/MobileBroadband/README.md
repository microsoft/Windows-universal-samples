<!---
  category: NetworkingAndWebServices 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620572
--->

# Mobile broadband sample

Shows how to use the Mobile Broadband Networking API 
([Windows.Networking.NetworkOperators](https://msdn.microsoft.com/library/windows/apps/windows.networking.networkoperators.aspx)). 

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample shows the following scenarios:
- Displaying device information by account
- Listening for changes on mobile broadband devices by account
- Displaying device information by modem
- Enumerating device services
- Displaying SIM card information

## Prerequisites

This sample accesses privileged APIs used by mobile broadband operators.
Hence it requires a [special-use capability](https://msdn.microsoft.com/library/windows/apps/mt270968#special_and_restricted_capabilities)
to be declared in package manifest.

### Remarks

- This sample will not pass WACK because it uses a special-use capability.
- This samples requires phone devices to be OEM developer unlocked.

### Reference

[Mobile broadband API reference](https://msdn.microsoft.com/library/windows/apps/windows.networking.networkoperators.aspx)

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

