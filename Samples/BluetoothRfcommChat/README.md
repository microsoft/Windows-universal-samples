<!---
  category: Communications
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=626688
--->

# Bluetooth RFCOMM chat sample

This sample demonstrates the use of classes in the
[**Windows.Devices.Bluetooth.Rfcomm**](https://msdn.microsoft.com/en-us/library/windows/apps/windows.devices.bluetooth.rfcomm.aspx)
namespace
to communicate over sockets.

**Note:** Two devices will be needed to run this sample.

- **Chat Client**: Connects to an RFCOMM server running on another device and establishes a socket connection.
  Demonstrates reading from and writing to the server.
- **Foreground Chat Server**: Publish an RFCOMM server with a custom service.
  When connected to by a client, establishes a socket connection and reads from and writes to the client.
- **Background Chat Server**: Same as the Foreground Chat Server,
  except that it initializes a Background task that runs only when a client device is connected.
  The Background task also causes the host device to advertise support for the custom service in the SDP record until it is unregistered.

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](https://dev.windows.com)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples

[DataReaderWriter sample](../DataReaderWriter)

[DeviceEnumeration sample](../DeviceEnumerationAndPairing)

[StreamSocket sample](../StreamSocket)

### Reference

[**Windows.Devices.Bluetooth.Rfcomm**](https://msdn.microsoft.com/en-us/library/windows/apps/windows.devices.bluetooth.rfcomm.aspx)
namespace

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
