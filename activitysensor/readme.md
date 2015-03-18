Activity Detection Sensor Sample
================================

This sample demonstrates the use of the Windows.Devices.Sensors.ActivitySensor API.

**Note**  This sample was created using one of the universal app templates available in Visual Studio. It shows how its solution is structured so it can run on both Windows 10 and Windows Phone 10.

This sample allows the user to interact with the activity detection functionality on the system. You can choose one of four scenarios:

-   Current activity
-   History
-   Events
-   Background activity

**Current activity**

Gets the default activity sensor and displays the current activity.

**History**

Gets the activity history from at most 1 day ago. Displays the first entry, last entry, and count of entries.

**Events**

Subscribes to reading and status changed events and displays the updated activity reading and sensor status.

**Background activity**

Registers a background task for activity changes. The background task runs whenever the most likely activity changes to/from any of the subscribed activities.

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

