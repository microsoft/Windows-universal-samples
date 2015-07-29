<!---
  category: NetworkingAndWebServices 
--->
# Syndication sample

This sample shows you how to retrieve feeds from a web service using classes in the [**Windows.Web.Syndication**](http://msdn.microsoft.com/library/windows/apps/br243632) namespace in your Universal Windows Platform (UWP) app. This sample is provided in the JavaScript, C\#, and C++ programming languages.

**Note**  This sample was created using one of the universal app templates available in Visual Studio. It shows how its solution is structured so it can run on Windows 10. For more info about how to build apps that target Windows and Windows Phone with Visual Studio, see [Build apps that target Windows 10 by using Visual Studio](http://msdn.microsoft.com/library/windows/apps/dn609832).

This sample demonstrates the following features:

-   Use the [**SyndicationClient**](http://msdn.microsoft.com/library/windows/apps/br243456) class and class members to retrieve a web feed.
-   Use the [**SyndicationItem**](http://msdn.microsoft.com/library/windows/apps/br243533) class and class members to display items in a web feed.

**Note**  Use of this sample requires Internet or intranet access in order to retrieve feeds.

## Network capabilities

This sample requires that network capabilities be set in the *Package.appxmanifest* file to allow the app to access the network at runtime. These capabilities can be set in the app manifest using Microsoft Visual Studio.

To build the sample, set the following network capabilities:

-   **Internet (Client & Server)**: This sample has complete access to the network for both client operations (outbound-initiated access) and server operations (inbound-initiated access). This allows the app to download various types of content from an HTTP server and upload content to an HTTP server located on the Internet or on a local intranet. This is represented by the **Capability name = "internetClientServer"** tag in the app manifest.

    **Private Networks (Client & Server)**: The sample has inbound and outbound network access on a home or work network (a local intranet). This allows the app to download various types of content from an HTTP server and upload content to an HTTP server located on a local intranet. The **Private Networks (Client & Server)** capability is represented by the **Capability name = "privateNetworkClientServer"** tag in the app manifest.

For more information on network capabilities, see [How to set network capabilities](http://msdn.microsoft.com/library/windows/apps/hh770532).

## Related topics

### Other - C\#/VB/C++ and XAML

[Accessing and managing syndicated content (XAML)](http://msdn.microsoft.com/library/windows/apps/hh452981)

[Adding support for networking (XAML)](http://msdn.microsoft.com/library/windows/apps/hh452751)

[How to set network capabilities (XAML)](http://msdn.microsoft.com/library/windows/apps/jj835817)

[How to access a web feed (XAML)](http://msdn.microsoft.com/library/windows/apps/hh452994)

### Other - JavaScript and HTML

[Accessing and managing syndicated content (HTML)](http://msdn.microsoft.com/library/windows/apps/hh452973)

[Adding support for networking (HTML)](http://msdn.microsoft.com/library/windows/apps/hh452752)

[How to set network capabilities (HTML)](http://msdn.microsoft.com/library/windows/apps/hh770532)

[How to access a web feed (HTML)](http://msdn.microsoft.com/library/windows/apps/hh700374)

### Reference

[**SyndicationClient**](http://msdn.microsoft.com/library/windows/apps/br243456)

[**SyndicationItem**](http://msdn.microsoft.com/library/windows/apps/br243533)

[**Windows.Web.AtomPub**](http://msdn.microsoft.com/library/windows/apps/br210609)

[**Windows.Web.Syndication**](http://msdn.microsoft.com/library/windows/apps/br243632)

### Samples

[AtomPub sample](http://go.microsoft.com/fwlink/p/?linkid=245061)

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Insider Preview

**Phone:**  Windows 10

## Build the sample

1. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2. Go to the subdirectory for the desired language.
3. Double-click the Visual Studio 2015 Solution (.sln) file. 
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**. 

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select **Build** \> **Deploy Solution**. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.
