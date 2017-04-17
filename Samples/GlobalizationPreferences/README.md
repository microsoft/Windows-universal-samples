<!--
  category: GlobalizationAndLocalization
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=624045
-->

# Globalization preferences sample

Shows how to use the [Windows.System.UserProfile.GlobalizationPreferences](http://msdn.microsoft.com/library/windows/apps/br241825)
class to obtain the user's globalization preferences. In addition, it shows how to use the
[GeographicRegion](http://msdn.microsoft.com/library/windows/apps/br206792) and
[Language](http://msdn.microsoft.com/library/windows/apps/br206804) classes in the
[Windows.Globalization](http://msdn.microsoft.com/library/windows/apps/br206813)
namespace in order to display language and geographic region characteristics..

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Several user preferences related to globalization settings are available in the
[Windows.System.UserProfile](http://msdn.microsoft.com/library/windows/apps/br241881)
namespace. The [GlobalizationPreferences](http://msdn.microsoft.com/library/windows/apps/br241825)
class provides specific methods for accessing the following user preferences:

- Language list ([Languages](http://msdn.microsoft.com/library/windows/apps/br241830)).
- Geographic region ([HomeGeographicRegion](http://msdn.microsoft.com/library/windows/apps/br241829)).
- Calendar system and settings ([Calendars](http://msdn.microsoft.com/library/windows/apps/br241826)).
- Clock ([Clocks](http://msdn.microsoft.com/library/windows/apps/br241827)).
- First day of the week ([WeekStartsOn](http://msdn.microsoft.com/library/windows/apps/br241831)).

The [GeographicRegion](http://msdn.microsoft.com/library/windows/apps/br206792)
class also enables the caller to obtain details about a given geographic region or language.

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.

To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples

[Calendar sample](../Calendar)  
[DateTimeFormatting sample](../DateTimeFormatting)  
[NumberFormatting sample](../NumberFormatting)  

### Reference

[Windows.System.UserProfile namespace](http://msdn.microsoft.com/library/windows/apps/br241881)  
[Windows.System.UserProfile.GlobalizationPreferences class](http://msdn.microsoft.com/library/windows/apps/br241825)  
[Windows.Globalization namespace](http://msdn.microsoft.com/library/windows/apps/br206813)  
[Windows.Globalization.Calendar](http://msdn.microsoft.com/library/windows/apps/br206724)  
[Windows.Globalization.GeographicRegion class](http://msdn.microsoft.com/library/windows/apps/br206792)  
[Windows.Globalization.Language class](http://msdn.microsoft.com/library/windows/apps/br206804)  

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
