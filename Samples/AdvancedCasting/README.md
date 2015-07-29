<!---
  category: AudioVideoAndCamera
--->
Advanced Casting Sample
-----------

This sample shows how to use the APIs in the **Windows.Media.Casting** and **Windows.Media.DialProtocol** namespaces.  It also illustrates how to use the **Windows.UI.ViewManagement.ProjectionManager** and **Windows.Devices.Enumeration.DevicePicker** APIs to render media on a remote device.  It covers sending media to various devices- Miracast, DLNA, DIAL, and Bluetooth.  For an intro to casting, see the *Basic Media Casting Sample*.

**Scenario 1: Media Element Casting 101:**   
Press the *Cast* button next to the progress bar in the video element.  Select the device you'd like to cast to.

This is an example of the built in casting that comes with the media element transport controls.  This will enable casting to Miracast, DLNA, and Bluetooth devices.

**Scenario 2: Casting APIs and a Custom Cast Button:**  
Press the *Cast* button next to the progress bar in the video element.  Select the device you'd like to cast to.

This is very similar to the first scenario, however, in this case, the **Windows.Media.Casting** APIs are used manually to create a custom cast button that's then included in the media transport controls.  

**Scenario 3: DIAL Sender Universal Windows App**  
For this scenario you'll need a device that supports Dial.  You can set the application name and arguments in the fields provided and then use the cast button in the transport controls to launch the app on the remote device.

In this scenario, the **Windows.Media.DialProtocol** APIs are illustrated.

**Scenario 4: DIAL Receiver Windows Universal App** 
This scenario is used to illustrate how a developer would write a universal app that supports being launched by DIAL. This app can be launched with the APIs used in Scenario 3.  Currently this scenario is only valid for select Xbox configurations.

**Scenario 5: Multi-View Media Application**
Again, the cast button is used here to send a video to a second screen.  In this case, however the **Windows.UI.ViewManagement.ProjectionManager** API is used.  This allows the developer to customize their second screen experience.  This API works with wired monitors and Miracast devices.

**Scenario 6: Combine Casting Methods**
This scenario brings all the prior scenarios together and shows how to use them all at the same time in order to reach the widest set of devices.  **Windows.Devices.Enumeration.DevicePicker** is used to build a picker to show all the different devices in one place.

Related topics
--------------

[Windows.Media.Casting namespace](https://msdn.microsoft.com/en-us/library/windows/apps/windows.media.casting.aspx)
[Windows.Media.DialProtocol namespace](https://msdn.microsoft.com/en-us/library/windows/apps/windows.media.dialprotocol.aspx)
[Windows.UI.ViewManagement.ProjectionManager API](https://msdn.microsoft.com/en-us/library/windows/apps/windows.ui.viewmanagement.projectionmanager.aspx)
[Windows.Devices.Enumeration.DevicePicker API](https://msdn.microsoft.com/en-us/library/windows/apps/windows.devices.enumeration.devicepicker.aspx)

System requirements
-----------------------------

Client:
Windows 10  
Windows Phone 10  

Build the sample
----------------

1.  Start Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2.  Go to the directory to which you unzipped the sample. Then go to the subdirectory containing the sample in the language you desire - either C++, C\#, or JavaScript. Double-click the Visual Studio 2015 Solution (.sln) file.
3.  Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

Run the sample
--------------

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

**Deploying the sample**
1.  Select **Build** \> **Deploy Solution**.

**Deploying and running the sample**
1.  To debug the sample and then run it, press F5 or select **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or select**Debug** \> **Start Without Debugging**.

