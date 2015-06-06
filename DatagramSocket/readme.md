DatagramSocket sample
=====================

This sample shows you how to a use datagram (UDP) socket to send and receive data using the [**DatagramSocket**](http://msdn.microsoft.com/library/windows/apps/br241319) and related classes in the [**Windows.Networking.Sockets**](http://msdn.microsoft.com/library/windows/apps/br226960) namespace in your Windows Runtime app.

**Note**  This sample was created using one of the universal app templates available in Visual Studio. It shows how its solution is structured so it can run on Windows 10 Technical Preview. For more info about how to build apps that target Windows and Windows Phone with Visual Studio, see [Build apps that target Windows 10 Technical Preview by using Visual Studio](http://msdn.microsoft.com/library/windows/apps/dn609832).

The client component of the sample creates a UDP socket, uses the socket to send and receive data, and closes the socket. The server component of the sample creates a UDP socket to listen for incoming network packets, receives incoming UDP packets from the client, sends data to the client, and closes the socket. This sample is provided in the JavaScript, C\#, and C++ programming languages.

The client component of the sample demonstrates the following features:

-   Use the [**DatagramSocket**](http://msdn.microsoft.com/library/windows/apps/br241319) class to create a UDP socket for the client to send and receive data.
-   Add a handler for a [**DatagramSocket.MessageReceived**](http://msdn.microsoft.com/library/windows/apps/br241358) event that indicates that a UDP datagram was received on the [**DatagramSocket**](http://msdn.microsoft.com/library/windows/apps/br241319) object.
-   Set the remote endpoint for a UDP network server where packets should be sent using one of the [**DatagramSocket.ConnectAsync**](http://msdn.microsoft.com/library/windows/apps/hh701219) methods.
-   Send data to the server using the [**Streams.DataWriter**](http://msdn.microsoft.com/library/windows/apps/br208154) object which allows a programmer to write common types (integers and strings, for example) on any stream.
-   Close the socket.

The server component of the sample demonstrates the following features:

-   Use the [**DatagramSocket**](http://msdn.microsoft.com/library/windows/apps/br241319) class to create a UDP socket to listen for and receive incoming datagram packets and for sending packets.
-   Add a handler for a [**DatagramSocket.MessageReceived**](http://msdn.microsoft.com/library/windows/apps/br241358) event that indicates that a UDP datagram was received on the [**DatagramSocket**](http://msdn.microsoft.com/library/windows/apps/br241319) object.
-   Bind the socket to a local service name to listen for incoming UDP packets using the [**DatagramSocket.BindServiceNameAsync**](http://msdn.microsoft.com/library/windows/apps/dn279143) method.
-   Receive a [**DatagramSocket.MessageReceived**](http://msdn.microsoft.com/library/windows/apps/br241358) event that indicates that a UDP datagram was received on the [**DatagramSocket**](http://msdn.microsoft.com/library/windows/apps/br241319) object.
-   Receive data from the client using the [**DatagramSocket.MessageReceived**](http://msdn.microsoft.com/library/windows/apps/br241358) handler. The [**DatagramSocketMessageReceivedEventArgs**](http://msdn.microsoft.com/library/windows/apps/br241344) object passed to the **DatagramSocket.MessageReceived** handler allows an app to receive data from the client and also determine the remote address and port that sent the data.
-   Close the socket.

**Note**  Use of this sample requires network access using the loopback interface.

**Network capabilities**

This sample requires that network capabilities be set in the *Package.appxmanifest* file to allow the app to access the network at runtime. These capabilities can be set in the app manifest using Microsoft Visual Studio.

-   **Private Networks (Client & Server)**: The sample has inbound and outbound network access on a home or work network (a local intranet). Even though this sample by default runs on loopback, it needs either the **Private Networks (Client & Server)** or **Internet (Client & Server)** capability in order to send and receive UDP packets using a [**DatagramSocket**](http://msdn.microsoft.com/library/windows/apps/br241319). The **Private Networks (Client & Server)** capability is represented by the **Capability name = "privateNetworkClientServer"** tag in the app manifest. The **Internet (Client & Server)** capability is represented by the **Capability name = "internetClientServer"** tag in the app manifest.

-   If the sample is modified to connect to the server component running on a different device on the Internet (a more typical app), the **Internet (Client)** capability must be set for the client component. This is represented by the **Capability name = "internetClient"**.

-   If the sample is modified so the server component is running on a different device on the Internet (a more typical app), the **Internet (Client & Server)** capability must be set for the server component. This is represented by the **Capability name = "internetClientServer"** tag in the app manifest.

For more information on network capabilities, see [How to set network capabilities](http://msdn.microsoft.com/library/windows/apps/hh770532).

**Note**  Network communications using an IP loopback address cannot normally be used for interprocess communication between a Windows Runtime app and a different process (a different Windows Runtime app or a desktop app) because this is restricted by network isolation. Network communication using an IP loopback address is allowed within the same process for communication purposes in a Windows Runtime app. For more information, see [How to set network capabilities](http://msdn.microsoft.com/library/windows/apps/hh770532).

Related topics
--------------

**Other - C\#/VB/C++ and XAML**

[Adding support for networking (XAML)](http://msdn.microsoft.com/library/windows/apps/hh452751)

[Connecting to network services (XAML)](http://msdn.microsoft.com/library/windows/apps/hh452976)

[How to send and receive network data with a datagram socket (XAML)](http://msdn.microsoft.com/library/windows/apps/hh452986)

[How to set network capabilities (XAML)](http://msdn.microsoft.com/library/windows/apps/jj835817)

[How to use advanced socket controls (XAML)](http://msdn.microsoft.com/library/windows/apps/jj150598)

[Troubleshooting and debugging network connections](http://msdn.microsoft.com/library/windows/apps/hh770534)

**Other - JavaScript and HTML**

[Adding support for networking (HTML)](http://msdn.microsoft.com/library/windows/apps/hh452752)

[Connecting to network services (HTML)](http://msdn.microsoft.com/library/windows/apps/hh452977)

[How to send and receive network data with a datagram socket (HTML)](http://msdn.microsoft.com/library/windows/apps/hh452986)

[How to set network capabilities (HTML)](http://msdn.microsoft.com/library/windows/apps/hh770532)

[How to use advanced socket controls (HTML)](http://msdn.microsoft.com/library/windows/apps/hh780596)

[Troubleshooting and debugging network connections](http://msdn.microsoft.com/library/windows/apps/hh770534)

**Reference**

[**DatagramSocket**](http://msdn.microsoft.com/library/windows/apps/br226882)

[**DatagramSocketMessageReceivedEventArgs**](http://msdn.microsoft.com/library/windows/apps/br241344)

[**Windows.Networking**](http://msdn.microsoft.com/library/windows/apps/br207124)

[**Windows.Networking.Sockets**](http://msdn.microsoft.com/library/windows/apps/br226960)

[**Windows.Storage.Streams.DataReader**](http://msdn.microsoft.com/library/windows/apps/br208119)

[**Windows.Storage.Streams.DataWriter**](http://msdn.microsoft.com/library/windows/apps/br208154)

Operating system requirements
-----------------------------

Client

Windows 10 Technical Preview

Server

Windows 10 Technical Preview

Phone

Windows 10 Technical Preview

Build the sample
----------------

1.  Start Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2.  Go to the directory to which you unzipped the sample. Go to the directory named for the sample, and double-click the Visual Studio 2015 Solution (.sln) file.
3.  Press Ctrl+Shift+B, or use **Build** \> **Build Solution**

Run the sample
--------------

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

**Deploying the sample**

-   To deploy the sample: Use **Build** \> **Deploy Solution**


**Deploying and running the sample**

-   To deploy and run the sample: Press F5 or use **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.

