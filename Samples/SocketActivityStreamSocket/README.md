<!---
  category: NetworkingAndWebServices 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620606
--->

# Socket activity trigger stream socket sample

This sample shows how to use the Socket Activity Stream Socket API to keep a socket connection alive beyond the lifetime of the application.

This sample connects to a server listening on TCP port 40404. The application can be terminated but the connection to the server will remain alive and will get background notifications if the server sends messages on the connection.

This sample requires the following capabilities:

-   **Private Networks (Client & Server)** - If the server it connects to is in intranet then it requires private Network capabilities. 

For more information on network capabilities, see [How to set network capabilities](http://msdn.microsoft.com/library/windows/apps/hh770532).

**Note**  This sample is comprised of two apps: SocketActivityStreamSocket (client-side) and StreamSocketListenerServer (server-side). Due to loopback restrictions enforced by network isolation policy, these apps must run on separate machines for them to be able to communicate with each other. The sample scenario will not work if both apps are running on the same machine.


## Related topics

### Other resources

[Adding support for networking](http://msdn.microsoft.com/library/windows/apps/hh452752)

[How to configure network isolation capabilities](http://msdn.microsoft.com/library/windows/apps/hh770532)

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select **Build** \> **Deploy Solution**. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.

This sample requires two computers that can connect to each other over a network. One computer will play the role of the client, while the other computer will play the role of the server. 

To deploy this sample after building it, select Deploy Solution from the Build menu. 

The sample must be deployed to both the client and server computers.

To run the sample, first the start the server app on the server computer. It will wait for the clients to connect.

After the server app has started, start the client app on the client computer. 

1. Specify the server name in the text box and click connect. 

2. Once a connection is established, you can terminate the client application.

Once the client is connected, you can send messages from the server application text box. The client displays received messages as toast notifications. 

