# Relative Inclinometer sensor sample

This sample demonstrates use of the Windows.Devices.Sensors.Inclinometer API for the Relative Inclinometer sensor.

This sample allows the user to view the angles of incline about the X-, Y-, and Z-axes for a 3-axis relative inclinometer. (The incline about the X-axis corresponds to the pitch value; the incline about the Y-axis correspond to the roll value; and the incline about the Z-axis corresponds to the yaw value.) You can choose one of three scenarios:

-   Relative inclinometer data events
-   Polling relative inclinometer readings

### Relative Inclinometer Data Events

When you choose the Enable button for the **Data Events** option, the app will begin streaming relative inclinometer readings in real time.

### Polling Relative Inclinometer Readings

When you choose the Enable button for the **Polling** option, the app will retrieve the current sensor readings.

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
