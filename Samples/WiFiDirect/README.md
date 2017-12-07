<!---
  category: NetworkingAndWebServices 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620626
--->

# Wi-Fi Direct sample

Shows how to use the Wi-Fi Direct API to discover devices and connect to the them over Wi-Fi Direct.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample demonstrates the following:

- **Advertise a Wi-Fi Direct device:**
A device can advertise its presence so that other Wi-Fi Direct devices
can discover and connect to it. There are optional settings that
an advertiser may specify, such as custom information elements
to share during discovery, and options for how to establish a connection.
An advertiser can also specify advanced optional settings such as
pairing configuration methods (in order of preference)
and the preferred pairing procedure.
The advertiser can also be cofigured in legacy mode wherein legacy
(non-WiFiDirect) clients can connect to it.
The legacy mode also supports specifying a SSID and a passphrase.
- **Discover a Wi-Fi Direct device:** Discover advertising devices over Wi-Fi Direct.
- **Connect to a device/Accept an incoming connection:** After discovering a Wi-Fi Direct device, a device may connect to it. The advertising device may accept or decline the connection.
The connector can also be configured with optional advanced settings
such as the pairing configuration methods (in order of preference)
and the preferred pairing procedure.
- **Send data over Wi-Fi Direct after connecting to a device:** Connected peers may open TCP and UDP sockets and send data. This sample just sends text strings as a sample.

The C++ and JS samples do not demonstrate advanced optional settings yet.
Support for those will come in a future sample.

**Note** This sample requires two or more devices with Wi-Fi chipsets and drivers that support Wi-Fi Direct.
 
**Note** This sample was created using one of the universal app templates available in Visual Studio. It shows how its solution is structured so it can run on Windows 10. For more info about how to build apps that target Windows and Windows Phone with Visual Studio, see [Build apps that target Windows 10 by using Visual Studio](http://msdn.microsoft.com/library/windows/apps/dn609832).

## Related topics

### Reference

[Wi-Fi Direct](https://msdn.microsoft.com/library/windows/apps/windows.devices.wifidirect.aspx)  
[Sockets](https://msdn.microsoft.com/library/windows/apps/windows.networking.sockets.aspx)  

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:**  Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2017 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select **Build** \> **Deploy Solution**. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or select **Debug** \> **Start Without Debugging**. 

**Note** You will need to deploy the sample to at least two devices in order to actually discover and connect to devices, unless you are using the sample to interact with other devices that support Wi-Fi Direct.
