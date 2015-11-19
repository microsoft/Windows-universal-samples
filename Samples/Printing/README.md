<!---
  category: ControlsLayoutAndText
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=619984
--->

# Printing sample

This sample demonstrates how apps can add support for printing on Windows. 

The scenarios demonstrated in this sample include:

- Adding support for printing using app UX
- Customizing the print experience by changing how the printer settings are shown to the user
- Using a custom print template to print a range of pages
- Printing alternate content that is not part of the current document

Changes from previous versions of the sample:
- Removed charms-based printing scenario
- Refactored BasePrintPage out of solution for increased clarity. Sample now uses PageToPrint class and PrintHelper class instead. 
- Added exception handling to ShowPrintUIAsync calls
- Updated app to Universal Windows Platform (UWP)

System requirements
-------------------

Client

Windows 10


Build the sample
----------------

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

Run the sample
--------------

To debug the app and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the app without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.
