Custom Sensor Sample
====================

This sample demonstrates the use of the Windows.Devices.Sensors.Custom sensor API.

**Note**  This sample was created using one of the universal app templates available in Visual Studio. It shows how its solution is structured so it can run on both Windows 10 and Windows Phone 10.

This sample allows the user to view the CO2 level for custom sensor based CO2 sensor. You can choose one of two scenarios:

-   Custom sensor data events
-   Polling custom sensor readings

**Custom Sensor Data Events**

When you click on the **Enable** button for the **Data Events** option, the app will begin streaming Custom Sensor readings in real time.

**Polling Custom Sensor Readings**

When you click on the **Get CO2 level** button for the **Polling** option, the app will retrieve the current sensor readings.

**Note**  For Windows 8 app samples, download the [Windows 8 app samples pack](http://go.microsoft.com/fwlink/p/?LinkId=301698). The samples in the Windows 8 app samples pack will build and run only on Microsoft Visual Studio 2012.

Operating system requirements
-----------------------------

Client

Windows 10

Server

None supported

Phone

Windows Phone 10

Build the sample
----------------

1.  Start Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2.  Go to the directory to which you unzipped the sample. Then go to the subdirectory containing the sample in the language you desire - either C++ or C\#. Double-click the Visual Studio 2015 Solution (.sln) file.
3.  Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

Run the sample
--------------

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

**Deploying the sample**

1.  Select **Build** \> **Deploy Solution**.

**Deploying and running the sample**

1.  To debug the sample and then run it, press F5 or select **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or select**Debug** \> **Start Without Debugging**.

