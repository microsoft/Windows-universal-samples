---
page_type: sample
languages:
- csharp
- cpp
- cppcx
products:
- windows
- windows-uwp
urlFragment: WiFiScan
extendedZipContent:
- path: SharedContent
  target: SharedContent
- path: LICENSE
  target: LICENSE
description: "Shows how to use the Windows.Devices.WiFi APIs."
---

<!---
  category: NetworkingAndWebServices 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620628
--->

# Wi-Fi scanning sample

Shows how to use the [Windows.Devices.WiFi](https://docs.microsoft.com/en-us/uwp/api/Windows.Devices.WiFi) APIs.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> You can download this sample as a standalone ZIP file
> [from docs.microsoft.com](https://docs.microsoft.com/samples/microsoft/windows-universal-samples/wifiscan/),
> or you can download the entire collection as a single
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample demonstrates:

-   Viewing a cached list of available networks on each WiFi adapter on the system
-   Requesting a scan on a WiFi adapter and and viewing the results
-   Registering for available network changed notifications on a WiFi adapter
-   Connecting to a WiFi network

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:**  Windows 10

# Related topics

## Related samples
* [On-Demand Hotspot](/Samples/OnDemandHotspot/)

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

Each scenario has buttons you can use to perform the actions described in the scenario text. If you do not have any WiFi Adapters on your system, you will not be able to experience the scenarios.
