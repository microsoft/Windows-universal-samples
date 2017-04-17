<!---
  category: NetworkingAndWebServices 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620609
--->

# StreamSocket sample

Shows how to a use stream (TCP) socket to send and receive data using the [StreamSocket](http://msdn.microsoft.com/library/windows/apps/br226882) 
and related classes in the [Windows.Networking.Sockets](http://msdn.microsoft.com/library/windows/apps/br226960) namespace in your Universal Windows Platform (UWP) app.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

The client component of the sample creates a TCP socket to make a network connection, uses the socket to send data, and closes the socket. The server component sets up 
a TCP listener that provides a connected socket for every incoming network connection, uses the socket to receive data from the client, and closes the socket. This sample 
is provided in the JavaScript, C\#, and C++ programming languages.

The client component of the sample demonstrates the following features:

-   Use the [StreamSocket](http://msdn.microsoft.com/library/windows/apps/br226882) class to create a TCP socket.
-   Make a network connection to a TCP network server using one of the [StreamSocket.ConnectAsync](http://msdn.microsoft.com/library/windows/apps/hh701504) methods.
-   Send data to the server using the [Streams.DataWriter](http://msdn.microsoft.com/library/windows/apps/br208154) object which allows a programmer to write common types 
(integers and strings, for example) on any stream.
-   Close the socket.
-   Attempt a socket connection using SSL to web server at port 443 (HTTPS), evaluate the server certificate validity, and display its properties. A certificate error is 
expected since the self-signed certificate is not trusted and issued to a different site name.

The server component of the sample demonstrates the following features:

-   Use the [StreamSocketListener](http://msdn.microsoft.com/library/windows/apps/br226906) class to create a TCP socket to listen for an incoming TCP connection.
-   Bind the socket to a local service name to listen for an incoming network connection using the 
[StreamSocketListener.BindServiceNameAsync](http://msdn.microsoft.com/library/windows/apps/dn298302) method.
-   Receive a [StreamSocketListener.ConnectionReceived](http://msdn.microsoft.com/library/windows/apps/hh701494) event that indicates that a connection was received on 
the [StreamSocketListener](http://msdn.microsoft.com/library/windows/apps/br226906) object.
-   Receive data from the client using the [Streams.DataReader](http://msdn.microsoft.com/library/windows/apps/br208119) object which allows a programmer to read common 
types (integers and strings, for example) on any stream.
-   Close the socket.

For a sample that shows how to use a datagram (UDP) socket to send and receive data in a UWP app, download the [DatagramSocket sample](/Samples/DatagramSocket).

For a sample that shows how to use a [StreamSocket](http://msdn.microsoft.com/library/windows/apps/br226882) so that the app is always connected and always reachable 
using background network notifications in a UWP app, download the [SocketActivityTrigger StreamSocket Sample](/Samples/SocketActivityStreamSocket).

## Network capabilities

This sample requires that network capabilities be set in the *Package.appxmanifest* file to allow the app to access the network at runtime. These capabilities can be set 
in the app manifest using Microsoft Visual Studio. For more information on network capabilities, see 
[How to set network capabilities](http://msdn.microsoft.com/library/windows/apps/hh770532).

**Note** Network communications using an IP loopback address cannot normally be used for interprocess communication between a UWP app and a different process 
(a different UWP app or a desktop app) because this is restricted by network isolation. Network communication using an IP loopback address is allowed within 
the same process for communication purposes in a UWP app. For more information, see 
[How to enable loopback and troubleshoot network isolation](https://msdn.microsoft.com/library/windows/apps/hh780593).

## Related topics

### Reference

[StreamSocket](http://msdn.microsoft.com/library/windows/apps/br226882)  
[StreamSocketListener](http://msdn.microsoft.com/library/windows/apps/br226906)  
[Windows.Networking](http://msdn.microsoft.com/library/windows/apps/br207124)  
[Windows.Networking.Sockets](http://msdn.microsoft.com/library/windows/apps/br226960)  
[Windows.Storage.Streams.DataReader](http://msdn.microsoft.com/library/windows/apps/br208119)  
[Windows.Storage.Streams.DataWriter](http://msdn.microsoft.com/library/windows/apps/br208154)  
[SocketActivityTrigger](https://msdn.microsoft.com/library/windows/apps/windows.applicationmodel.background.socketactivitytrigger.aspx)  

### Samples

[DatagramSocket sample](/Samples/DatagramSocket)  
[SocketActivityTrigger StreamSocket Sample](/Samples/SocketActivityStreamSocket)  

### Other - C\#/VB/C++ and XAML

[Adding support for networking (XAML)](http://msdn.microsoft.com/library/windows/apps/hh452751)  
[Connecting to network services (XAML)](http://msdn.microsoft.com/library/windows/apps/hh452976)  
[How to secure socket connections with TLS/SSL (XAML)](http://msdn.microsoft.com/library/windows/apps/jj150597)  
[How to send and receive network data with a stream socket (XAML)](http://msdn.microsoft.com/library/windows/apps/jj150599)  
[Networking basics](https://msdn.microsoft.com/library/windows/apps/mt280233)  
[How to use advanced socket controls (XAML)](http://msdn.microsoft.com/library/windows/apps/jj150598)  
[Proximity and tapping (XAML)](http://msdn.microsoft.com/library/windows/apps/hh465221)  
[Troubleshooting and debugging network connections](http://msdn.microsoft.com/library/windows/apps/hh770534)  

### Other - JavaScript and HTML

[Adding support for networking (HTML)](http://msdn.microsoft.com/library/windows/apps/hh452752)  
[Connecting to network services (HTML)](http://msdn.microsoft.com/library/windows/apps/hh452977)  
[How to secure socket connections with TLS/SSL (HTML)](http://msdn.microsoft.com/library/windows/apps/hh780595)  
[How to send and receive network data with a stream socket (HTML)](http://msdn.microsoft.com/library/windows/apps/hh452996)  
[How to set network capabilities (HTML)](http://msdn.microsoft.com/library/windows/apps/hh770532)  
[How to use advanced socket controls (HTML)](http://msdn.microsoft.com/library/windows/apps/hh780596)  
[Supporting proximity and tapping (HTML)](http://msdn.microsoft.com/library/windows/apps/hh465229)  
[Troubleshooting and debugging network connections](http://msdn.microsoft.com/library/windows/apps/hh770534)  

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

### Deploying and running the sample on a Windows 10 Desktop

- To debug the sample and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.

For the app to attempt a socket connection using SSL to a web server at port 443 (HTTPS) and view the server certificate, this sample requires that a web server is available that supports HTTPS. The web server must be started before the app is run. The sample includes a PowerShell script that will install and enable IIS on a local computer, generate a self-signed, untrusted certificate, and enable HTTPS connections. The easiest way to run the sample is to use the provided web server scripts.

Browse to the *Server* folder in your sample folder to setup and start the web server. There are two options possible.

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

The sample can run using any web server, not only the one provided with the sample. If IIS is used on a different computer, then the previous scripts can be used with minor changes.

-   Copy the *Server* folder to the device where IIS will be run.
-   Run the above scripts to install and enable IIS, generate a self-signed certificate, and enable HTTPS connections.

The sample must also be updated when run against a non-localhost web server. To configure the sample for use with IIS on a different device:

-   Additional capabilities may need to be added to the app manifest for the sample. For example, **Internet (Client & Server)** if the web server is located on the Internet not on a local intranet.
-   The hostname of the server to connect to also needs to be updated. This can be handled in two ways. The **HostNameForConnect** element in the HTML or XAML files can be edited so that "localhost" is replaced by the hostname or IP address of the web server. Alternately when the app is run, enter the hostname or IP address of the web server instead of the default "localhost" value.

**Note** IIS is not available on ARM builds. Instead, set up the web server on a separate 64-bit or 32-bit computer and follow the steps for using the sample against non-localhost web server.

However if a server different than IIS is used, then this requires some special configuration of the server.

-   Configure the web server to accept HTTPS connections.
-   Generate a self-signed certificate for the web server with SN=www.fabrikam.com.

To configure the sample for use with a web server different than IIS not using localhost:

-   Additional capabilities may need to be added to the app manifest for the sample. For example, **Internet (Client & Server)** if the web server is located on the Internet not on a local intranet.
-   The hostname of the server to connect to also needs to be updated. This can be handled in two ways. The **HostNameForConnect** element in the HTML or XAML files can be edited so that "localhost" is replaced by the hostname or IP address of the web server. Alternately when the app is run, enter the hostname or IP address of the web server instead of the default "localhost" value.

### Deploying and running the sample on a Windows 10 Phone

IIS is not available on Windows Phone. For the app to attempt a socket connection using SSL to a web server, there are two options:

-   The easiest way to run the sample is to use the provided web server scripts on a separate 64-bit or 32-bit device that can run IIS. Follow the instructions for deploying and running the Windows version of the sample using IIS on a different device.
-   Use a web server different than IIS on a separate device. Follow the instructions for deploying and running the Windows version of the sample using a non-IIS web server.
