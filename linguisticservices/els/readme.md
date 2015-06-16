# Extended Linguistic Services (ELS) sample

This sample demonstrates the use of [Extended Linguistic Services (ELS)](http://msdn.microsoft.com/library/windows/apps/dd317839) in a Windows Store app.

The sample implements scenarios that demonstrate the use of the three available ELS services. The scenarios demonstrate how to request the desired service using the [**MappingGetServices**](http://msdn.microsoft.com/library/windows/apps/dd319060) function, and how to prepare parameters to be passed to the [**MappingRecognizeText**](http://msdn.microsoft.com/library/windows/apps/dd319063) function using that service.

The scenarios demonstrate the use of the these ELS services:

-   [Microsoft Language Detection](http://msdn.microsoft.com/library/windows/apps/dd319066)

    Enter a selection of text for which you want to detect the language(s). This returns the names of the languages recognized, sorted by confidence.

-   [Microsoft Script Detection](http://msdn.microsoft.com/library/windows/apps/dd319067)

    Enter a selection of text in which you want to detect the scripts. This returns each range in the input text for which a particular script is recognized, with the standard Unicode name for the script.

-   [ELS Transliteration Services](http://msdn.microsoft.com/library/windows/apps/dd374080)

    Enter a selection of text in Cyrillic. This returns the input text transliterated to Latin.

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10, go to [Windows 10](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## System requirements

**Client:** Windows 10 Technical Preview

**Server:** Windows 10 Technical Preview

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
