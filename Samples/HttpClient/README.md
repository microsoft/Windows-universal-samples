<!---
  category: NetworkingAndWebServices 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620551
--->

# HttpClient sample

Shows how to upload and download various types of content with an HTTP server using the 
[HttpClient](http://msdn.microsoft.com/library/windows/apps/dn298639) and related classes in 
[Windows.Web.Http](http://msdn.microsoft.com/library/windows/apps/dn279692) namespace.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample shows the use of asynchronous GET and POST requests using [HttpClient](http://msdn.microsoft.com/library/windows/apps/dn298639). 
The [HttpClient](http://msdn.microsoft.com/library/windows/apps/dn298639) class is used to send and receive basic requests over HTTP. 
It is the main class for sending HTTP requests and receiving HTTP responses from a resource identified by a URI. This class can be used to send 
a GET, PUT, POST, DELETE, and other requests to a web service. Each of these requests is sent as an asynchronous operation. The **HttpClient** 
class can be used in scenarios that use text as well as scenarios that use arbitrary streams of data. Filters 
(see [Windows.Web.Http.Filters](http://msdn.microsoft.com/library/windows/apps/dn298623) namespace) provide a way to modify the 
request/response flow of HTTP and handle common HTTP web service issues. Filters can be chained together in a sequence to address more complex web service issues. 

The sample covers the following scenarios:

Scenario 1: Use HTTP GET command to download HTML text from a server, using various caching options

Scenario 2: Use HTTP GET command to download a stream from a server

Scenario 3: Use HTTP GET command to download a list of items in XML format from a server

Scenario 4: Use HTTP POST command to upload text to a server

Scenario 5: Use HTTP POST command to upload a stream to a server

Scenario 6: Use HTTP POST command to upload a MIME form using a HttpMultipartFormDataContent class

Scenario 7: Use HTTP POST command to upload a stream - use progress indicator and request/response encoding settings

Scenario 8: Use HTTP POST command to upload custom content

Scenario 9-12: Query for cookies, set new cookies, delete existing cookies, and disable cookies

Scenario 13: Use a filter to retry HTTP requests if required

Scenario 14: Use a filter to adapt download behavior based on whether the device is on a metered network connection or not

Scenario 15: Validate the server certificate

**Note**  This sample by default requires network access using the loopback interface.

For a sample that shows how to use [HttpClient](http://msdn.microsoft.com/library/windows/apps/dn298639) so that the app is always connected and always reachable using background network notifications in a Universal Windows Platform (UWP) app, download the [ControlChannelTrigger HttpClient sample](http://go.microsoft.com/fwlink/p/?linkid=258323) .

## Network capabilities

This sample requires that network capabilities be set in the *Package.appxmanifest* file to allow the app to access the network at runtime. These capabilities can be set in the app manifest using Microsoft Visual Studio.

To build the sample, set the following network capabilities:

-   **Internet (Client & Server)**: This sample has complete access to the network for both client operations (outbound-initiated access) and server operations (inbound-initiated access). This allows the app to download various types of content from an HTTP server and upload content to an HTTP server located on the Internet or on a local intranet. This is represented by the **Capability name = "internetClientServer"** tag in the app manifest.

    **Private Networks (Client & Server)**: The sample has inbound and outbound network access on a home or work network (a local intranet). This allows the app to download various types of content from an HTTP server and upload content to an HTTP server located on a local intranet. The **Private Networks (Client & Server)** capability is represented by the **Capability name = "privateNetworkClientServer"** tag in the app manifest.

For more information on network capabilities, see [How to set network capabilities](http://msdn.microsoft.com/library/windows/apps/hh770532).


## Related topics

### Other - C\#/VB/C++ and XAML

[Adding support for networking (XAML)](http://msdn.microsoft.com/library/windows/apps/hh452751)  
[Connecting to network services (XAML)](http://msdn.microsoft.com/library/windows/apps/hh452976)  
[Connecting with WebSockets (XAML)](http://msdn.microsoft.com/library/windows/apps/hh994396)  
[HttpClient](https://msdn.microsoft.com/library/windows/apps/mt187345)  
[Networking basics](https://msdn.microsoft.com/library/windows/apps/mt280233)  
[Troubleshooting and debugging network connections](http://msdn.microsoft.com/library/windows/apps/hh770534)  

### Other - JavaScript and HTML

[Adding support for networking (HTML)](http://msdn.microsoft.com/library/windows/apps/hh452752)  
[Connecting to network services (HTML)](http://msdn.microsoft.com/library/windows/apps/hh452977)  
[Connecting with WebSockets (HTML)](http://msdn.microsoft.com/library/windows/apps/hh761442)  
[How to connect to an HTTP server using Windows.Web.Http (HTML)](http://msdn.microsoft.com/library/windows/apps/dn469430)  
[How to set background connectivity options (HTML)](http://msdn.microsoft.com/library/windows/apps/hh771189)  
[How to set network capabilities (HTML)](http://msdn.microsoft.com/library/windows/apps/hh770532)  
[Troubleshooting and debugging network connections](http://msdn.microsoft.com/library/windows/apps/hh770534)  

### Other resources

[Adding support for networking](http://msdn.microsoft.com/library/windows/apps/hh452752)  
[Connecting to a WebSocket service](http://msdn.microsoft.com/library/windows/apps/hh761442)  
[How to configure network isolation capabilities](http://msdn.microsoft.com/library/windows/apps/hh770532)  
[How to secure HttpClient connections](http://msdn.microsoft.com/library/windows/apps/hh781240)  
[Quickstart: Connecting using HttpClient](http://msdn.microsoft.com/library/windows/apps/hh781239)  

### Reference

[HttpClient](http://msdn.microsoft.com/library/windows/apps/dn298639)  
[Windows.Web.Http](http://msdn.microsoft.com/library/windows/apps/dn279692)  
[Windows.Web.Http.Headers](http://msdn.microsoft.com/library/windows/apps/dn252713)  
[Windows.Web.Http.Filters](http://msdn.microsoft.com/library/windows/apps/dn298623)  

## System requirements

**Client:** Windows 10 version 1604 preview

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10 version 1604 preview

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select **Build** \> **Deploy Solution**. 

### Deploying and running the Windows version of the sample

This sample requires that a web server is available for the app to access for uploading and downloading files. The web server must be started before the app is run. The web server must also have an *HttpClientSample* path available for uploads and downloads. The sample includes a PowerShell script that will install IIS on the local computer, create the *HttpClientSample* folder on the server, copy files to this folder, and enable IIS.

The easiest way to run the sample is to use the provided web server scripts. Browse to the *Server* folder in your sample folder to setup and start the web server. There are two options possible.

-   Start PowerShell elevated (Run as administrator) and run the following command:

    **.\\SetupServer.ps1**

    Note that you may also need to change script execution policy.

-   Start an elevated Command Prompt (Run as administrator) and run following command:

    **PowerShell.exe -ExecutionPolicy Unrestricted -File SetupServer.ps1**

When the web server is not needed anymore, please browse to the *Server* folder in you sample folder and run one of the following:

-   Start PowerShell elevated (Run as administrator) and run the following command:

    **.\\RemoveServer.ps1**

    Note that you may also need to change script execution policy.

-   Start an elevated Command Prompt (Run as administrator) and run following command:

    **PowerShell.exe -ExecutionPolicy Unrestricted -File RemoveServer.ps1**

The sample can run using other web servers or websites, not only the one provided with the sample. However for scenarios in the sample, the web server may need to be configured to interpret the query string and cookies similar to IIS so it can send the expected responses.

If IIS is used on a different computer, then the previous scripts can be used with minor changes.

-   Copy the *Server* folder to the device where IIS will be run.
-   Run the above scripts to install IIS, create the *HttpClientSample* folder on the server, copy files to this folder, and enable IIS.

The sample must also be updated when run against a non-localhost web server. To configure the sample for use with IIS on a different device:

-   The hostname of the server to connect to needs to be updated. This can be handled in two ways. The **AddressField** element in the HTML or XAML files can be edited so that "localhost" is replaced by the hostname or IP address of the web server. Alternately when the app is run, enter the hostname or IP address of the web server instead of the "localhost" value in the Address textbox.

**Note**  IIS is not available on Windows Phone. Instead, set up the web server on a separate 64-bit or 32-bit computer and follow the steps for using the sample against non-localhost web server.

**Note**  When used with the supplied scripts, this sample communicates with another process (IIS server which is a desktop app) on the same machine over loopback for demonstration purposes only. A UWP app that communicates over loopback to another process that represents a UWP app or a desktop app is not allowed and such apps will not pass Store validation. For more information, see [How to enable loopback and troubleshoot network isolation](http://msdn.microsoft.com/library/windows/apps/hh780593).

However if a server different than IIS is used, then this requires some special configuration of the server to create the *HttpClientSample* folder.

Copy the *Server\\webSite* directory to the *HttpClientSample* folder on the web server and configure the server to allow GET and POST requests.

To configure the sample for use with a web server different than IIS not using localhost:

The target URI field should be updated. This can be handled in two ways. The **AddressField** element in the HTML or XAML files can be edited so that the URI is replaced by a URI for the non-IIS server. Alternately when the app is run, enter the URI to access on the web server instead of the default value in the Address textbox.

Once the server is ready, you can deploy and run the sample:

- To debug the sample and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.




