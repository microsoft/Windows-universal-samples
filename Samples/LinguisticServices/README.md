<!---
  category: GlobalizationAndLocalization
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620562
--->

# Linguistic services sample

Shows how to use [Extended Linguistic Services (ELS)](http://msdn.microsoft.com/library/windows/apps/dd317839) in a Windows Store app.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

The sample implements scenarios that demonstrate the use of the three available services. The scenarios demonstrate how to request the desired service using the [MappingGetServices](http://msdn.microsoft.com/library/windows/apps/dd319060) function, and how to prepare parameters to be passed to the [MappingRecognizeText](http://msdn.microsoft.com/library/windows/apps/dd319063) function using that service.

The scenarios demonstrate the use of the these services:

-   [Languaged detection](http://msdn.microsoft.com/library/windows/apps/dd319066)

    Enter a selection of text for which you want to detect the language(s). This returns the names of the languages recognized, sorted by confidence.

-   [Script detection](http://msdn.microsoft.com/library/windows/apps/dd319067)

    Enter a selection of text in which you want to detect the scripts. This returns each range in the input text for which a particular script is recognized, with the standard Unicode name for the script.

-   [Transliteration services](http://msdn.microsoft.com/library/windows/apps/dd374080)

    Enter a selection of text in Cyrillic. This returns the input text transliterated to Latin.

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Not supported

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
