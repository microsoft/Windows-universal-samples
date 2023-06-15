---
page_type: sample
languages:
- csharp
- cpp
- cppwinrt
products:
- windows
- windows-uwp
urlFragment: NetworkingConnectivity
extendedZipContent:
- path: SharedContent
  target: SharedContent
- path: LICENSE
  target: LICENSE
description: "Shows how to query network connectivity and respond to network connectivity changes."
---

# NetworkingConnectivity sample

Demonstrates how to use the NetworkInformation and related classes
to determine the network connectivity status,
and shows how to use this information to determine
when to attempt to connect to the Internet.

Apps can use the NetworkInformation and related Windows Runtime classes
to check the network connectivity status before attempting to connect to the Internet.
These class simplify the complex task of determining connectivity for various network configurations.
These checks are not strictly required because higher-level APIs (such as HttpClient)
report insufficient network connectivity through failures/results at the point of connection.

This sample also demonstrates how to register for network connectivity change events.
Apps can subscribe to the events instead of building their
own logic to track network connectivity changes.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> You can download this sample as a standalone ZIP file
> [from docs.microsoft.com](https://docs.microsoft.com/samples/microsoft/windows-universal-samples/networkingconnectivity/),
> or you can download the entire collection as a single
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/main.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 


### Declaring the internetClient capability

This sample requires that internetClient capability be set in the *Package.appxmanifest* file to allow the app to access the Internet connection at runtime.
The capability can be set in the app manifest using Microsoft Visual Studio.

## System requirements

* Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio Solution (.sln) file.
4. Press Ctrl+Shift+B or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Debugging and running the sample

- To debug the sample and then run it, press F5 or select Debug > Start Debugging. To run the sample without debugging, press Ctrl+F5 or select Debug > Start Without Debugging. 
