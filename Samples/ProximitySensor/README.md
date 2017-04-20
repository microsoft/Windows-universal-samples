<!---
  category: DevicesSensorsAndPower
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620588
--->

# Proximity sensor sample

Shows how to use the [Windows.Devices.Sensors.ProximitySensor](https://msdn.microsoft.com/library/windows/apps/windows.devices.sensors.proximitysensor.aspx) API.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample allows the user to view the proximity detection reading as a boolean value.
It includes the following scenarios:

-   Proximity sensor data events
-   Polling proximity sensor readings
-   Proximity Monitoring to Turn the Display On or Off
-   Background Proximity Sensor

**Proximity Sensor Data Events**

When you choose the Enable button for the **Data Events** option, the app will begin displaying proximity sensor detection readings in real time.

**Polling Proximity Sensor Readings**

When you choose the Get Data button for the **Polling** option, the app will retrieve the current sensor readings.

**Proximity Monitoring to Turn the Display On or Off**

When you choose the Enable button for the **Display On/Off** option, the app will request the system to monitor the proximity sensor detection state to automatically turn the display on or off.

**Background Proximity Sensor**

This demonstrates using a Proximity sensor's state as a background trigger.
When you click the 'Register Task' button, an available proximity sensor will be used to register for notifications in the change of proximity.
When a change in the proximity is detected by the sensor, the registered background task gets fired. Proximity Sensor readings are then retrieved from the trigger to update the scenario UI.

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

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 
