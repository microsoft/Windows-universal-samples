<!---
  category: CustomUserInteractions
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=627612
--->

# Touch keyboard sample

This sample demonstrates how apps can influence whether the touch keyboard displays when the user taps on a control with the pen or touch.

* Standard XAML text controls (such as TextBox, RichTextBox, and PaswordBox) display the touch keyboard by default.
* Controls derived from standard XAML text controls display the touch keyboard by default.
* Other controls do not display the touch keyboard by default.
* On the PC, you can request that the touch keyboard display for a custom control
  by implementing the TextPattern provider interface ([**ITextProvider**](http://msdn.microsoft.com/library/windows/apps/br242627))
  and the ValuePattern provider interface ([**IValueProvider**](http://msdn.microsoft.com/library/windows/apps/br242663)).
  Not supported on Phone.
* You can request that the touch keyboard not display when focus is programmatically placed on a control.

**Note** In Windows 10,
the touch keyboard will not display automatically if a hardware keyboard is connected,
or the PC is in Desktop mode and "Automatically show the touch keyboard in windowed apps when there is no keyboard attached to your device" in Settings -> Devices -> Typing is disabled.

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10, go to [Windows 10](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10. (KeyboardEnabledTextBox not supported; see remarks above.)

## Build the sample

1. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2. Go to the directory to which you unzipped the sample. Then go to the subdirectory containing the sample in the language you desire - either C++, C#, or JavaScript. Double-click the Visual Studio 2015 Solution (.sln) file. 
3. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**. 

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or select Debug > Start Without Debugging. 


