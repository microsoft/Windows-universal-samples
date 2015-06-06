# Accelerometer sensor sample

This sample shows how to use the [**Accelerometer**](http://msdn.microsoft.com/library/windows/apps/br225687) API.

This sample allows the user to view the acceleration forces along the X-, Y-, and Z-axes for a 3-axis accelerometer. You can choose one of four scenarios:

-   Accelerometer data events
-   Accelerometer shake events
-   Poll accelerometer readings
-   Accelerometer orientation changed
-   Accelerometer data events batching

### Acclerometer Data Events

When you choose the **Enable** button for the **Data Events** option, the app begins streaming accelerometer readings in real time.

### Accelerometer Shake Events

When you choose the **Enable** button for the **Shake Events** option, the app displays the cumulative number of shake events each time an event occurs. (The app first increments the event count and then renders the most recent value.)

### Poll Accelerometer Readings

When you choose the **Enable** button for the **Polling** option, the app will retrieve the sensor readings at a fixed interval.

### Accelerometer Orientation Changed

When you choose the **Enable** button for the **OrientationChange** option, the app will display both raw sensor readings, as well as sensor readings that align with the current display orientation.

### Accelerometer data events batching**

When you choose the **Enable** button for the **Data Events** option, the app begins streaming accelerometer readings in batches.

## Related topics

[**Accelerometer.GetCurrentReading method**](http://msdn.microsoft.com/library/windows/apps/br225699)

[**Accelerometer.ReadingChanged event handler**](http://msdn.microsoft.com/library/windows/apps/br225702)

[Quickstart: Responding to user movement with the accelerometer](http://msdn.microsoft.com/library/windows/apps/hh465265)

[Windows.Devices.Sensors namespace](http://go.microsoft.com/fwlink/p/?linkid=241981)

## System requirements

**Client:** Windows 10 Technical Preview

**Server:** Windows 10 Technical Preview

**Phone:**  Windows 10 Technical Preview

## Build the sample

1. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2.  Go to the directory to which you unzipped the sample. Then go to the subdirectory containing the sample in the language you desire - either C++ or C\#. Double-click the Visual Studio 2015 Solution (.sln) file.
3.  Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 

