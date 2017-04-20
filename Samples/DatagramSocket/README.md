<!---
  category: NetworkingAndWebServices 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620534
--->

# DatagramSocket sample

Shows how to a use datagram (UDP) socket to send and receive data using the [DatagramSocket](http://msdn.microsoft.com/library/windows/apps/br241319) 
and related classes in the [Windows.Networking.Sockets](http://msdn.microsoft.com/library/windows/apps/br226960) namespace in your Universal Windows Platform (UWP) app.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

The client component of the sample creates a UDP socket, uses the socket to send and receive data, and closes the socket. The server component of the sample creates a UDP socket to listen for incoming network packets, receives incoming UDP packets from the client, sends data to the client, and closes the socket. This sample is provided in the JavaScript, C\#, and C++ programming languages.

The client component of the sample demonstrates the following features:

-   Use the [DatagramSocket](http://msdn.microsoft.com/library/windows/apps/br241319) class to create a UDP socket for the client to send and receive data.
-   Add a handler for a [DatagramSocket.MessageReceived](http://msdn.microsoft.com/library/windows/apps/br241358) event that indicates that a UDP datagram was received on the [DatagramSocket](http://msdn.microsoft.com/library/windows/apps/br241319) object.
-   Set the remote endpoint for a UDP network server where packets should be sent using one of the [DatagramSocket.ConnectAsync](http://msdn.microsoft.com/library/windows/apps/hh701219) methods.
-   Send data to the server using the [Streams.DataWriter](http://msdn.microsoft.com/library/windows/apps/br208154) object which allows a programmer to write common types (integers and strings, for example) on any stream.
-   Close the socket.
-   Send multicast and broadcast packets using IOutputStreams obtained through [DatagramSocket.GetOutputStreamAsync](https://msdn.microsoft.com/library/windows/apps/hh701265.aspx).

The server component of the sample demonstrates the following features:

-   Use the [DatagramSocket](http://msdn.microsoft.com/library/windows/apps/br241319) class to create a UDP socket to listen for and receive incoming datagram packets and for sending packets.
-   Add a handler for a [DatagramSocket.MessageReceived](http://msdn.microsoft.com/library/windows/apps/br241358) event that indicates that a UDP datagram was received on the [DatagramSocket](http://msdn.microsoft.com/library/windows/apps/br241319) object.
-   Bind the socket to a local service name to listen for incoming UDP packets using the [DatagramSocket.BindServiceNameAsync](http://msdn.microsoft.com/library/windows/apps/dn279143) method.
-   Receive a [DatagramSocket.MessageReceived](http://msdn.microsoft.com/library/windows/apps/br241358) event that indicates that a UDP datagram was received on the [DatagramSocket](http://msdn.microsoft.com/library/windows/apps/br241319) object.
-   Receive data from the client using the [DatagramSocket.MessageReceived](http://msdn.microsoft.com/library/windows/apps/br241358) handler. The [DatagramSocketMessageReceivedEventArgs](http://msdn.microsoft.com/library/windows/apps/br241344) object passed to the **DatagramSocket.MessageReceived** handler allows an app to receive data from the client and also determine the remote address and port that sent the data.
-   Close the socket.
-   Listen for multicast packets while coexisting with other multicast applications by using the [DatagramSocketControl.MulticastOnly](https://msdn.microsoft.com/library/windows/apps/windows.networking.sockets.datagramsocketcontrol.multicastonly.aspx) control option.

For a sample that shows how to use a stream (TCP) socket to send and receive data in a UWP app, download the [StreamSocket sample](/Samples/StreamSocket).

If you are interested in developing an app that is always connected and always reachable using background network notifications in a UWP app, refer to the [SocketActivityTrigger](https://msdn.microsoft.com/library/windows/apps/windows.applicationmodel.background.socketactivitytrigger.aspx) documentation. The [SocketActivityTrigger StreamSocket Sample](/Samples/SocketActivityStreamSocket) is a good starting point, since the programming pattern with DatagramSocket is essentially the same as with StreamSocket.

## Network capabilities

This sample requires that network capabilities be set in the *Package.appxmanifest* file to allow the app to access the network at runtime. These capabilities can be set in the app manifest using Microsoft Visual Studio. For more information on network capabilities, see [How to set network capabilities](http://msdn.microsoft.com/library/windows/apps/hh770532).

**Note** Network communications using an IP loopback address cannot normally be used for interprocess communication between a Universal Windows Platform (UWP) app and a different process (a different UWP app or a desktop app) because this is restricted by network isolation. Network communication using an IP loopback address is allowed within the same process for communication purposes in a UWP app. For more information, see [How to set network capabilities](http://msdn.microsoft.com/library/windows/apps/hh770532).

## Related topics

### Reference

[DatagramSocket](http://msdn.microsoft.com/library/windows/apps/br226882)  
[DatagramSocketMessageReceivedEventArgs](http://msdn.microsoft.com/library/windows/apps/br241344)  
[Windows.Networking](http://msdn.microsoft.com/library/windows/apps/br207124)  
[Windows.Networking.Sockets](http://msdn.microsoft.com/library/windows/apps/br226960)  
[Windows.Storage.Streams.DataReader](http://msdn.microsoft.com/library/windows/apps/br208119)  
[Windows.Storage.Streams.DataWriter](http://msdn.microsoft.com/library/windows/apps/br208154)  
[SocketActivityTrigger](https://msdn.microsoft.com/library/windows/apps/windows.applicationmodel.background.socketactivitytrigger.aspx)  

### Samples

[StreamSocket sample](/Samples/StreamSocket)  
[SocketActivityTrigger StreamSocket Sample](/Samples/SocketActivityStreamSocket)  

### Other - C\#/VB/C++ and XAML

[Adding support for networking (XAML)](http://msdn.microsoft.com/library/windows/apps/hh452751)  
[Connecting to network services (XAML)](http://msdn.microsoft.com/library/windows/apps/hh452976)  
[How to send and receive network data with a datagram socket (XAML)](http://msdn.microsoft.com/library/windows/apps/hh452986)  
[Networking basics](https://msdn.microsoft.com/library/windows/apps/mt280233)  
[How to use advanced socket controls (XAML)](http://msdn.microsoft.com/library/windows/apps/jj150598)  
[Troubleshooting and debugging network connections](http://msdn.microsoft.com/library/windows/apps/hh770534)  

### Other - JavaScript and HTML

[Adding support for networking (HTML)](http://msdn.microsoft.com/library/windows/apps/hh452752)  
[Connecting to network services (HTML)](http://msdn.microsoft.com/library/windows/apps/hh452977)  
[How to send and receive network data with a datagram socket (HTML)](http://msdn.microsoft.com/library/windows/apps/hh452986)  
[How to set network capabilities (HTML)](http://msdn.microsoft.com/library/windows/apps/hh770532)  
[How to use advanced socket controls (HTML)](http://msdn.microsoft.com/library/windows/apps/hh780596)  
[Troubleshooting and debugging network connections](http://msdn.microsoft.com/library/windows/apps/hh770534)  

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

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 

