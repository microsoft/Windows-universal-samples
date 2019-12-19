---
page_type: sample
languages:
- csharp
products:
- windows
- windows-uwp
urlFragment: AppWindow
extendedZipContent:
- path: SharedContent
  target: SharedContent
- path: LICENSE
  target: LICENSE
description: "This sample shows you how to work with secondary windows in your app."
---

<!---
  category: ControlsLayoutAndText
--->

# AppWindow sample

This sample shows you how to work with secondary windows in your app.

AppWindows are secondary windows that run on the same thread as the CoreWindow, and they share the same core dispatcher.

Specifically, this sample shows how to:

- **Create a secondary window:** Creates an AppWindow and switches to it.
- **Position a secondary window onto another display:** AppWindows can be positioned, including the ability to position onto a specific display. This sample will show how to find the display your app is on, and other displays currently active in the system.
- **Setting the size of a secondary window:** Sets the size and position of an AppWindow before showing it to the user.
- **Position a secondary window relative to another window in the app or a display:** You can position the AppWindow relative to the upper left corner of a display, relative to the app's main view, or relative to another window in the app (not demonstrated).
- **Participating in CompactOverlay with a secondary window using AppWindow:** AppWindows can be placed into view modes like regular application views. This sample shows how to place an AppWindow into CompactOverlay mode.

**Note** The Windows universal samples require Visual Studio to build and Windows 10 to execute.

To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio and the tools for developing Windows apps, go to [Visual Studio](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples

[Multiple app views using ApplicationView](../Sample/MultipleViews)

### Reference

[AppWindow](https://docs.microsoft.com/en-us/uwp/api/windows.ui.windowmanagement)

[ApplicationView and ApplicationViewSwitcher](https://docs.microsoft.com/en-us/uwp/api/windows.ui.viewmanagement)

[CoreWindow](https://docs.microsoft.com/en-us/uwp/api/Windows.UI.Core.CoreWindow)

## System requirements

**Client:** Windows 10 

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10 

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
