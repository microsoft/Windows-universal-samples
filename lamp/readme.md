Lamp Sample
-----------

This sample demonstrates Windows.Devices.Lights.Lamp API usage. The Windows.Devices.Lights.Lamp API allows applications that utilize the camera flash
independently of the capture device. This allows for more control over the flash device and operates at a lower power and CPU consumption since the flash
is no longer seen as a slave peripheral controlled by the capture device.  

Specifically, this sample covers the three most common scenarios for utilizing the flash:

**Scenario 1: Acquiring a Lamp instance.** There are two methods to acquire the lamp device:

1. When you choose the "Get Lamp Instance from Device Information", this demonstrates acquiring Lamp via querying DeviceInformation with class selection string for Lamp. In this scenario, you have more control over picking what specific lamp device you'd like to use. For the demonstration, we grab the back lamp device which is most common lamp location.
2. When you choose the "Get Default Lamp Instance", this is the more simplified method is to just get the default lamp device. 

**Scenario 2: Enable Lamp and Settings adjustment.** This scenario demonstrates determining the capabilities of the lamp, and adjusting those capabilities.

1. When you choose "Adjust Brightness and turn on Lamp", this queries the lamp device for Brightness capability, and demonstrates adjusting the lamp brightness setting to 0.5 or 50%. 
2. When you choose "Adjust Color", this queries the lamp device for Color capability, and demonstrates adjusting the color lamp color to Blue.

**Scenario 3: Lamp Device Change Events.** This sample demonstrates registering for lamp AvailablityChanged event. Lamp devices can be acquired by another 
applications that utilize camera at any time when lamp is in use. When this happens, an AvailiablityChanged event will fire. In this scenario we demonstrate registering
for the AvailabillityChanged event and handling that event with a message to the user. In this scenario we acquire the default lamp device page turns on, and provide a
toggle to turn the lamp on so you can see the AvailablityChanged event fire by launching another app (Camera app will suffice) and turning on the flash. 

1. When you choose "Register for AvailaiblityChanged Event", this registers for the event.
2. When you choose "Unregister for AvailaiblityChanged Event", this unregisters for the event.

## Related topics

[Windows.Media.Devices namespace](https://msdn.microsoft.com/en-us/library/windows/apps/windows.media.devices.aspx)

## System requirements

**Hardware:** Camera with flash device

**Client:** Windows 10 Technical Preview

**Server:** Windows 10 Technical Preview

**Phone:**  Windows 10 Technical Preview

## Build the sample

1. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2. Go to the directory to which you unzipped the sample. Then go to the subdirectory containing the sample in the language you desire - either C++, C#, or JavaScript. Double-click the Visual Studio 2015 Solution (.sln) file. 
3. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**. 

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 
