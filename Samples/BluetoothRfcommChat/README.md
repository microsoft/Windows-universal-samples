<!---
  category: Communications
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=626688
--->

# Bluetooth RFCOMM chat sample

Shows how to use the [Windows.Devices.Bluetooth.Rfcomm](https://msdn.microsoft.com/library/windows/apps/windows.devices.bluetooth.rfcomm.aspx)
namespace to communicate over sockets.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

**Note:** You need two devices to run this sample.

##Chat Client
Connects to an RFCOMM server running on another device and establishes a socket connection.
Demonstrates reading from and writing to the server.

In order to use this scenario, you must have an Rfcomm server running on a remote device at the time of connection.
Press the "Run" button, find the device in the list and select it.
If the server has been found on the remote device,
an Rfcomm socket will be established and you can chat with the remote device.

Be sure to "Request Access" to the device if the remote device is likely to become paired.

##Foreground Chat Server
Publish an RFCOMM server with a custom service.
When connected to by a client, establishes a socket connection to communicate with client.
The server must be published when it is connected to for the client to find a valid service.

##Background Chat Server
Same as the Foreground Chat Server,
except that it initializes a Background task that runs only when a client device is connected.
The host device will advertise support for the custom service in the SDP record until the background task is unregistered.

**Note:** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples

[DataReaderWriter sample](../DataReaderWriter)  
[DeviceEnumeration sample](../DeviceEnumerationAndPairing)  
[StreamSocket sample](../StreamSocket)  

### Reference

[Windows.Devices.Bluetooth.Rfcomm](https://msdn.microsoft.com/library/windows/apps/windows.devices.bluetooth.rfcomm.aspx) namespace  

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

## Build the sample

1. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2. Go to the directory to which you unzipped the sample. Then go to the subdirectory containing the sample in the language you desire - either C++, C#, or JavaScript. Double-click the Visual Studio 2015 Solution (.sln) file. 
3. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**. 

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 
