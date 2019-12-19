---
page_type: sample
languages:
- csharp
products:
- windows
- windows-uwp
urlFragment: MixedRealityModel
extendedZipContent:
- path: SharedContent
  target: SharedContent
- path: LICENSE
  target: LICENSE
description: "Shows how you to place 3D content using Secondary Tiles by defining a mixed reality model at creation time."
---

<!---
  category: Holographic
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=865929
-->

# Mixed Reality Model sample

Shows how you to place 3D content using Secondary Tiles by defining a mixed reality model at creation time.

Specifically, this sample shows how to:

- **Create a 3D app launcher:** Define a 3D launcher asset in the app manifest to override the default 2D launcher for your app.
- **Create a 3D Secondary Tile:** Place 3D models from your app into the Windows Mixed Reality home.

**Note** The Windows universal samples require Visual Studio to build and Windows 10 to execute.

To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio and the tools for developing Windows apps, go to [Visual Studio](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples

[SecondaryTiles](../SecondaryTiles)

### Reference

[TileMixedRealityModel](https://docs.microsoft.com/uwp/api/windows.ui.startscreen.tilemixedrealitymodel)

[SecondaryTile](https://docs.microsoft.com/uwp/api/windows.ui.startscreen.secondarytile)

### Conceptual

[Implementing 3D app launchers](https://developer.microsoft.com/windows/mixed-reality/implementing_3d_app_launchers)

[Implementing 3D deep links for your app in the Windows Mixed Reality home](https://developer.microsoft.com/windows/mixed-reality/implementing_3d_deep_links_for_your_app_in_the_windows_mixed_reality_home)

[3D app launcher design guidance](https://developer.microsoft.com/windows/mixed-reality/3d_app_launcher_design_guidance)

[Creating 3D models for use in the Windows Mixed Reality home](https://developer.microsoft.com/windows/mixed-reality/creating_3d_models_for_use_in_the_windows_mixed_reality_home)


## System requirements

**Client:** Windows 10 Fall Creators Update 

**Server:** Not supported

**Phone:** Not supported 

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
