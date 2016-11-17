<!---
  category: AppSettings 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620581
--->

# App package information sample

Shows how to get package info by using the Windows Runtime packaging API ([Windows.ApplicationModel.Package](http://msdn.microsoft.com/library/windows/apps/br224667) 
and [Windows.ApplicationModel.PackageId](http://msdn.microsoft.com/library/windows/apps/br224668)).

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Users acquire your Windows Runtime app as an app package. The operating system uses the info in an app package to install the app on a per-user basis, and ensure that all traces of the app are gone from the device after all users who installed the app uninstall it. Each package consists of the files that constitute the app, along with a package manifest file that describes the app to the operating system.

Each package is defined by a globally unique identifier known as the package identity. Each package is described through package properties such as the display name, description, and logo.

The sample covers these key tasks:

-   Getting the package identity using [Package.Id](http://msdn.microsoft.com/library/windows/apps/br224680)
-   Getting the package directory using [Package.InstalledLocation](http://msdn.microsoft.com/library/windows/apps/br224681)
-   Getting package dependencies using [Package.Dependencies](http://msdn.microsoft.com/library/windows/apps/br224679)

The sample covers these new tasks for Windows 10:

-   Getting the package description using [Package.Description](http://msdn.microsoft.com/library/windows/apps/dn175742)
-   Getting the package display name using [Package.DisplayName](http://msdn.microsoft.com/library/windows/apps/dn175743)
-   Determining whether the package is a bundle package using [Package.IsBundle](http://msdn.microsoft.com/library/windows/apps/dn175744)
-   Determining whether the package is installed in development mode using [Package.IsDevelopmentMode](http://msdn.microsoft.com/library/windows/apps/dn175745)
-   Determining whether the package is a resource package using [Package.IsResourcePackage](http://msdn.microsoft.com/library/windows/apps/dn175746)
-   Getting package logo using [Package.Logo](http://msdn.microsoft.com/library/windows/apps/dn175747)
-   Getting publisher display name of the package using [Package.PublisherDisplayName](http://msdn.microsoft.com/library/windows/apps/dn175748)

To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

**Conceptual**

[Packaging apps](https://msdn.microsoft.com/library/windows/apps/mt270969)  

**Reference**

[Windows.ApplicationModel.Package](http://msdn.microsoft.com/library/windows/apps/br224667)  
[Windows.ApplicationModel.PackageId](http://msdn.microsoft.com/library/windows/apps/br224668)  

### Reference

To learn more about sharing and the [Windows.ApplicationModel.DataTransfer](http://msdn.microsoft.com/library/windows/apps/br205967) namespace, 
we recommend you take a look at the [Sharing and exchanging data](http://msdn.microsoft.com/library/windows/apps/hh464923) section of our documentation, 
which describes how sharing works and contains several how-to topics that cover how to share [text](http://msdn.microsoft.com/library/windows/apps/hh758313), 
an [image](http://msdn.microsoft.com/library/windows/apps/hh758305), files, and other formats. Our 
[Guidelines for sharing content](http://msdn.microsoft.com/library/windows/apps/hh465251) can also help you create a great user experience with the share feature.

For more info about the concepts and APIs demonstrated in this sample, see these topics:

-   [Sharing content target app sample](http://go.microsoft.com/fwlink/p/?linkid=231519)
-   [Sharing and exchanging data](http://msdn.microsoft.com/library/windows/apps/hh464923)
-   [How to share files (HTML)](http://msdn.microsoft.com/library/windows/apps/hh758308)
-   [How to share files (XAML)](http://msdn.microsoft.com/library/windows/apps/hh871371)
-   [How to share HTML (HTML)](http://msdn.microsoft.com/library/windows/apps/hh758310)
-   [How to share HTML (XAML)](http://msdn.microsoft.com/library/windows/apps/hh973055)
-   [How to share text (HTML)](http://msdn.microsoft.com/library/windows/apps/hh758313)
-   [How to share text (XAML)](http://msdn.microsoft.com/library/windows/apps/hh871372)
-   [Quickstart: Sharing content (HTML)](http://msdn.microsoft.com/library/windows/apps/hh465261)
-   [Share data](https://msdn.microsoft.com/library/windows/apps/mt243293)
-   [DataPackageView](http://msdn.microsoft.com/library/windows/apps/hh738408)
-   [ShareOperation](http://msdn.microsoft.com/library/windows/apps/br205977)
-   [Windows.ApplicationModel.DataTransfer](http://msdn.microsoft.com/library/windows/apps/br205967)
-   [Windows.ApplicationModel.DataTransfer.Share](http://msdn.microsoft.com/library/windows/apps/br205989)

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

