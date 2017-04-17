<!---
  category: GlobalizationAndLocalization
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620487
--->

# Application resources and localization sample

Shows how to use application resources to separate localizable content from app code.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

The sample uses the [Windows.ApplicationModel.Resources.Core](http://msdn.microsoft.com/library/windows/apps/br225039) and 
[Windows.Globalization](http://msdn.microsoft.com/library/windows/apps/br206813) namespaces, and [WinJS.Resources](http://msdn.microsoft.com/library/windows/apps/br229779).

We encourage you to separate your resources, such as strings and files, from your app code, so you can maintain and alter your resources independently. You can use [Application Resources](http://msdn.microsoft.com/library/windows/apps/br225039) APIs to tailor your apps to a variety of device, system, and user configurations. Resources enable apps to be localized into multiple languages, customized for various device DPIs and form factors, and tailored for specific types of users.

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

[Creating and retrieving resources in Windows apps](http://go.microsoft.com/fwlink/p/?linkid=251463)  
[ResourceLoader class](http://msdn.microsoft.com/library/windows/apps/br206014)  
[ResourceManager class](http://msdn.microsoft.com/library/windows/apps/br206078)  
[Windows.ApplicationModel.Resources.Core namespace](http://msdn.microsoft.com/library/windows/apps/br225039)  
[Windows.Globalization namespace](http://msdn.microsoft.com/library/windows/apps/br206813)  
[Windows 8 app samples](http://go.microsoft.com/fwlink/p/?LinkID=227694)  
[WinJS.Resources namespace](http://msdn.microsoft.com/library/windows/apps/br229779)  

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or select Debug > Start Without Debugging. 


To debug the app and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the app without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.

