<!---
  category: DevicesSensorsAndPower
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620580
--->

# Orientation sensor sample

Shows how to use the [Windows.Devices.Sensors.OrientationSensor](http://msdn.microsoft.com/library/windows/apps/br206371) API.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample allows the user to view the rotation matrix and Quaternion values that reflect the current device orientation.
After choosing the sensor reporting type and optimization goal,
you can choose one of these scenarios:

-   Orientation sensor data events
-   Poll orientation sensor readings
-   Sensor calibration

**Orientation sensor data events**

When you click the **Enable** button for the **Data Events** option, the app begins streaming sensor readings in real time.

**Poll orientation sensor readings**

When you click the **Enable** button for the **Polling** option, the app will retrieve the current sensor readings.

**Sensor calibration**

Allows the user to simulate sensor accuracy and demonstrates usage of the calibration bar.

Related topics
--------------

[Accelerometer sample](../Accelerometer)  
[Windows.Devices.Sensors namespace](http://msdn.microsoft.com/library/windows/apps/br206408)  

Operating system requirements
-----------------------------

**Client:** Windows 10 build 14295

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10 build 14295

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

