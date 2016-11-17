<!---
  category: TilesToastAndNotifications
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620593
--->

# Secondary tiles sample

Shows how to pin and use a secondary tile, which is a tile that directly accesses a specific, non-default section or experience within an app, 
such as a saved game or a specific friend in a social networking app. Sections or experiences within an app that can be pinned to the Start screen 
as a secondary tile are chosen and exposed by the app, but the creation of the secondary tile is strictly up to the user.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample demonstrates the following scenarios:

- Pinning a secondary tile to the Start screen 
- Removing a secondary tile from the Start screen 
- Enumerating all secondary tiles owned by the calling app 
- Determining whether a particular tile is currently pinned to the Start screen 
- Processing arguments when the app is activated through a secondary tile 
- Sending a local tile notification and badge notification to the secondary tile 
- Using the app bar to pin and unpin tiles. (JavaScript and C# only) 
- Updating the secondary tile's default logo 
- Selecting from among alternative secondary tile visuals (Windows only) 
- Selecting from among alternative secondary tile visuals as an asynchronous operation (Windows only) 

**Note**  Some functionality in the sample requires that the tile can receive notifications. Tile notifications can be disabled by a user for a single app or for all apps, or by a system administrator by using group policy.

To obtain an insider copy of Windows 10, go to [Windows 10](http://insider.windows.com). 

**Note**  For Windows 10 app samples, go to  [Windows 10 Samples](https://github.com/Microsoft/Windows-universal-samples). The samples for Windows 10 can be built and run using Windows developer [tools](https://developer.windows.com).

## Related topics

[Guidelines and checklist for secondary tiles](http://msdn.microsoft.com/library/windows/apps/hh465398)  
[Quickstart: Pinning a secondary tile](http://msdn.microsoft.com/library/windows/apps/hh465443)  
[Quickstart: Sending notifications to a secondary tile](http://msdn.microsoft.com/library/windows/apps/hh761474)  
[SecondaryTile class](http://msdn.microsoft.com/library/windows/apps/br242183)  
[Secondary tiles overviews](http://msdn.microsoft.com/library/windows/apps/hh465372)  

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or select Debug > Start Without Debugging. 

##How to use the sample
In some of the scenarios, you need to switch to the Start screen to see the effect of the scenario on the secondary tile. Click the sample tile to return to the main sample page or the secondary tile (in this sample, the secondary tile simply says "Windows SDK") to be taken into an area of the sample that confirms that you've launched it from a secondary tile.

Note that the response to pinning a tile differs between Windows and Windows Phone. On Windows Phone, when you pin a secondary tile, you exit the app and are taken to the Start screen. In Windows, you must manually switch to the Start screen to see the secondary tile.
