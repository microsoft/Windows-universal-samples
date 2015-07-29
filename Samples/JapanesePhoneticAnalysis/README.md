<!--
  category: GlobalizationAndLocalization
-->

# Japanese phonetic analysis globalization sample

This sample demonstrates how to analyze Japanese texts and retrieves words or segments in the text by using the [**JapanesePhoneticAnalyzer**](http://msdn.microsoft.com/library/windows/apps/dn434076) class.

The sample demonstrates these tasks:

1.  **Analyze Japanese text**

    This scenario shows how to use the [**GetWords**](http://msdn.microsoft.com/library/windows/apps/dn434078) method to split Japanese text, one segment per line.
    The segments can be words or pronunciation units.

**Note** The Universal Windows app samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10, go to [Windows 10](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples

[Unicode](/Samples/Unicode)

### Reference

[JapanesePhoneticAnalyzer class](https://msdn.microsoft.com/en-us/library/windows/apps/windows.globalization.japanesephoneticanalyzer.aspx)

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Not supported

## Build the sample

1. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2. Go to the directory to which you unzipped the sample. Then go to the subdirectory containing the sample in the language you desire - either C++, C#, or JavaScript. Double-click the Visual Studio 2015 Solution (.sln) file. 
3. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**. 

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug > Start Debugging. To run the sample without debugging, press Ctrl+F5 or select Debug > Start Without Debugging. 

## How to use the sample

- Enter Japanese text into the edit control, or use the sample text provided.
- Select whether you wish to split the text based on words or based on units of pronunciation.
- Click the Analyze button to perform phonetic analysis.
