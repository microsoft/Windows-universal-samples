<!---
  category: AudioVideoAndCamera
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=624252
--->

# Camera resolution sample

Shows how to change the resolution of a capture device with the
[GetMediaStreamProperties](https://msdn.microsoft.com/library/windows/apps/windows.media.devices.videodevicecontroller.getmediastreamproperties.aspx)
and [SetMediaStreamPropertiesAsync](https://msdn.microsoft.com/library/windows/apps/hh700895.aspx) methods. 

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample covers the three common scenarios for changing the resolution:

**Scenario 1**: Changing the resolution of the preview.

1. Press the *Initialize Camera* button to populate the combo box with the resolutions
returned by the
[GetAvailableMediaStreamProperties](https://msdn.microsoft.com/library/windows/apps/windows.media.devices.videodevicecontroller.getmediastreamproperties.aspx)
method.
2. When an option from the combo box is selected, the changes will be reflected in the preview.

**Scenario 2**: Changing the resolution of the preview and photo streams seperately.

1. Press the *Initialize Camera* button to populate the combo boxes with the resolutions
returned by the
[GetAvailableMediaStreamProperties](https://msdn.microsoft.com/library/windows/apps/windows.media.devices.videodevicecontroller.getmediastreamproperties.aspx)
method.
2. When an option from the preview stream combo box is selected, the changes will be reflected in the preview.
3. When an option from the photo stream combo box is selected, the changes will be reflected in any photos taken with the photo button.
   * Some devices may share streams, which will result in changes made to one stream also being applied to other streams.

**Scenario 3**: Keeping the aspect ratios the same.

1. Press the *Initialize Camera* button to populate the combo boxes with the resolutions
returned by the
[GetAvailableMediaStreamProperties](https://msdn.microsoft.com/library/windows/apps/windows.media.devices.videodevicecontroller.getmediastreamproperties.aspx)
method.
Video settings will be based on the aspect ratio of the default resolution.
2. When an option from the preview stream combo box is selected, the changes will be reflected in the preview.
If the aspect ratio has changed, the video combo box will
also change to show only formats that match the aspect ratio.
3. When an option from the video stream combo box is selected,
the changes will be reflected in any videos taken with the video button.
   * Some devices may not support multiple streams and changes will also be reflected in the preview.
   * Although this sample shows maintaining the aspect ratio only between the preview and video streams,
     the same should be done with the photo stream when capturing photos.

## Related topics

### Samples

[Basic camera app sample](/Samples/CameraStarterKit)  
[Media capture using capture device](https://code.msdn.microsoft.com/windowsapps/Media-Capture-Sample-adf87622)  

### Reference

[Windows.Media.Devices namespace](https://msdn.microsoft.com/library/windows/apps/windows.media.devices.aspx)  
[Windows.Media.Capture.MediaCapture namespace](https://msdn.microsoft.com/library/windows/apps/windows.media.devices.aspx)  
[Windows.Media.Capture.MediaCaptureInitializationSettings constructor](https://msdn.microsoft.com/library/windows/apps/windows.media.capture.mediacaptureinitializationsettings.mediacaptureinitializationsettings.aspx)  
[Windows.Media.Capture.MediaCaptureInitializationSettings.VideoDeviceId property](https://msdn.microsoft.com/library/windows/apps/windows.media.capture.mediacaptureinitializationsettings.videodeviceid.aspx)  
[Windows.Devices.Enumeration.DeviceInformation class](https://msdn.microsoft.com/library/windows/apps/windows.devices.enumeration.deviceinformation)  
[Windows.Graphics.Imaging.BitmapDecoder class](https://msdn.microsoft.com/library/windows/apps/windows.graphics.imaging.bitmapdecoder.aspx)  
[Windows.Graphics.Imaging.BitmapEncoder class](https://msdn.microsoft.com/library/windows/apps/windows.graphics.imaging.bitmapencoder.aspx)  
[Capture photos and video with MediaCapture](https://msdn.microsoft.com/library/windows/apps/mt243896)  

### System requirements

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

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or select Debug > Start Without Debugging. 
