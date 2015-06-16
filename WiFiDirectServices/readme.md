# Wi-Fi Direct Services sample

This sample shows how to use the Wi-Fi Direct services API to publish and discover services over Wi-Fi Direct.

The Wi-Fi Direct services API provides the "Enable" service, according to the Wi-Fi Alliance specification. This allows developers to define custom "services" that can be advertised by one device and discovered from another.

Specifically, this sample shows how to:

- **Advertise a service:** A device can advertise a service that other devices can discover and connect to. There are optional settings that an advertiser may specify, such as additional service information to share during discovery, and options for how to establish a connection.
- **Discover a service:** Specify a service name and discover it over Wi-Fi Direct.
- **Connect to a service/Accept an incoming connection:** After discovering a service, a device may connect to it. The advertising device may accept or decline the connection, if they did not choose to advertise the service as auto-accept.
- **Send data over Wi-Fi Direct after connecting to a service:** Connected peers may open TCP and UDP sockets and send data. This sample just sends text strings as a sample.

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute. This sample requires two or more devices with Wi-Fi chipsets and drivers that support Wi-Fi Direct services.
 
To obtain information about Windows 10, go to [Windows 10](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Reference

<!-- Add links to related API -->

[Wi-Fi Direct Services](https://msdn.microsoft.com/en-us/library/windows/apps/windows.devices.wifidirect.services.aspx)

[Sockets](https://msdn.microsoft.com/en-us/library/windows/apps/windows.networking.sockets.aspx)

## System requirements

**Client:** Windows 10 Insider Preview

**Server:** Windows 10 Insider Preview

**Phone:**  Windows 10 Insider Preview

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

**Note** You will need to deploy the sample to at least two devices in order to actually discover and connect to services, unless you are using the sample to interact with other devices that support Wi-Fi Direct services.
