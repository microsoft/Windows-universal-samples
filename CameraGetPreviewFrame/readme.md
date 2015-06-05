GetPreviewFrame Sample
----------------------

This sample applies an end-to-end approach to demonstrate how to write a camera application using the Windows.Media.Capture API in conjunction with the DisplayInformation API to acquire preview frames from the camera stream for further processing.

Specifically, this sample will cover how to:

1. **Manage the MediaCapture object** throughout the lifecycle of the app and through navigation events.
2. **Acquire a camera located on a specific side of the device**. In this case, the sample attempts to get the rear camera.
3. **Start and stop the preview** to a UI element, including mirroring for front-facing cameras.
4. **Get a preview frame** using both overloads for the method.
  1. Get it as a **Direct3DSurface** and display information about the frame: Width, Height, Format.
  2. Get it as a **SoftwareBitmap**, display information about the frame: Width, Height, Format, and optionally display it on the screen and/or save it to disk.
5. **Handle page rotation events** to apply any necessary corrections to the preview stream rotation, ensuring acquired preview frames have the correct orientation and aspect ratio.
6. **Handle MediaCapture Failed event** to clean up the MediaCapture instance when an error occurs.

Due to the custom UI that this sample implements, any messages intended for the developer are printed to the debug console.

## Related topics

**Samples**

[How to rotate captured video](https://msdn.microsoft.com/en-us/library/windows/apps/hh868174.aspx)

[How to preview video from a webcam](https://msdn.microsoft.com/en-us/library/windows/apps/xaml/hh868171.aspx)

**Reference**

[Windows.Media.Capture.MediaCapture namespace](https://msdn.microsoft.com/en-us/library/windows/apps/windows.media.devices.aspx)

[Windows.Media.Capture.MediaCaptureInitializationSettings constructor](https://msdn.microsoft.com/en-us/library/windows/apps/windows.media.capture.mediacaptureinitializationsettings.mediacaptureinitializationsettings.aspx) 

[Windows.Media.Capture.MediaCaptureInitilizationSettings.VideoDeviceId property](https://msdn.microsoft.com/en-us/library/windows/apps/windows.media.capture.mediacaptureinitializationsettings.videodeviceid.aspx)

[Windows.Devices.Enumeration namespace](https://msdn.microsoft.com/en-us/library/windows/apps/windows.devices.enumeration.aspx)

[Windows.Devices.Enumeration.DeviceInformation class](https://msdn.microsoft.com/en-us/library/windows/apps/windows.devices.enumeration.deviceinformation)

[Windows.Devices.Sensors.SimpleOrientationSensor class](https://msdn.microsoft.com/en-us/library/windows/apps/windows.devices.sensors.simpleorientationsensor.aspx)

[Windows.Graphics.Display.DisplayInformation class](https://msdn.microsoft.com/en-us/library/windows/apps/windows.graphics.display.displayinformation.aspx)

[Windows.UI.Xaml.Media.Imaging.WriteableBitmap class](https://msdn.microsoft.com/en-us/library/windows/apps/windows.ui.xaml.media.imaging.writeablebitmap.aspx)

## System requirements

**Hardware:** Camera

**Client:** Windows 10 Technical Preview

**Server:** Windows 10 Technical Preview

**Phone:**  Windows 10 Technical Preview

## Build the sample

1.  Start Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2.  Go to the directory to which you unzipped the sample. Then go to the subdirectory containing the sample in the language you desire - either C++, C\#, or JavaScript. Double-click the Visual Studio 2015 Solution (.sln) file.
3.  Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

**Deploying the sample:**

1.  Select **Build** \> **Deploy Solution**.

**Deploying and running the sample:**

1.  To debug the sample and then run it, press F5 or select **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or select **Debug** \> **Start Without Debugging**.


