Pedometer sensor sample
===========================

This sample shows how to use the  Windows.Devices.Sensors.Pedometer API.

**Note**  This sample was created using one of the universal uap templates available in Visual Studio. It shows how its solution is structured so it can run on both Windows 10 and Windows Phone 10.

This sample allows the user to view the default pedometer events and history available on the system. You can choose one of four scenarios:

-   Events
-   History
-   Current steps count

**Events**

It demonstrates getting the default pedometer asynchronously.
When you click on the 'Register ReadingChanged' button, it registers to the 'ReadingChanged' event on the default pedometer and displays pedometer readings as they are notified.

**History**

This demonstrates usage of History APIs for pedometer.
When you click on the 'Get History' button, pedometer history for the requested timespan will be displayed. Two ways to choose the timespan is provided, which demonstrate the two overloaded ways of getting history.

**Current steps count**

When you click the 'Get steps count' button, it displays the last known step counts. Since 'ReadingChanged' may not be fired when there is no activity, this relies on History to get the last known step counts.

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

