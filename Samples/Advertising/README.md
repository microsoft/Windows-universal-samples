<!---
  category: NetworkingAndWebServices
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=722924
--->

# Advertising sample

Shows how to use the Microsoft Advertising SDK (Microsoft.Advertising) to show display ads and interstitial ads.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample shows how to:

- Create an ad control to show display ads using only XAML / HTML markup
- Create an ad control to show display ads programatically
- Create an ad control to show interstitial video ads programatically

## Prerequisites

This sample requires the [Microsoft Advertising SDK Libraries for XAML or JavaScript](http://go.microsoft.com/fwlink/?LinkID=619694).
[Download it here](http://go.microsoft.com/fwlink/p/?LinkId=518026).
It also requires the [internet client capability](https://msdn.microsoft.com/library/windows/apps/mt270968#general-use_capabilities) to be added to the manifest.

When you incorporate this sample into your own app,
obtain your own ApplicationId and AdUnitId from the Dev Center.
See [Monetize with Ads](https://msdn.microsoft.com/library/windows/apps/mt170658.aspx) on MSDN
for more details.

## Related topics

### Samples

[Store in-app purchase](/Samples/Store)  

## Reference

[Microsoft Advertising Client SDK API Reference](https://msdn.microsoft.com/library/windows/apps/mt691884.aspx)  

### See also

[Getting Started](https://msdn.microsoft.com/windows/uwp/monetize/get-started-with-microsoft-advertising-libraries)  
[Developer Walkthroughs](https://msdn.microsoft.com/windows/uwp/monetize/developer-walkthroughs)  
[Interstitial Ads](https://msdn.microsoft.com/windows/uwp/monetize/interstitial-ads)  

## System requirements

**Client:** Windows 10

**Phone:** Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio�2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 
