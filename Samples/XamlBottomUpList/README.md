<!---
  category: ControlsLayoutAndText
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=761467
--->

# Bottom-up list (XAML) sample

Demonstrates a ListView that is tailored for scenarios
in which the last item is the most interesting.
This type of ListView is common for chat windows.
You might also find it useful for showing a trace log.

* Sets the ItemsStackPanel.ItemsUpdatingScrollMode property to KeepLastItemInView
to indicate that the last item should be used as the anchor when performing layout.
This affects how the scroll offset is persisted,
how it is maintained when items are added/removed,
and the animation that occurs when items are added/removed.
* Saves and restores the scroll position.
* Adds older items to the top of the list incrementally.
* Manually triggers the ISupportIncrementalLoading.LoadMoreAsync method
as the scroll position nears the top of the list.

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)


## System requirements

**Client:** Windows 10 build 14295

**Server:** Windows Server 2016 Technical Preview

**Phone:**  Windows 10 build 14295

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

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 
