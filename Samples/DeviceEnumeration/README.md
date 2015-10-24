<!---
  category: DevicesSensorsAndPower
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620536&clcid=0x409
--->

# Device enumeration sample

This sample shows how to use the Windows.Devices.Enumeration APIs find devices internally connected to the system, externally connected, or nearby over wireless or networking protocols and get information about them.  It also shows how to pair wireless and networking devices with the system. 

The sample demonstrates eight scenarios:

1.  Device Picker Common Control: This scenario demonstrates the use of the Windows.Devices.Enumeration.DevicePicker. DevicePicker is a UI control that allows users of your app to pick a device. After they pick a device you can get information from it and start using it through the appropriate device API. 

2.  Enumerate and Watch Devices: This scenario demonstrates the use of the Windows.Devices.Enumeration DeviceWatcher. Device Watcher allows you to find and recieve updates for devices you're interested in. Even if you don't need updates for devices as the arrive, depart, or change, this method still has advantages over FindAllAsync in that the results come back one-by-one has they are discovered, rather than waiting for all results to be retreived and then returned in a single collection. 

3.  Enumerate and Watch Devices in a Background Task: This scenario demonstrates the use of the Windows.Devices.Enumeration DeviceWatcherTrigger. DeviceWatcherTrigger is similar to DeviceWatcher, but is performed in a background task.  First you create a DeviceWatcher object, but instead of starting the DeviceWatcher in the foreground, you get the background trigger and then register it. Your background task is called with initial results and then called repeatedly over time as the device you're watching arrive, depart, or change. 

4.  Enumerate Snapshot of Devices: This scenario demonstrates the use of the Windows.Devices.Enumeration DeviceInformation.FindAllAsync. Use for grabbing a quick snapshot of devices you're interested in. This method is simplier to use than the DeviceWatcher, but doesn't allow you to process results as they arrive.  Rather all results are internally added to a collection until the enumeration is complete, then the collection is returned when the async operation completes. 

5.  Get Single Device: This scenario demonstrates the use of the Windows.Devices.Enumeration DeviceInformation.CreateFromIdAsync. This would typically be used when you have a device id saved, and want to retrieve information about that device again. Other uses would be if you have a DeviceInformation.Id from another API and need to obtain a DeviceInformation object. 

6.  Custom Filter with Additional Properties: This scenario demonstrates using custom filters and/or requesting additional properties with the DeviceWatcher. A custom filter and/or requested additional properties can also be done with the DevicePicker, FindAllAsync, and DeviceWatcherTrigger

7.  Request Specific DeviceInformationKind: This scenario demonstrates requesting various DeviceInformationKinds. Methods without the DeviceInformationKind parameter default to DeviceInterface. 

8.  Pair Device: This scenario demonstrates how to pair a device with the system. It uses DeviceInformation.PairAsync on a DeviceInformation which is of kind DeviceInformationKind.AssociationEndpoint. e.g. This method can be used to pair devices like WiFi Direct, WSD, or UPnP device. Bluetooth is not supported at this time, but will be in a future release.


## Related topics

[Enumerating devices article](http://msdn.microsoft.com/en-us/library/windows/apps/Hh464977)

[Windows.Devices.Enumeration reference](http://msdn.microsoft.com/en-us/library/windows/apps/windows.devices.enumeration.aspx)

[Windows Universal App Samples](https://github.com/Microsoft/Windows-universal-samples)

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

## Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 

