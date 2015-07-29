This sample shows how mail clients can retrieve device information and work with supplied Exchange Active Sync (EAS) policies.

Windows Store apps can configure their mail clients to stay compliant with the given EAS policies. To bring the local computer compliant, you must request the user's consent before you proceed through the use of a consent UI. Then you can configure the exchange account.

This sample requires some experience with WMI programming.

**Note**  This sample was created using one of the universal app templates available in Visual Studio. It shows how its solution is structured so it can run on both Windows 10 and Windows Phone 10.

You can choose one of three scenarios: 
- Retrieving device information
- Checking if device policies are compliant with supplied EAS policies
- Applying supplied EAS policies.

Related Topics
--------------
[Windows.Security.ExchangeActiveSyncProvisioning](http://msdn.microsoft.com/library/windows/apps/hh701506)

Operating system requirements
-----------------------------

Windows 10

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
