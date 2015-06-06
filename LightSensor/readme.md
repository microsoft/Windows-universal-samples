# LightSensor sample

This sample shows how to use the [**Windows.Devices.Sensors.LightSensor**](http://msdn.microsoft.com/library/windows/apps/br225790) API.

This sample allows the user to view the ambient light reading as a LUX value. You can choose one of two scenarios:

-   LightSensor data events
-   Poll light sensor readings

### LightSensor Data Events

When you click the **Enable** button for the **Data Events** option, the app begins streaming light-sensor readings in real time.

### Poll LightSensor Readings

When you click the **Enable** button for the **Polling** option, the app will retrieve the sensor readings at a fixed interval.

## Related topics

[Windows.Devices.Sensors namespace](http://go.microsoft.com/fwlink/p/?linkid=241981)

## System requirements

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
