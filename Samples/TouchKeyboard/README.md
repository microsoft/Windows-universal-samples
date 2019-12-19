---
page_type: sample
languages:
- csharp
- cpp
- cppcx
products:
- windows
- windows-uwp
urlFragment: TouchKeyboard
extendedZipContent:
- path: SharedContent
  target: SharedContent
- path: LICENSE
  target: LICENSE
description: "Shows both the default display behavior of the touch keyboard and how that behavior can be customized in a UWP app."
---

<!---
  category: CustomUserInteractions Touch
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=627612
--->

# Touch keyboard sample

Shows both the default display behavior of the touch keyboard and how that behavior can be customized in a UWP app.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> You can download this sample as a standalone ZIP file
> [from docs.microsoft.com](https://docs.microsoft.com/samples/microsoft/windows-universal-samples/touchkeyboard/),
> or you can download the entire collection as a single
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample shows how:

* XAML text controls (such as TextBox, RichTextBox, and PaswordBox) display the touch keyboard by default.
* Controls derived from XAML text controls display the touch keyboard by default.
* Apps can subscribe to events indicating when the touch keyboard is shown and hidden.
* Apps can request that the touch keyboard be shown or hidden.

**Note** The touch keyboard does not automatically display if a hardware keyboard is connected,
or the device is in Desktop mode and "Show the touch keyboard when not in tablet mode and there's no keyboard attached" is set to "Off" in **Settings -> Devices -> Typing**.

**Note** The Windows universal samples require Visual Studio to build and Windows 10 to execute.

**Note** <a href="../CustomEditControl">Custom text edit control sample</a> shows how to manage the visibility of the touch keyboard programatically.
 
To obtain information about Windows 10, go to [Windows 10](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio and the tools for developing Windows apps, go to [Visual Studio](http://go.microsoft.com/fwlink/?LinkID=532422)

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10. (KeyboardEnabledTextBox not supported; see remarks above.)

## Build the sample

1. Start Microsoft Visual Studio and select **File** \> **Open** \> **Project/Solution**.
2. Go to the directory to which you unzipped the sample. Then go to the subdirectory containing the sample in the language you desire - either C++, C#, or JavaScript. Double-click the Visual Studio Solution (.sln) file. 
3. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**. 

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or select Debug > Start Without Debugging. 


