---
page_type: sample
languages:
- csharp
- cpp
- cppcx
products:
- windows
- windows-uwp
urlFragment: MapControl
extendedZipContent:
- path: SharedContent
  target: SharedContent
- path: LICENSE
  target: LICENSE
description: "Shows how to use the universal map control (MapControl) in a UWP app."
---

<!---
  category: MapsAndLocation 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=619977
--->

# MapControl sample

Shows how to use the universal map control ([MapControl]( https://msdn.microsoft.com/library/windows/apps/xaml/windows.ui.xaml.controls.maps.mapcontrol.aspx)) in a UWP app. 

> **Note:** This sample is part of a large collection of UWP feature samples. 
> You can download this sample as a standalone ZIP file
> [from docs.microsoft.com](https://docs.microsoft.com/samples/microsoft/windows-universal-samples/mapcontrol/),
> or you can download the entire collection as a single
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample shows:

* MapControl Basics: adjusting the ZoomLevel, Heading, DesiredPitch, map style, and map projection
* Adding points of interest on the map: PushPins, images, billboards, 3D models and shapes
* Adding multiple layers (points of interests on the map) on the map
* Adding XAML overlays on the map
* Showing 3D locations in MapControl
* Showing Streetside experience within MapControl
* Launching Maps using URI Schemes
* Displaying points of interest with clustering
* Finding and downloading Offline Maps
* Customizing the map appearance by using style sheets
* Customizing the map elements appearance more deeply by using style entries and states
* Overlay and animate custom tiled images on the map

This sample is written in XAML.

## For more info

[Display maps with 2D, 3D, and Streetside views](https://msdn.microsoft.com/library/windows/apps/mt219695)  
[Display points of interest (POI) on a map](https://msdn.microsoft.com/library/windows/apps/mt219696)  
[MapControl API reference]( https://msdn.microsoft.com/library/windows/apps/xaml/windows.ui.xaml.controls.maps.mapcontrol.aspx)  
[Launch the Windows Maps app](https://msdn.microsoft.com/library/windows/apps/mt228341)  
[Traffic app sample](https://github.com/microsoft/windows-appsample-trafficapp/)  

## Operating system requirements

**Client:** Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

To debug the app and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the app without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.
