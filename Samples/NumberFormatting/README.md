<!--
  category: GlobalizationAndLocalization
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620578
-->

# Number formatting and parsing sample

Shows how to use the [DecimalFormatter](http://msdn.microsoft.com/library/windows/apps/br226068), 
[CurrencyFormatter](http://msdn.microsoft.com/library/windows/apps/br206883), [PercentFormatter](http://msdn.microsoft.com/library/windows/apps/br226101) and 
[PermilleFormatter](http://msdn.microsoft.com/library/windows/apps/br226119) classes in the 
[Windows.Globalization.NumberFormatting](http://msdn.microsoft.com/library/windows/apps/br226136) namespace to display and parse numbers, currencies, and percent values.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

The sample also shows how to:

-   Round and pad numbers using the many rounding algorithms (enumerated in [RoundingAlgorithm](http://msdn.microsoft.com/library/windows/apps/dn278791)) supported by the 
[IncrementNumberRounder](http://msdn.microsoft.com/library/windows/apps/dn278487) and [SignificantDigitsNumberRounder](http://msdn.microsoft.com/library/windows/apps/dn278794) 
classes.
-   Use the [NumeralSystemTransator](http://msdn.microsoft.com/library/windows/apps/dn278744) class to convert strings containing Latin numbers to an 
appropriate numeral system that can be rendered in an app which does not perform any digit substitution.
-   Use language names with Unicode extensions to directly set properties of number formatters.

The [Windows.Globalization.NumberFormatting](http://msdn.microsoft.com/library/windows/apps/br226136) namespace provides number formatting and parsing APIs 
that generate strings for display that respect either the current user's preferences, or a caller-specified language(s) and region. There are individual methods 
for formatting or parsing numbers in the form of decimals, currencies, percentages, and units per thousand (permillages).

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples

[Calendar sample](../Calendar)  
[DateTimeFormatting sample](../DatetimeFormatting)  
[GlobalizationPreferences sample](../GlobalizationPreferences)  

### Reference

[Windows.Globalization.NumberFormatting.CurrencyFormatter](http://msdn.microsoft.com/library/windows/apps/br206883)  
[Windows.Globalization.NumberFormatting.DecimalFormatter](http://msdn.microsoft.com/library/windows/apps/br226068)  
[Windows.Globalization.NumberFormatting.IncrementNumberRounder](http://msdn.microsoft.com/library/windows/apps/dn278487)  
[Windows.Globalization.NumberFormatting.NumeralSystemTransator](http://msdn.microsoft.com/library/windows/apps/dn278744)  
[Windows.Globalization.NumberFormatting.PercentFormatter](http://msdn.microsoft.com/library/windows/apps/br226101)  
[Windows.Globalization.NumberFormatting.PermilleFormatter](http://msdn.microsoft.com/library/windows/apps/br226119)  
[Windows.Globalization.NumberFormatting.RoundingAlgorithm](http://msdn.microsoft.com/library/windows/apps/dn278791)  
[Windows.Globalization.NumberFormatting.SignificantDigitsNumberRounder](http://msdn.microsoft.com/library/windows/apps/dn278794)  

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
