<!---
  category: CustomUserInteractions Touch
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=690716
--->

# Touch keyboard text input sample

Shows how to enable optimized views on the touch keyboard by using input scopes and input types with controls in the 
[WinJS.UI](http://msdn.microsoft.com/library/windows/apps/br229782) namespace, 
and with the [TextBox](http://msdn.microsoft.com/library/windows/apps/br209683) and [RichEdit](http://msdn.microsoft.com/library/windows/apps/br227548) 
Extensible Application Markup Language (XAML) controls. 
Also, this sample demonstrates spell checking by using the [spellcheck](http://msdn.microsoft.com/library/windows/apps/hh441107) and 
[IsSpellCheckEnabled](http://msdn.microsoft.com/library/windows/apps/br209688) properties, 
and it shows text prediction by using the [IsTextPredictionEnabled](http://msdn.microsoft.com/library/windows/apps/br209690) property.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

The input types shown in this sample are:

-   URL
-   Email
-   Password
-   Number
-   Search
-   Telephone

**Note** Because of a known issue, the first Scenario in the JS sample does not work properly in Phone - the text controls in the JS sample do not obey the "spellcheck" attribute.

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10, go to [Windows 10](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

Related topics
--------------

**Conceptual**

[Getting started with apps](http://msdn.microsoft.com/library/windows/apps/)  
[Guidelines and checklist for login controls](http://msdn.microsoft.com/library/windows/apps/hh965453)  
[Guidelines and checklist for touch keyboard](http://msdn.microsoft.com/library/windows/apps/hh972345)  
[Responding to user interaction](http://msdn.microsoft.com/library/windows/apps/hh700412)  

**Reference**

[IsSpellCheckEnabled](http://msdn.microsoft.com/library/windows/apps/br209688)  
[IsTextPredictionEnabled](http://msdn.microsoft.com/library/windows/apps/br209690)  
[spellcheck](http://msdn.microsoft.com/library/windows/apps/hh441107)  
[RichEdit](http://msdn.microsoft.com/library/windows/apps/br227548)  
[TextBox](http://msdn.microsoft.com/library/windows/apps/br209683)  
[Windows.UI.Core](http://msdn.microsoft.com/library/windows/apps/br208383)  

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

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

