<!---
  category: ControlsLayoutAndText
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620579
--->

# OCR sample

Shows how to use Windows.Media.Ocr API. Optical character recognition (OCR) API allows for application developer to extract text in the specific language from an image.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample covers:

Scenario 1: Load image from a file and extract text in user specified language.

1. Determine whether any language is OCR supported on device.
2. Get list of all available OCR languages on device.
3. Create OCR recognizer for specific language.
4. Create OCR recognizer for the first OCR supported language from GlobalizationPreferences.Languages list.
5. Load image from a file and extract text.
6. Overlay word bounding boxes over displayed image.
7. Differentiate vertical and horizontal text lines.

Scenario 2: Capture image from camera and extract text.

1. Check if specific language is available on device.
2. Capture image from camera and extract text.
3. Overlay word bounding boxes and recognized text over displayed image.

## Related topics

**Reference**

[Windows.Media.Ocr namespace](https://msdn.microsoft.com/library/windows/apps/windows.media.ocr.aspx)  
[Windows.Globalization.Language class](https://msdn.microsoft.com/library/windows/apps/windows.globalization.language.aspx)  
[GlobalizationPreferences.Languages property](https://msdn.microsoft.com/library/windows/apps/windows.system.userprofile.globalizationpreferences.languages.aspx)  
[Language matching] (https://msdn.microsoft.com/library/windows/apps/jj673578.aspx)  
[Windows.Graphics.Imaging.BitmapDecoder class](https://msdn.microsoft.com/library/windows/apps/windows.graphics.imaging.bitmapdecoder.aspx)  
[Windows.Media.Capture.MediaCapture namespace](https://msdn.microsoft.com/library/windows/apps/windows.media.capture.aspx)  

## System requirements

**Hardware:** Camera

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

**Deploying the sample:**

1.  Select **Build** \> **Deploy Solution**.

**Deploying and running the sample:**

1.  To debug the sample and then run it, press F5 or select **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or select **Debug** \> **Start Without Debugging**.
