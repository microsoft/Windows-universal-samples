CameraHdr Sample
----------------

This sample applies an end-to-end approach to demonstrate how to write a video recording camera application using the Windows.Media.Capture API in conjunction with orientation sensors to cover the functions that most camera apps will require. It will also use the Windows.Media.Core.SceneAnalysisEffect API to get information about the preview scene and give a recommendation on how beneficial an HDR capture would be. In addition, it will show a simple way to use the Windows.Media.Capture.AdvancedCapture API, which enables High Dynamic Range (HDR) captures, included in Windows. This sample is based on the CameraStarterKit.

Specifically, this sample will cover how to:

1. **Manage the MediaCapture object** throughout the lifecycle of the app and through navigation events.
2. **Acquire a camera located on a specific side of the device**. In this case, the sample attempts to get the rear camera.
3. **Start and stop the preview** to a UI element, including mirroring for front-facing cameras.
4. **Take a regular picture** to a file, taking into account the orientation of the device.
5. **Manage the Scene Analysis effect**, including creation, activation/deactivation of the HighDynamicRangeAnalyzer, registering for the SceneAnalyzed event, and cleanup. The effect will be used throughout the lifetime of the app, and the output will be represented in the UI as a bar that fills up according to the information provided in the event raised by the effect.
6. **Configure the AdvancedPhotoControl** to capture HDR images, create an instance of the AdvancedCapture, and register for the AllAllPhotosCaptured event, which signals that the camera is ready to capture again, and for the OptionalReferencePhotoCaptured, which will be raised only on devices that support delivering a reference image alongside the fused image, and carries the reference image in the payload.
7. **Take an HDR picture** to a file, taking into account the orientation of the device.
8. **Handle rotation events** for both, the device moving in space and the page orientation changing on the screen. Also apply any necessary corrections to the preview stream rotation.
9. **Handle MediaCapture Failed event** to clean up the MediaCapture instance when an error occurs.

This sample also implements a custom UI to better simulate the experience that a camera application would provide, so any messages intended for the developer are printed to the debug console.

## Related topics

**Samples**

[CameraStarterKit](https://github.com/Microsoft/Windows-universal-samples/tree/master/camerastarterkit)

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

**Client:** Windows 10 Technical Preview - Known Issue with HDR requires build greater than 10142

**Server:** Windows 10 Technical Preview - Known Issue with HDR requires build greater than 10142

**Phone:**  Windows 10 Technical Preview - Known Issue with HDR requires build greater than 10142

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


