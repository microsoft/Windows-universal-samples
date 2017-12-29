<!---
  category: PlatformArchitecture
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=846904
--->

# Custom Capability sample

This sample demonstrates the use of custom capabilities.

Note that the SCCD file included with this sample is not valid,
but it is included for instructional purposes.
If the PC is Developer Mode, the sample will deploy despite the invalid SCCD file.
When writing your own app, follow the instructions in
[Custom Capabilities for Universal Windows Platform apps](https://msdn.microsoft.com/windows/hardware/drivers/devapps/custom-capabilities-for-universal-windows-platform-apps)
to obtain a properly-signed SCCD file.

## Using a custom capability to connect to an NT service

The "Connect to an NT service" scenario uses a custom capability
to control access to an NT service.
For demonstration purposes, the service reads data from an imaginary device.

The scenario demonstrates the following:

- Declaring a custom capability "microsoft.hsaTestCustomCapability_q536wpkpf5cy2" in the manifest.
- Establishing an RPC connection to the NT service.
- Invoking methods over RPC to initiate data collection and alter the sampling period.
- Using an RPC callback to receive data from the imaginary device.

The service portion of this sample demonstrates the following:

- Converting the capability string to a SID using the `DeriveCapabilitySidsFromName` function.
- Using that SID to create the Security Descriptor for the RPC endpoint.

The following diagram summarizes the communication between the app and the NT service:

```sh                     
                  CLIENT                                   SERVER
         ------------------------                     -----------------
         |                      |                     |               |
         | CustomCapability.exe |                     | RpcServer.exe |
         |                      |                     |               |
         ------------------------                     -----------------
                    |                                         |
                    |                                         |
 Click Start button |-------------StartMetering-------------->|
                    |            [Blocking call]              |
                    |                                         |
                    |                                         |
 Updates Sample     |<------------MeteringData----------------|
 text box           |  [Sent at choosen sample period]        |
                    |                                         |
                    |                                         |
 Move the sample    |-----------SetSamplePeriod-------------->|
 period slider      |<-------SetSamplePeriod completes--------|
                    |                                         |
 Click Stop button  |-------------StopMetering--------------->|
                    |<--------StopMetering completes----------|
                    |                                         |
                    |<-------StartMetering completes----------|
                    |                                         |
```

Before you run this scenario, you must start the corresponding NT service.

* Build the NT service portion of the sample, which requires that the Windows SDK for Desktop C++ Apps be installed.
* Start the service from an elevated command prompt in one of two ways:
  * Install it as a service: `rpcserver.exe -install`
  * Run the service in console mode: `rpcserver.exe -console`

## Using a custom capability to access a custom device

A series of scenarios demonstrate how to use a custom capability to access the
OSR USB FX-2 Learning Kit device.
The code for the OSR USB FX-2 driver can be found in the
[Microsoft Windows-driver-samples repo](https://github.com/Microsoft/Windows-driver-samples/)
under
[usb/umdf2_fx2/driver](https://github.com/Microsoft/Windows-driver-samples/tree/master/usb/umdf2_fx2/driver).
The driver must be built for Windows 10 version 1703 or higher in order to support
the custom capability used by this sample.

The "Connect to the OSR FX-2 device" scenario demonstrates the following:

- Declaring a custom capability "microsoft.hsaTestCustomCapability_q536wpkpf5cy2" in the manifest.
- Updating driver code or driver INF to enable access to driver interface using the custom capability   
- Using a `DeviceWatcher` to enumerate OSR FX-2 devices by using its device interface GUID.
- Using the `CustomDevice.FromIdAsync` method to access a particular instance of the device.

The "Send IOCTLs to the device" scenario demonstrates the following:

- Sending an IOCTL to the device to set the 7 segment LED value.
- Sending an IOCTL to the device to get the 7 segment LED value.
  - The values and meanings of I/O control codes are defined by the device.

The "Handle asynchronous device events" scenario demonstrates the following:

- Sending an IOCTL that does not complete until the DIP switches change.
- Canceling the task which is waiting for the switches to change.
- Sending an IOCTL to read the current state of the switches.
  - The values and meanings of I/O control codes are defined by the device.

To use this scenario, click "Begin Receiving Switch Change Events" and then
change the DIP switches. The app displays the state of the switches when they
change. You can also click "Get Switch State" to read the switch state immediately.

The "Read and Write operations" scenario demonstrates the following:

- Writing data to the device's internal memory by writing to its OutputStream.
- Reading data from the device's internal memory by reading from its InputStream.

Each time you click "Write Block", a message is written to the device,
and each time you click "Read Block", a message is read from the device.
The OSR FX-2 has four message buffers.
When they are full, the next write operation will wait until a message is read,
thereby freeing up a message buffer.
When they are empty, the next read operation will wait until a message is written.

**Note** The Windows universal samples require Visual Studio 2017 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio and the tools for developing Windows apps, go to [Visual Studio](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples

* [IoT-GPIO](/Samples/IoT-GPIO)
* [IoT-I2C](/Samples/IoT-I2C)
* [IoT-SPI](/Samples/IoT-SPI)
* [Custom HID device access](/Samples/CustomHidDeviceAccess)
* [Custom serial device access](/Samples/CustomSerialDeviceAccess)
* [Custom USB device access](/Samples/CustomUsbDeviceAccess)

### Reference

* [Windows.Devices.Custom.CustomDevice runtime class](https://msdn.microsoft.com/library/windows/apps/windows.devices.custom.customdevice.aspx) class
* The [Custom driver access sample](https://code.msdn.microsoft.com/windowsapps/Custom-device-access-sample-43bde679)
  sample has been converted to a Windows 10 app.
* [How to use RPC callbacks](https://support.microsoft.com/kb/96781)
* [Custom Capabilities for Universal Windows Platform apps](https://msdn.microsoft.com/windows/hardware/drivers/devapps/custom-capabilities-for-universal-windows-platform-apps)
* [Hardware access for Universal Windows Platform apps](https://msdn.microsoft.com/windows/hardware/drivers/devapps/hardware-access-for-universal-windows-platform-apps)
## System requirements

**Client:** Windows 10 version 1703

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10 version 1703

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2017 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2017 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or select Debug > Start Without Debugging. 
- To run the "Connect to an NT service" scenario, see additional instructions above.
