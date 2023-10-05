---
page_type: sample
languages:
- csharp
- cppwinrt
products:
- windows
- windows-uwp
urlFragment: MobileHotspot
extendedZipContent:
- path: SharedContent
  target: SharedContent
- path: LICENSE
  target: LICENSE
description: "Shows how to use the Windows.Networking.NetworkOperators.NetworkOperatorTetheringAccessPointConfiguration APIs."
---

# Mobile hotspot sample

Shows how to use the [Windows.Networking.NetworkOperators.NetworkOperatorTetheringAccessPointConfiguration](https://docs.microsoft.com/en-us/uwp/api/Windows.Networking.NetworkOperators.NetworkOperatorTetheringAccessPointConfiguration) APIs. 

> **Note:** This sample is part of a large collection of UWP feature samples. 
> You can download this sample as a standalone ZIP file
> [from docs.microsoft.com](https://docs.microsoft.com/samples/microsoft/windows-universal-samples/mobilehotspot/),
> or you can download the entire collection as a single
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample demonstrates:

- Reading the current Mobile hotspot configuration
- Editing and applying a new configuration ([Windows.Networking.NetworkOperators.NetworkOperatorTetheringAccessPointConfiguration](https://docs.microsoft.com/uwp/api/Windows.Networking.NetworkOperators.NetworkOperatorTetheringAccessPointConfiguration))
- Turning the mobile hotspot on and off

## System requirements

**Client:** Windows 10, Build 19041 and later.

**Server:** N/A

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select **Build** \> **Deploy Solution**. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or select **Debug** \> **Start Without Debugging**. 

Each scenario has buttons you can use to perform the actions described in the scenario text. If you do not have any WiFi Adapters on your system and an active Internet connection to share, you will not be able to experience the scenarios.
