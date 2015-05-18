# AutoSuggestBox migration sample

This sample shows how to migrate your Windows 8.1 app to Windows 10 by replacing your SearchBox with an AutoSuggestBox.  With the improvements in AutoSuggestBox, you will get the same end-user experience across all supported Windows 10 devices.

Specifically, this sample shows how to:

- **Add a glyph to your AutoSuggestBox:** With the new QueryIcon API, you can specify a glyph to put in the TextBox of your AutoSuggestBox without re-templating.
- **Add keyboarding support:** The QuerySubmitted event has been added to support all cases where an item could be selected, including Enter key, mouse click, or tap.

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10, go to [Windows 10](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples

[XAML UI Basics](https://github.com/Microsoft/Windows-universal-samples/tree/master/XAMLUIBasics)

### Reference


[AutoSuggestBox](http://msdn.microsoft.com/library/windows/apps/windows.ui.xaml.controls.autosuggestbox.aspx)


## System requirements

**Client:** Windows 10 Insider Preview

**Server:** Windows 10 Insider Preview

**Phone:**  Windows 10 Insider Preview

## Build the sample

1. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2. Go to the directory to which you unzipped the sample. Then go to the subdirectory containing the sample in the language you desire - either C++, C#, or JavaScript. Double-click the Visual Studio 2015 Solution (.sln) file. 
3. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**. 

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 
