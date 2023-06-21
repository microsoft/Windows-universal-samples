---
page_type: sample
languages:
- csharp
- cpp
- cppwinrt
products:
- windows
- windows-uwp
urlFragment: OnDemandHotspot
extendedZipContent:
- path: SharedContent
  target: SharedContent
- path: LICENSE
  target: LICENSE
description: "Shows how to integrate an on-demand Wi-Fi hotspot with Windows."
---

<!---
  category: NetworkingAndWebServices 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620628
--->

# Wi-Fi on-demand hotspot sample

Shows how to use the [Windows.Devices.WiFi.WiFiOnDemandHotspotNetwork](https://docs.microsoft.com/uwp/api/Windows.Devices.WiFi.WiFiOnDemandHotspotNetwork) APIs. 

An on-demand hotspot is a Wi-Fi hotspot that can be enabled programmatically.
When you create an on-demand hotspot and mark it as available,
the system includes the hotspot in the list of available Wi-Fi networks.
If the user chooses to connect to it, the system triggers the application's background task
to turn on the hotspot.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> You can download this sample as a standalone ZIP file
> [from docs.microsoft.com](https://docs.microsoft.com/samples/microsoft/windows-universal-samples/ondemandhotspot/),
> or you can download the entire collection as a single
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample demonstrates:

-   Registering and unregistering the background tasks ([Windows.ApplicationModel.Background.WiFiOnDemandHotspotUpdateMetadataTrigger](https://docs.microsoft.com/uwp/api/windows.applicationmodel.background.wifiondemandhotspotupdatemetadatatrigger) and [Windows.ApplicationModel.Background.WiFiOnDemandHotspotConnectTrigger](https://docs.microsoft.com/uwp/api/windows.applicationmodel.background.wifiondemandhotspotconnecttrigger)) for managing on-demand hotspots
-   Showing the on-demand hotspot in the list of available Wi-Fi networks
-   Reporting signal strength and other information about the on-demand hotspot
-   Showing when to turn on the on-demand hotspot when the user chooses to connect to it and providing the connection SSID and password

> **Note:** Note that the SCCD file included with this sample is not valid,
> but it is included for instructional purposes.
> If the PC is Developer Mode, the sample will deploy despite the invalid SCCD file.
> When writing your own app, follow the instructions in
> [Custom Capabilities for Universal Windows Platform apps](https://msdn.microsoft.com/windows/hardware/drivers/devapps/custom-capabilities-for-universal-windows-platform-apps)
> to obtain a properly-signed SCCD file.

## System requirements

**Client:** Windows 11, Build 22621 and later.

**Server:** N/A

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++ or C#). Double-click the Visual Studio Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select **Build** \> **Deploy Solution**. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or select **Debug** \> **Start Without Debugging**. 

Each scenario has buttons you can use to perform the actions described in the scenario text. If you do not have any WiFi Adapters on your system, you will not be able to experience the scenarios.
