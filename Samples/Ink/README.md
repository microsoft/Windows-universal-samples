<!---
  category: CustomUserInteractions Inking
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620554
--->

# Inking sample

Shows how to use ink functionality (such as, capturing, manipulating, and interpreting ink strokes) in Universal Windows apps using JavaScript.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample covers using the [Windows.UI.Input.Inking](http://msdn.microsoft.com/library/windows/apps/br208524) APIs to:

-   Render ink strokes
-   Save and load ink strokes
-   Copy and paste ink strokes
-   Select ink strokes
-   Delete ink strokes
-   Recognize handwriting from ink strokes
-   Search for a string within recognition results

**Note**  Read the following instructions before using the app.

-   Draw ink strokes with a tablet pen.
-   The toolbar contains color pickers, line selectors, and other functions.
-   In **Erase** mode, a stroke deletes all strokes that it touches. If the pen has an eraser, it erases strokes (switching back to the pen tip exits erase mode).
-   In **Select** mode, a stroke selects all strokes that it encompasses. Exit **Select** mode by choosing another function on the toolbar.

    **Note**   Ink color or width is applied to all selected strokes and highlight color or width is applied to all selected highlights.

-   In **Recognition** mode, all (or only selected) strokes are processed through handwriting recognition. After recognition has completed:
    -   View the primary text candidate(s) at the bottom of the app.
    -   Tap on a word to show all text candidates for that word.
    -   Use **Find** to search for a string within the recognition results.
-   Delete all strokes (or only selected strokes) by pressing the **Erase** button.
-   Additional commands include:
    -   Use **Copy** to copy selected strokes onto the clipboard.
    -   Use **Paste** to paste strokes from the clipboard.
    -   Use **Save** and **Load** to read and write Ink Serialized Format (ISF) files to the Libraries or Pictures folder or both.
-   All buttons and menu choices can be activated with mouse, pen, or touch.

To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

[Responding to pen and stylus interactions](http://msdn.microsoft.com/library/windows/apps/hh700425)  
[Responding to user interaction](http://msdn.microsoft.com/library/windows/apps/hh700412)  

**Samples**

[SimpleInk](/Samples/SimpleInk)  
[ComplexInk](/Samples/ComplexInk)  

**Reference**

[Windows.UI.Input.Inking](http://msdn.microsoft.com/library/windows/apps/br208524)  

## Operating system requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

To debug the app and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the app without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.
