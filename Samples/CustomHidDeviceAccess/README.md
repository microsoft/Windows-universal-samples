<!---
  category: DevicesSensorsAndPower
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620527
--->

# Custom HID device sample

Shows how to use the [Windows.Devices.HumanInterfaceDevices](http://msdn.microsoft.com/library/windows/apps/dn264174) API. 
Designed to work with a programmable, USB-based, HID device called the SuperMUTT.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

You can use the sample to toggle an LED on the device by sending feature reports. You can also retrieve a feature report 
to determine the current LED blink pattern. In addition, you can use the sample to send output reports, receive input 
reports, and receive input-report interrupts.

If you are new to the HID protocol and concepts like: feature reports, input reports, and output reports, you can find 
more information [here](http://go.microsoft.com/fwlink/p/?linkid=296834).

You can purchase a SuperMUTT device from [JJG Technologies](http://go.microsoft.com/fwlink/p/?linkid=296610). 
(Note that you will need to update the device firmware before you can run the app.)

The sample demonstrates the following tasks:

-   Enumerating available SuperMUTT devices.
-   Connecting to a specific device.
-   Issuing a feature report to retrieve, or set, the blink pattern on the device LED.
-   Handling input-report events.
-   Receiving an input report.
-   Issuing an output report.
-   Disconnecting from a specific device.

When the app is suspended, or the user removes the SuperMUTT, the sample explicitly closes the device. When the app is resumed, or the user reconnects the SuperMUTT, the sample re-opens the device.

**Enumerating available SuperMUTT devices.**

When you start the application and press the **Start Device Watcher** button, the app executes code that enumerates the available SuperMUTT devices.

Internally, the sample accomplishes this by retrieving a device selector and using this selector to create a device watcher (which looks for instances of the SuperMUTT device). The app retrieves a device selector by invoking the **HidDevice.GetDeviceSelector** method. This method returns an Advanced Query Syntax (AQS) string. The app then passes the AQS string to the **DeviceInformation.CreateWatcher** method.

**Connecting to a SuperMUTT device.**

When you click the corresponding device entry (identified by a string that contains the Vendor ID and Product ID) in the Select a HID Device list, the app establishes a connection to that specific device.

**Issuing a feature report**

When you choose the **Feature Reports** scenario the app displays a **Get Led Blink Pattern** and a **Set Led Blink Pattern** button.

To retrieve the current blink pattern, click the **Get Led Blink Pattern** button and view the current setting in the adjacent control. To set the current blink pattern, enter a value in the control and click the **Set Led Blink Pattern** button.

**Handling input-report events**

When you choose the **Input Report Events** scenario the app displays a **Register For Event** and an **Unregister From Event** button.

To receive input-report interrupts, click the **Register For Event** button and view the output under the Scenario pane. The app will display the cumulative number of events received as well as a count of bytes received in each event.

To halt the transmission of asynchronous events, press the **Unregister From Event** button.

Note that whenever the user leaves this scenario, the app will unregister the event.

**Receiving an input report**

When you choose the **Input and Output Reports** scenario the app displays two **Get Input Report** buttons. The first button lets you retrieve a numeric value issued by a device input report. The second button lets you retrieve a boolean value issued by a device input report.

**Issuing an output report**

When you choose the **Input and Output Reports** scenario the app displays two **Get Output Report** buttons. The first button lets you send a numeric value via an output report. The second button lets you send a boolean value via output report.

**Disconnecting from a SuperMUTT device.**

When you press the **Stop Device Watcher** button, the app executes code that disconnects from the SuperMUTT device.

Also, whenever the app is suspended, it executes code that disconnects from the SuperMUTT device.

## Operating system requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

To run this sample after building it, press F5 (run with debugging enabled) or Ctrl+F5 (run without debugging enabled) from Visual Studio. (Or select the corresponding options from the Debug menu.)

## Device Driver Requirements

The sample runs over the inbox USB and HID device-drivers that ship in Windows. These device drivers are installed when you first attach the SuperMUTT device.

Note that the **Windows.Devices.HumanInterfaceDevice** API does not support custom, or filter, drivers.

## Understanding the app manifest

The app manifest is an XML document that contains the info the system needs to deploy, display, or update a Windows Store app. This info includes package identity, package dependencies, required capabilities, visual elements, and extensibility points. Every app package must include one package manifest.

A Windows Store app that accesses a HID device must include specific **DeviceCapability** data in the **Capabilities** node of its manifest. This data identifies the device and its purpose (or function). Note that some devices may have multiple functions.

The **Device Id** element corresponds to the device identifier. This element may specify a combination **Vendor Id** (vid) and **Product Id** (pid); or, it may specify a generic string ("any"). In addition, the **Device ID** may contain an optional provider string of "usb" or "bluetooth".

The **Function Type** element specifies the device function. This element contains one or more HID usage values. These values consist of a **Usage Page** and an optional **Usage Id**, each of which are 16-bit hexadecimal values.

**The sample DeviceCapabilities**

In the following vendor-defined usage data, the device is identified by the **Vendor Id** and the **Product Id** combination.

``` {.syntax xml:space="preserve"}
  <Capabilities>
     <!-- There are multiple ways declare a device -->
    <DeviceCapability Name="humaninterfacedevice">
      <!--SuperMutt Device-->
      <Device Id="vidpid:045E 0610">
        <Function Type="usage:FFAA 0001" />
      </Device>
    </DeviceCapability>
  </Capabilities>
```

## Configuring the SuperMUTT firmware

The following steps allow you to configure the SuperMUTT device to run with your sample.

-   Download and install the MUTT Software Package.
-   Open a command prompt and run the MuttUtil tool included in the package. Use this tool to update the firmware. (Note that you need to repeat the following command twice.)

    ``` {.syntax xml:space="preserve"}
    MuttUtil.exe -forceupdatefirmware
    ```

-   Use the MuttUtil tool to set the HID device-mode.

    ``` {.syntax xml:space="preserve"}
    MuttUtil.exe -setwinrthid
    ```

Once you've tackled these steps, the SuperMUTT device is configured to work with the sample app.

If needed, the you can revert the SuperMUTT from HID device-mode back to its default configuration by running the following MuttUtil tool command.

``` {.syntax xml:space="preserve"}
MuttUtil.exe -hidtodefault
```

## Limitations

**Designed for Peripherals**

This API is designed primarily for accessing peripheral devices. That said, you can use it to access PC internal devices. However, access to these devices is limited to a privileged app that is created by the OEM.

**Not designed for Control Panel apps**

This API is intended for Windows Store apps. Because there is no way for these apps to save settings outside of application scope you should not use it to write Control Panel apps.

**Host Controller Limitations**

The SuperMUTT device is compatible with EHCI host controllers. However, it does not currently support interrupts with XHCI host controllers.

