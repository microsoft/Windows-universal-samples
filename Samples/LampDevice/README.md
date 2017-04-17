<!---
  category: DevicesSensorsAndPower 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620558
--->

# Lamp device sample

Shows how to use the [Windows.Devices.Lights.Lamp](https://msdn.microsoft.com/library/windows/apps/windows.devices.lights.aspx) 
API to enable apps to use the camera flash
independently of the capture device. The Windows.Devices.Lights.Lamp API allows for more control 
over the flash device and consumes less power and CPU
resources because the overhead of running capture device is avoided.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample covers the three most common scenarios for utilizing the flash:

Scenario 1: Acquiring a Lamp instance. There are two methods for acquiring the lamp device:
     1)  When you choose the "Get Lamp Instance from Device Information" button, the app demonstrates acquiring Lamp by calling DeviceInformation.FindAllAsync()
     and passing in the device selection string for the lamp device. Using this method you have more control over picking a specific lamp device. In this example
     we get the back lamp device, which is most common lamp location.
     2) When you choose the "Get Default Lamp Instance" button, the app demonstrates a simplified method for getting the default lamp device with Lamp.GetDefaultAsync() call. 

Scenario 2: Enable Lamp and Settings adjustment. This scenario demonstrates determining the capabilities of the lamp and adjusting those capabilities.
    1) When you choose the "Adjust Brightness and turn on Lamp" button, the app queries the lamp device for current brightness level. Then the app demonstrates adjusting the lamp
    brightness level to 0.5, or 50%. 
    2) When you choose "Adjust Color" button, the app queries the lamp device for Color capability. If the capability is supported the app then adjusts the lamp color to Blue.

Scenario 3: Lamp Device Change Events. This sample demonstrates registering for Lamp.AvailablityChanged event. If another application on the device starts using the camera, control of the lamp is taken away from your app.
When this happens, an AvailablityChanged event is raised. In this scenario we demonstrate registering for the Lamp.AvailablityChanged event and handling that event by updating the UI when the event is raised. We acquire
the default lamp device when page loads and provide a toggle to turn the lamp on and off. To raise the AvailablityChanged event for your app, launch another app that uses the camera,. Launching the built-in camera app is recommend.
    1) When you choose "Register for AvailablityChanged Event" button, the app registers for the AvailablityChanged event
    2) When you choose "Unregister for AvailablityChanged Event", the app unregisters for the AvailablityChanged event.

Related topics
--------------

[Windows.Media.Devices namespace] (https://msdn.microsoft.com/library/windows/apps/windows.media.devices.aspx)

Windows 8 and 8.1 method for turning on flash (requires starting a video recording session)
[Windows.Media.Devices.VideoDeviceController.TorchControl] (https://msdn.microsoft.com/library/windows/apps/windows.media.devices.videodevicecontroller.torchcontrol.aspx)


System requirements
-----------------------------
Independent Flash Device
Client
Windows 10
Windows Phone 10

Build the sample
----------------

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

Run the sample
--------------

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

**Deploying the sample**
1.  Select **Build** \> **Deploy Solution**.

**Deploying and running the sample**
1.  To debug the sample and then run it, press F5 or select **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or select**Debug** \> **Start Without Debugging**.