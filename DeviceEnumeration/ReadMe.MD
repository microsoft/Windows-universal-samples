Device enumeration sample
=========================

This sample shows how to use the device enumeration API to find available devices and look for device information.

**Note**  This sample was created using one of the universal app templates available in Visual Studio. It shows how its solution is structured so it can run on both Windows 8.1 and Windows Phone 8.1. For more info about how to build apps that target Windows and Windows Phone with Visual Studio, see [Build apps that target Windows and Windows Phone 8.1 by using Visual Studio](http://msdn.microsoft.com/library/windows/apps/dn609832).

The sample presents two scenarios:

1.  In the first scenario, the Device Enumeration API is used to look for specific device interfaces. A device interface is the programmatic entry point for a device. The sample lets you enter the GUID of a device interface class, or choose a common device interface class (Printers, Webcams, or Portable Devices) from a drop-down box, and then click **Enumerate** to find device interfaces that belong to the specified device interface class.
2.  In the second scenario, the Device Enumeration API is used to enumerate device containers. A device container is an object which represents the visible aspects of a device hardware product, such as manufacturer or model name. These properties are localized and are displayed to identify a particular device.

To obtain an evaluation copy of Windows 8.1, go to [Windows 8.1](http://go.microsoft.com/fwlink/p/?linkid=301696).

To obtain an evaluation copy of Microsoft Visual Studio 2013 Update 2, go to [Microsoft Visual Studio 2013](http://go.microsoft.com/fwlink/p/?linkid=301697).

**Note**  For Windows 8 app samples, download the [Windows 8 app samples pack](http://go.microsoft.com/fwlink/p/?LinkId=301698). The samples in the Windows 8 app samples pack will build and run only on Microsoft Visual Studio 2012.

Related topics
--------------

[Device Enumeration](http://msdn.microsoft.com/library/windows/apps/hh464977)

[Windows 8 app samples](http://go.microsoft.com/fwlink/p/?LinkID=227694)

Related technologies
--------------------

[Device Enumeration](http://msdn.microsoft.com/library/windows/apps/hh464977)

Operating system requirements
-----------------------------

Client

Windows 8.1

Server

Windows Server 2012 R2

Phone

Windows Phone 8.1

Build the sample
----------------

1.  Start Visual Studio 2013 Update 2 and select **File** \> **Open** \> **Project/Solution**.
2.  Go to the directory to which you unzipped the sample. Then go to the subdirectory containing the sample in the language you desire - either C++, C\#, JavaScript, or Visual Basic. Double-click the Visual Studio 2013 Update 2 Solution (.sln) file.
3.  Select either the Windows or Windows Phone project version of the sample. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

Run the sample
--------------

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

**Deploying the sample**

1.  Select either the Windows or Windows Phone project version of the sample.
2.  Select **Build** \> **Deploy Solution**.

**Deploying and running the sample**

1.  Right-click either the Windows or Windows Phone project version of the sample in **Solution Explorer** and select **Set as StartUp Project**.
2.  To debug the sample and then run it, press F5 or select **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or select**Debug** \> **Start Without Debugging**.

