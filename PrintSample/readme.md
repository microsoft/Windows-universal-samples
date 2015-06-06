Print sample
============

This sample demonstrates how apps can add support for printing on Windows. 

The scenarios demonstrated in this sample include:

-   Adding support for printing using app UX
-   Customizing the print experience by changing how the printer settings are shown to the user
-   Using a custom print template to print a range of pages
-   Printing alternate content that is not part of the current document

Changes from previous versions of the sample:
-	Removed charms-based printing scenario
- 	Refactored BasePrintPage out of solution for increased clarity. Sample now uses PageToPrint class and PrintHelper class instead. 
-	Added exception handling to ShowPrintUIAsync calls
-	Updated app to Universal Windows Platform (UWP)

System requirements
-------------------

Client

Windows 10 Insider Preview


Build the sample
----------------

1.  Start Visual Studio 2015 and select **File \> Open \> Project/Solution**.
2.  Go to the directory in which you unzipped the sample. Go to the directory named for the sample, and double-click the Microsoft Visual Studio Solution (.sln) file.
3.  Press F7 or use **Build \> Build Solution** to build the sample.

Run the sample
--------------

To debug the app and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the app without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.
