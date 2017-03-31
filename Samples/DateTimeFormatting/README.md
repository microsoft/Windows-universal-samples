<!--
  category: GlobalizationAndLocalization
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=624044
--->

# Date and time formatting sample

Shows how to use the [DateTimeFormatter](http://msdn.microsoft.com/library/windows/apps/br206828)
class in the [Windows.Globalization.DateTimeFormatting](http://msdn.microsoft.com/library/windows/apps/br206859)
namespace to display dates and times according to the user's preferences.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

The **DateTimeFormatter**
class provides a globally-aware method for formatting a date or time into a string for display to a user.
It can either use the default preferences of the current user,
or the caller can override these to specify other languages, geographic region,
and clock and calendar systems.
The caller can request a format using the well-known constants
(shorttime, longtime, shortdate or longdate) or define the specific elements required.

The sample also uses the [Windows.Globalization.CalendarIdentifiers](http://msdn.microsoft.com/library/windows/apps/br229460)
and [Windows.Globalization.ClockIdentifiers](http://msdn.microsoft.com/library/windows/apps/br229462) classes.

This sample contains scenarios that demonstrate:

-   How to format the current date and time using the Long and Short formats.
-   How to format the current date and time using custom formats that are specified using a template string or a parameterized template.
-   How to format dates and times by overriding the user's default global context. This is used when an app presents dates or times that reflect different settings from the user's current defaults.
-   How to format dates and times by using Unicode extensions in specified languages, overriding the user's default global context if applicable.
-   How to convert and format the current date and time using the time zone support available in the [Format](http://msdn.microsoft.com/library/windows/apps/dn264145) method.

## Related topics

### Samples

[Calendar sample](../Calendar)  
[GlobalizationPreferences sample](../GlobalizationPreferences)  
[NumberFormatting sample](../NumberFormatting)  

### Reference

[Windows.Globalization.CalendarIdentifiers](http://msdn.microsoft.com/library/windows/apps/br229460)  
[Windows.Globalization.ClockIdentifiers](http://msdn.microsoft.com/library/windows/apps/br229462)  
[Windows.Globalization.DateTimeFormatting.DateTimeFormatter](http://msdn.microsoft.com/library/windows/apps/br206828)  

## System requirements

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

### Deploying the sample

- Select Build > Deploy Solution.

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or select Debug > Start Without Debugging. 
