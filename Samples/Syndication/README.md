<!---
  category: NetworkingAndWebServices 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620610
--->

# Syndication sample

Shows how to retrieve feeds from a web service using classes in the [Windows.Web.Syndication](http://msdn.microsoft.com/library/windows/apps/br243632) 
namespace in your Universal Windows Platform (UWP) app. This sample is provided in the JavaScript, C\#, and C++ programming languages.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample demonstrates the following features:

-   Use the [SyndicationClient](http://msdn.microsoft.com/library/windows/apps/br243456) class and class members to retrieve a web feed.
-   Use the [SyndicationItem](http://msdn.microsoft.com/library/windows/apps/br243533) class and class members to display items in a web feed.

**Note**  Use of this sample requires Internet or intranet access in order to retrieve feeds.

## Network capabilities

This sample requires that network capabilities be set in the *Package.appxmanifest* file to allow the app to access the network at runtime. These capabilities can be set in the app manifest using Microsoft Visual Studio.

To build the sample, set the following network capabilities:

-   **Internet (Client & Server)**: This sample has complete access to the network for both client operations (outbound-initiated access) and server operations (inbound-initiated access). This allows the app to download various types of content from an HTTP server and upload content to an HTTP server located on the Internet or on a local intranet. This is represented by the **Capability name = "internetClientServer"** tag in the app manifest.

    **Private Networks (Client & Server)**: The sample has inbound and outbound network access on a home or work network (a local intranet). This allows the app to download various types of content from an HTTP server and upload content to an HTTP server located on a local intranet. The **Private Networks (Client & Server)** capability is represented by the **Capability name = "privateNetworkClientServer"** tag in the app manifest.

For more information on network capabilities, see [How to set network capabilities](http://msdn.microsoft.com/library/windows/apps/hh770532).

## Related topics

### Other - C\#/VB/C++ and XAML

[RSS/Atom feeds](https://msdn.microsoft.com/library/windows/apps/mt429379)  
[Adding support for networking (XAML)](http://msdn.microsoft.com/library/windows/apps/hh452751)  
[Networking basics](https://msdn.microsoft.com/library/windows/apps/mt280233)  

### Reference

[SyndicationClient](http://msdn.microsoft.com/library/windows/apps/br243456)  
[SyndicationItem](http://msdn.microsoft.com/library/windows/apps/br243533)  
[Windows.Web.AtomPub](http://msdn.microsoft.com/library/windows/apps/br210609)  
[Windows.Web.Syndication](http://msdn.microsoft.com/library/windows/apps/br243632)  

### Samples

[RSS reader sample](https://github.com/Microsoft/Windows-appsample-rssreader)  
[AtomPub sample](http://go.microsoft.com/fwlink/p/?linkid=245061)  

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:**  Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select **Build** \> **Deploy Solution**. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.
