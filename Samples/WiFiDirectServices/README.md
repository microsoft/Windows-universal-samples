<!---
  category: NetworkingAndWebServices 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620627
--->

# Wi-Fi Direct services sample

Shows how to use the Wi-Fi Direct services API to publish and discover services over Wi-Fi Direct.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

The Wi-Fi Direct services API provides the "Enable" service, according to the Wi-Fi Alliance specification. This allows developers to define custom "services" that can be advertised by one device and discovered from another.

Specifically, this sample shows how to:

- **Advertise a service:** A device can advertise a service that other devices can discover and connect to. There are optional settings that an advertiser may specify, such as additional service information to share during discovery, and options for how to establish a connection.
- **Discover a service:** Specify a service name and discover it over Wi-Fi Direct.
- **Connect to a service/Accept an incoming connection:** After discovering a service, a device may connect to it. The advertising device may accept or decline the connection, if they did not choose to advertise the service as auto-accept.
- **Send data over Wi-Fi Direct after connecting to a service:** Connected peers may open TCP and UDP sockets and send data. This sample just sends text strings as a sample.

**Note** This sample requires two or more devices with Wi-Fi chipsets and drivers that support Wi-Fi Direct services.
 
**Note** This sample was created using one of the universal app templates available in Visual Studio. It shows how its solution is structured so it can run on Windows 10. For more info about how to build apps that target Windows and Windows Phone with Visual Studio, see [Build apps that target Windows 10 by using Visual Studio](http://msdn.microsoft.com/library/windows/apps/dn609832).

## Related topics

### Reference

[Wi-Fi Direct Services](https://msdn.microsoft.com/library/windows/apps/windows.devices.wifidirect.services.aspx)  
[Sockets](https://msdn.microsoft.com/library/windows/apps/windows.networking.sockets.aspx)  

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:**  Windows 10

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

- To debug the sample and then run it, press F5 or select **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or select **Debug** \> **Start Without Debugging**. 

**Note** You will need to deploy the sample to at least two devices in order to actually discover and connect to services, unless you are using the sample to interact with other devices that support Wi-Fi Direct services.
