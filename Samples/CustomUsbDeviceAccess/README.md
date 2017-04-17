<!---
  category: DevicesSensorsAndPower
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620530
--->

# Custom USB device sample

Shows how to communicate with a USB device by using the [Windows.Devices.Usb](http://msdn.microsoft.com/library/windows/apps/dn278466) namespace. 

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

The sample can communicate with these devices:

-   The OSR USB FX2 learning kit. You can get the learning kit from [OSR Online](http://www.osronline.com/).
-   The SuperMUTT device. You can purchase the device from [JJG Technologies](http://jjgtechnologies.com/mutt.htm). You must update the device firmware before using it (discussed later).

The sample demonstrates these key scenarios:

-   [How to connect to a USB device](http://msdn.microsoft.com/library/windows/apps/dn303343)
-   [How to send a USB control transfer](http://msdn.microsoft.com/library/windows/apps/dn303347)
-   [How to send a USB interrupt transfer](http://msdn.microsoft.com/library/windows/apps/dn303348)
-   [How to send a USB bulk transfer](http://msdn.microsoft.com/library/windows/apps/dn303346)
-   [How to get USB descriptors](http://msdn.microsoft.com/library/windows/apps/dn303344)
-   [How to select a USB interface setting](http://msdn.microsoft.com/library/windows/apps/dn303345)
-   How to handle app suspension and resume events

For step-by-step instructions about implementing USB features in a Windows Store app, see [Talking to USB devices, start to finish](http://msdn.microsoft.com/library/windows/apps/dn312121) and [Writing a Windows store app for a USB device](http://msdn.microsoft.com/library/windows/apps/dn303355).

## Code Structure

* **App**

  Invoked when the sample app is activated. The OnActivated implementation causes the app to get launched when the device is connected to the system. When the app is activated, the user is shown information about which device launched the app.

* **DeviceListEntry**

  This class stores [DeviceInformation](http://msdn.microsoft.com/library/windows/apps/br225393) objects associated with each device, dynamically detected by the [DeviceWatcher](http://msdn.microsoft.com/library/windows/apps/br225446) object. This class is used by the UI to display device-specific information. For example, the UI uses this class to get the device interface path so that the user can identify the device and use it for data transfers.

* **EventHandlerForDevice**

  This class implements all event handlers required by the sample app. There two types of events that the app handles:

  -  App events:

    Windows can suspend, resume, or terminate an app as a result of a change in system state or user action. Windows notifies the app about the state through **Suspending** and **Resuming** events. By implementing handlers, the app can save app or session data before the state changes. In the sample, the app stops all [DeviceWatcher](http://msdn.microsoft.com/library/windows/apps/br225446) objects on suspension. Otherwise, **DeviceWatcher** continues to raise events even when the app is suspended. On resume, the app starts them.

    In order to serialize events, on suspension, this class calls a registered callback (provided by a scenario). That allows the app to respond to app suspension before the app closes the device.

    Additionally, when an app suspends, the API releases the [UsbDevice](http://msdn.microsoft.com/library/windows/apps/dn263883) object. To avoid using a stale reference on resume, the sample explicitly closes the device in its **Suspending** event handler. In the **Resuming** handler, the sample opens a handle to the device and obtains a new reference to the **UsbDevice** object.

  -  [DeviceWatcher](http://msdn.microsoft.com/library/windows/apps/br225446) events:

    The sample registers for these [DeviceWatcher](http://msdn.microsoft.com/library/windows/apps/br225446) events: [Added](http://msdn.microsoft.com/library/windows/apps/br225450), [Removed](http://msdn.microsoft.com/library/windows/apps/br225453), and [EnumerationCompleted](http://msdn.microsoft.com/library/windows/apps/br225451). Detects when the current connected device is removed or reconnected. It then opens the device and obtains a [UsbDevice](http://msdn.microsoft.com/library/windows/apps/dn263883) reference and releases that reference on disconnect.

    The app calls the when EventHandlerForDevice:OpenAsync to open the device. The class also handles the surprise remove event. When the physical device is device unplugged, the [UsbDevice](http://msdn.microsoft.com/library/windows/apps/dn263883) is released. On reconnect, EventHandlerForDevice opens the device again.

  The class also holds a reference to the currently connected device for which the class handles events. The app can get a reference to the EventHandlerForDevice singleton and can access the [UsbDevice](http://msdn.microsoft.com/library/windows/apps/dn263883) by using EventHandlerForDevice::Current-\>Device.

* **Scenario1\_DeviceConnect**

  This class implements methods that use the [DeviceWatcher](http://msdn.microsoft.com/library/windows/apps/br225446) object to dynamically detect devices (see EventHandlerForDevice). That reference is used through the sample to interact with the device. When finished, the sample closes the device by releasing that reference.

  When the class is instantiated, the app creates [DeviceWatcher](http://msdn.microsoft.com/library/windows/apps/br225446) objects for SuperMUTT and OSRFX2 devices. Those types of devices are detected by their respective **DeviceWatcher** objects.

* **Scenario2\_ControlTransfer**

  This class implements methods that demonstrate how to initialize USB setup packets and send control transfers to the device. The sample changes the blink pattern (SuperMUTT) and seven segment display (OSRFX2) by using control transfers.

* **Scenario3\_InterruptPipes**

  This class implements methods that write to a USB interrupt pipe (only applies to SuperMUTT)and read from another interrupt pipe as data arrives in the pipe. To read data, the sample registers an event handler that gets invoked every time there is data to read. In the sample's event handler, it reads the received data and displays number of times it read data.

* **Scenario4\_BulkPipes**

  The class implements methods to read and write data to and from USB bulk pipes. The sample demonstrates how to start and cancel asynchronous operations for bulk transfers. After the transfer is complete, the sample displays the number of bytes read or written. Data is read and written by using [DataReader](http://msdn.microsoft.com/library/windows/apps/br208119) and [DataWriter](http://msdn.microsoft.com/library/windows/apps/br208154) objects.

* **Scenario5\_UsbDescriptors**

  The class implements methods that display all USB descriptors associated with the current device. The sample displays the device descriptor, configuration descriptor, interface descriptors for all settings and their endpoint descriptors.

  It also shows how to obtain the entire USB configuration descriptor as a raw descriptor and use the [DataReader](http://msdn.microsoft.com/library/windows/apps/br208119) to read bytes from the descriptor.

* **Scenario6\_InterfaceSettings**

  This class implements methods that demonstrate how to enumerate USB alternate settings for an interface, get an active setting and, select an alternate setting. Selecting an alternate setting applies to the SuperMUTT device. Retrieving number of interface settings (drop down list) and getting active interface setting applies to OSRFX2 and SuperMUTT devices.

* **Scenario7\_SyncDevice**

  This class implements methods that syncs data with the device in a background task. Because only one process can access [UsbDevice](http://msdn.microsoft.com/library/windows/apps/dn263883) object, the app closes the previous reference obtained in Scenario1\_DeviceConnect and opens the device again after the task is complete. Sync completion is is notified though a registered callback registered by the app.

  For more information, see [this topic](http://go.microsoft.com/fwlink/p/?linkid=306314) about sync-ing with a device.

## App manifest package

The sample adds the [DeviceCapability](http://msdn.microsoft.com/library/windows/apps/br211430) element in the Package.appxmanifest file. Device information includes the device's vendor/product Ids and device class information. For OSRFX2 and SuperMUTT devices, the device class is specified by indicating the device class code and a string for the code.

``` {.syntax xml:space="preserve"}
  <Capabilities>
      <!--When the device's classId is FF * *, there is a predefined name for the class. You can use the name instead of the class id. 
          There are also other predefined names that correspond to a classId.-->
      <DeviceCapability Name="usb">
          <!--OSRFX2 Device-->
          <Device Id="vidpid:0547 1002">
              <Function Type="classId:ff * *"/>
              <!--<Function Type="name:vendorSpecific"/>-->
          </Device>
          <!--SuperMutt Device-->
          <Device Id="vidpid:045E 0611">
              <!--<Function Type="classId:ff * *"/>-->
              <Function Type="name:vendorSpecific"/>
          </Device>
      </DeviceCapability>
  </Capabilities>
```

## Related technologies

* [Windows.Devices.Usb](http://msdn.microsoft.com/library/windows/apps/dn278466)

  Provides Windows Runtime classes and enumerations that a Windows store app can use to communicate with an external USB device that uses WinUSB (Winusb.sys) as the device driver.

* [Windows.Devices.Enumeration](http://msdn.microsoft.com/library/windows/apps/br225459)

  Provides Windows Runtime classes for device discovery and notifications about devices for apps that use them. [Windows.ApplicationModel.Background](http://msdn.microsoft.com/library/windows/apps/br224847)

* [Windows.ApplicationModel.Background](http://msdn.microsoft.com/library/windows/apps/br224847)

  Enables an app to schedule background tasks to run app code even when the app is suspended.

* [DeviceWatcher](http://msdn.microsoft.com/library/windows/apps/br225446)

  Enumerates devices dynamically, so that the app receives notifications if devices are added, removed, or changed after the initial enumeration is complete.

* [DataReader](http://msdn.microsoft.com/library/windows/apps/br208119)

  Reads data from an input stream. Used for reading data from a USB pipe.

* [DataWriter](http://msdn.microsoft.com/library/windows/apps/br208154)

  Writes data to an output stream. Used for writing data to a USB pipe.

## Operating system requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

## Driver requirements

The sample app communicates with the device through the Microsoft-provided kernel-mode driver, Winusb.sys. You must install it as the device driver.

Hardware manufacturers can specify Winusb.sys as the device driver in either of these two ways:

-   By providing a custom INF that references the Microsoft-provided Winusb.inf file. For more information, see [WinUSB (Winusb.sys) Installation](http://msdn.microsoft.com/library/windows/apps/ff540283).
-   By setting Microsoft operating system (OS) feature descriptors that report the compatible ID as "WINUSB". In this case, Windows matches the compatible ID with the driver and automatically loads Winusb.sys as the device driver. For more information, see [WinUSB Device](http://msdn.microsoft.com/library/windows/apps/hh450799).

When you connect your device, you might notice that Windows loads Winusb.sys automatically. Otherwise follow these instructions to load the driver:

1.  Open Device Manager and locate the device.
2.  Right-click the device and select **Update driver software...** from the context menu.
3.  In the wizard, select **Browse my computer for driver software**.
4.  Select **Let me pick from a list of device drivers on my computer**.
5.  From the list of device classes, select **Universal Serial Bus devices**.
6.  The wizard displays **WinUsb Device**. Select it to load the driver.

## If you are using the OSRFX2 device...

The preceding procedure does not add a device interface GUID for the app to access the device. You must add the GUID manually by following this procedure.

1.  Load the driver as described in the preceding procedure.
2.  Generate a device interface GUID for the OSRFX2 device, by using a tool such as guidgen.exe.
3.  Find the registry key for your OSRFX2 device under this key. In this example, VID and PID of the device is VID\_0547&PID\_1002:

    **HKEY\_LOCAL\_MACHINE\\SYSTEM\\CurrentControlSet\\Enum\\USB\\VID\_0547&PID\_1002**

4.  Under the **Device Parameters** key, add a String registry entry named **DeviceInterfaceGUID** or a Multi-String entry named **DeviceInterfaceGUIDs**. Set the value to the GUID you generated in step 2.
5.  Disconnect the device from the system and reconnect it to the same physical port.

    **Note**  If you change the physical port then you must repeat steps 1 through 4.

## If you are using the SuperMUTT device...

Windows automatically loads Winusb.sys as the device driver. Otherwise, manually install the driver by using the preceding instructions.

1.  Download and install the [MUTT Software Package](http://msdn.microsoft.com/library/windows/hardware/jj590752.aspx).
2.  Open a command prompt and run the MuttUtil tool included in the package. Use the tool to update the firmware:

    `MuttUtil.exe -forceupdatefirmware`

3.  By using the MuttUtil tool, change the device mode to WinRTUsbPersonality:

    `MuttUtil.exe -SetWinRTUsb`

    The SuperMUTT device when configured in WinRTUsbPersonality mode, exposes configuration, interfaces, and endpoints, that work with the sample.

## Customizing the sample for your device

You can extend this sample for your device by performing these tasks.

1.  Create a [DeviceWatcher](http://msdn.microsoft.com/library/windows/apps/br225446) object for your device in Scenario1\_DeviceConnect.
2.  Add information about your device in the [DeviceCapability](http://msdn.microsoft.com/library/windows/apps/br211430) in the Package.appxmanifest file. The information must be added under the **DeviceCapability** element with **Name** attribute set to "usb".

    If you specify the device class code, make sure that it is supported. For a list of supported device classes see [How to add USB device capabilities to the app manifest](https://msdn.microsoft.com/library/windows/apps/dn303351).

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

To run this sample after building it, press F5 (run with debugging enabled) or Ctrl+F5 (run without debugging enabled) from Visual Studio. (Or select the corresponding options from the Debug menu.)
