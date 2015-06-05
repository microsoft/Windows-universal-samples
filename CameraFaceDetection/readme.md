FaceDetection Sample
--------------------

This sample applies an end-to-end approach to demonstrate how to write a camera application using the Windows.Media.Capture API in conjunction with orientation sensors to cover the functions that most camera apps will require. In addition, it will show a simple way to use the Face Detection effect included in Windows.

Specifically, this sample will cover how to:

1. **Manage the MediaCapture object** throughout the lifecycle of the app and through navigation events.
2. **Acquire a camera located on a specific side of the device**. In this case, the sample attempts to get the rear camera.
3. **Start and stop the preview** to a UI element, including mirroring for front-facing cameras.
4. **Take a picture** to a file, and disable the video capture button if the app is running on a device that doesn't support concurrent capturing of photos and video.
5. **Record a video** to a file, and disable the photo capture button if the app is running on a device that doesn't support concurrent capturing of photos and video.
6. **Handle rotation events** for both, the device moving in space and the page orientation changing on the screen. Also apply any necessary corrections to the preview stream rotation and to captured photos and videos.
7. **Handle MediaCapture RecordLimitationExceeded and Failed events** to be notified that video recording needs to be stopped due to a video being too long, or clean up the MediaCapture instance when an error occurs.
8. **Manage the Face Detection effect**, including creation, configuration, activation/deactivation, registering for events, and cleanup.
9. **Render face bounding boxes** as an overlay on the camera preview, taking mirroring and rotation into account.

This sample also implements a custom UI to better simulate the experience that a camera application would provide, so any messages intended for the developer are printed to the debug console.

## Related topics

**Samples**

[How to rotate captured video](https://msdn.microsoft.com/en-us/library/windows/apps/hh868174.aspx)

[How to preview video from a webcam](https://msdn.microsoft.com/en-us/library/windows/apps/xaml/hh868171.aspx)

[Media capture using capture device](https://code.msdn.microsoft.com/windowsapps/Media-Capture-Sample-adf87622)

**Reference**

[Windows.Media.Capture.MediaCapture namespace](https://msdn.microsoft.com/en-us/library/windows/apps/windows.media.devices.aspx)

[Windows.Media.Capture.MediaCaptureInitializationSettings constructor]https://msdn.microsoft.com/en-us/library/windows/apps/windows.media.capture.mediacaptureinitializationsettings.mediacaptureinitializationsettings.aspx) 

[Windows.Media.Capture.MediaCaptureInitilizationSettings.VideoDeviceId property](https://msdn.microsoft.com/en-us/library/windows/apps/windows.media.capture.mediacaptureinitializationsettings.videodeviceid.aspx)

[Windows.Devices.Enumeration namespace](https://msdn.microsoft.com/en-us/library/windows/apps/windows.devices.enumeration.aspx)

[Windows.Devices.Enumeration.DeviceInformation class](https://msdn.microsoft.com/en-us/library/windows/apps/windows.devices.enumeration.deviceinformation)

[Windows.Devices.Sensors.SimpleOrientationSensor class](https://msdn.microsoft.com/en-us/library/windows/apps/windows.devices.sensors.simpleorientationsensor.aspx)

[Windows.Graphics.Display.DisplayInformation class](https://msdn.microsoft.com/en-us/library/windows/apps/windows.graphics.display.displayinformation.aspx)

[Windows.Phone.UI.Input.HardwareButtons.CameraPressed event](https://msdn.microsoft.com/en-us/library/windows/apps/windows.phone.ui.input.hardwarebuttons.camerapressed.aspx)

[Windows.Graphics.Imaging.BitmapDecoder class](https://msdn.microsoft.com/en-us/library/windows/apps/windows.graphics.imaging.bitmapdecoder.aspx)

[Windows.Graphics.Imaging.BitmapEncoder class](https://msdn.microsoft.com/en-us/library/windows/apps/windows.graphics.imaging.bitmapencoder.aspx)

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


