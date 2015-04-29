Custom Serial Device Sample
===========================

This sample demonstrates the use of the Windows.Devices.SerialCommunication WinRT APIs to communicate with a Serial device.

**Note**  This sample was created using one of the universal app templates available in Visual Studio. It shows how its solution is structured so it can run on both Windows 10 and Windows Phone 10.

This sample allows the user to configure and communicate with a Serial device. You can choose one of four scenarios:

-   Connect/Disconnect using Device Selection list
-   Configure the Serial device
-   Communicate with the Serial device
-   Register for Events on the Serial device

**Connect/Disconnect using Device Selection list**

When the application starts, a list of available Serial devices matching the search criterion specified by the application is displayer along with options to Connect/Disconnect to/from a selected device.

**Configure the Serial device**

This scenario demonstrates the use of various Get/Set property APIs in order to query for/alter Serial device properties such as Baud Rate, Stop Bits etc.

**Communicate with the Serial device**

This scenario demonstrates the use of Input and Output streams on the SerialDevice object in order to communicate with the Serial device.

**Register for Events on the Serial device**

This scenario demonstrates the use of event notification APIs provided by Windows.Devices.SerialCommunication for **Pin Changed** and **Error Received** event types.

Operating system requirements
-----------------------------

Client

Windows 10

Server

Windows 10

Phone

Windows 10

Build the sample
----------------

1.  Start Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2.  Go to the directory to which you unzipped the sample. Then go to the subdirectory containing the sample in the language you desire - either C++ or C\#. Double-click the Visual Studio 2015 Solution (.sln) file.
3.  Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

Run the sample
--------------

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

**Deploying the sample**

1.  Select either the Windows or Windows Phone project version of the sample.
2.  Select **Build** \> **Deploy Solution**.

**Deploying and running the sample**

1.  Right-click either the Windows or Windows Phone project version of the sample in **Solution Explorer** and select **Set as StartUp Project**.
2.  To debug the sample and then run it, press F5 or select **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or select**Debug** \> **Start Without Debugging**.

