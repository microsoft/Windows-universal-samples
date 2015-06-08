Socket Activity Trigger Stream Socket Sample
============================================

This sample shows how to use the Socket Activity Stream Socket API to keep a socket connection alive beyond the lifetime of the application.

This sample connects to a server listening on TCP port 40404. The application can be terminated but the connection to the server will remain alive and will get background notifications if the server sends messages on the connection.

This sample requires the following capabilities:

-   **Private Networks (Client & Server)** - If the server it connects to is in intranet then it requires private Network capabilities. 

**Note**  The Windows version of this sample by default requires network access using the loopback interface.

**Network capabilities**

For more information on network capabilities, see [How to set network capabilities](http://msdn.microsoft.com/library/windows/apps/hh770532).

**Note**  Network communications using an IP loopback address cannot normally be used for inter process communication between a Windows Runtime app and a different process (a different Windows Runtime app or a desktop app) because this is restricted by network isolation. Network communication using an IP loopback address is allowed within the same process for communication purposes in a Windows Runtime app. For more information, see [How to set network capabilities](http://msdn.microsoft.com/library/windows/apps/hh770532).


Related topics
--------------

**Other resources**

[Adding support for networking](http://msdn.microsoft.com/library/windows/apps/hh452752)

[How to configure network isolation capabilities](http://msdn.microsoft.com/library/windows/apps/hh770532)

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
----------------

This sample requires two computers that can connect to each other over a network. One computer will play the role of the client, while the other computer will play the role of the server. 

To deploy this sample after building it, select Deploy Solution from the Build menu. 

The sample must be deployed to both the client and server computers.

To run the sample, first the start the server app on the server computer. It will wait for the clients to connect.

After the server app has started, start the client app on the client computer. 

1. Specify the server name in the text box and click connect. 

2. Once a connection is established, you can terminate the client application.

Once the client is connected, you can send messages from the server application text box. The client displays received messages as toast notifications. 

