<!---
  category: CustomUserInteractions Inking
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620602
--->

# Simple inking sample

Shows how to use ink functionality (such as capturing ink from user input and performing handwriting recognition on ink strokes) in Universal Windows apps using C#.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample covers using the Windows.UI.Input.Inking APIs to do the following:
-   Capture user input and render as ink strokes
-   Create an ink toolbar which is associated with an ink canvas
-   Add custom buttons to the ink toolbar
-   Create an ink toolbar with only specified buttons
-   Control multiple ink canvases with a single ink toolbar
-   Save and load ink strokes
-   Erase ink strokes
-   Create and select different types of pens
-   Change which input types (Mouse/Touch/Pen) are used for inking
-   Recognize handwriting from ink strokes
-   Select ink strokes
-   Create a ruler, show it, hide it, and reposition it
-   manipulate wet ink strokes
    - make strokes follow a shape
    - complete a stroke prematurely if conditions are met

## Related topics

**Samples**

[ComplexInk](/Samples/ComplexInk)  
[Family Notes sample](https://github.com/Microsoft/Windows-appsample-familynotes)  
[Coloring Book app sample](https://github.com/Microsoft/Windows-appsample-coloringbook)  

**Reference**

[Windows.UI.Input.Inking](http://msdn.microsoft.com/library/windows/apps/br208524)  

## Operating system requirements

**Client:** Windows 10 build 14295

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10 build 14295

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

