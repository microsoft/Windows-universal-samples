<!--
  category: GlobalizationAndLocalization
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=624043
-->

# Calendar sample

Shows how to use the [Calendar](http://msdn.microsoft.com/library/windows/apps/br206724) class in the
[Windows.Globalization](https://msdn.microsoft.com/library/windows/apps/windows.globalization.aspx)
namespace to manipulate and process dates based on a calendar system and the user's globalization preferences.
(If you are looking for a sample which demonstrates an appointment calendar, see the [Appointments](../Appointments) sample.)

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

The
[Calendar](http://msdn.microsoft.com/library/windows/apps/br206724)
class provides details of the user's default calendar or a caller-specified calendar system.
The class gives access to specific statistical details
as well as the ability to perform calendar-aware math operations.

The Calendar object can convert to and from a language-specific date type:

* C++: [Windows::Foundation::DateTime](https://msdn.microsoft.com/library/windows/apps/windows.foundation.datetime.aspx)
* C#: [DateTimeOffset](http://msdn.microsoft.com/library/windows/apps/system.datetimeoffset.aspx)
* JavaScript: [Date](https://msdn.microsoft.com/library/windows/apps/cd9w2te4.aspx)

The sample also uses the
[Windows.Globalization.CalendarIdentifiers](http://msdn.microsoft.com/library/windows/apps/br229460)
and
[Windows.Globalization.ClockIdentifiers](http://msdn.microsoft.com/library/windows/apps/br229462)
classes.

This sample contains scenarios that demonstrate:

* How to create a calendar for the user's default preferences or for specific overrides, and how to display calendar details. 

* How to determine statistics for the current calendar date and time, such as the number of days in this month and the number of months in this year. 

* How to enumerate through a calendar and perform calendar math, such as determining the number of hours in a day that spans the transition from Daylight Saving Time. 
This scenario also demonstrates converting from a language-specific date type to a Calendar.

* How to create a calendar using language names with supported Unicode extension tags, and how the extension tags are used by the calendar object. 

* How to support time zones in calendars, by changing several time zones within a calendar and showing the effect of the time zone change in the date and time properties of the calendar. 

* How to convert between calendars and a language-specific date type.

## Related topics

### Samples

[DateTimeFormatting sample](../DateTimeFormatting)  
[GlobalizationPreferences sample](../GlobalizationPreferences)  
[NumberFormatting sample](../NumberFormatting)  

### Reference

[Windows.Globalization.Calendar](http://msdn.microsoft.com/library/windows/apps/br206724)  
[Windows.Globalization.CalendarIdentifiers](http://msdn.microsoft.com/library/windows/apps/br229460)  
[Windows.Globalization.ClockIdentifiers](http://msdn.microsoft.com/library/windows/apps/br229462)  

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
