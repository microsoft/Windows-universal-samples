Syndication sample
==================

This sample shows you how to retrieve feeds from a web service using classes in the [**Windows.Web.Syndication**](http://msdn.microsoft.com/library/windows/apps/br243632) namespace in your Windows Runtime app.This sample is provided in the JavaScript, C\#, and C++ programming languages.

**Note**  This sample was created using one of the universal app templates available in Visual Studio. It shows how its solution is structured so it can run on both Windows 8.1 and Windows Phone 8.1. For more info about how to build apps that target Windows and Windows Phone with Visual Studio, see [Build apps that target Windows and Windows Phone 8.1 by using Visual Studio](http://msdn.microsoft.com/library/windows/apps/dn609832).

This sample demonstrates the following features:

-   Use the [**SyndicationClient**](http://msdn.microsoft.com/library/windows/apps/br243456) class and class members to retrieve a web feed.
-   Use the [**SyndicationItem**](http://msdn.microsoft.com/library/windows/apps/br243533) class and class members to display items in a web feed.

**Note**  Use of this sample requires Internet or intranet access in order to retrieve feeds.

For a sample that shows how to manage syndicated content on a web service in a Windows Store app, download the [AtomPub sample](http://go.microsoft.com/fwlink/p/?linkid=245061).

**Network capabilities**

This sample requires that network capabilities be set in the *Package.appxmanifest* file to allow the app to access the network at runtime. These capabilities can be set in the app manifest using Microsoft Visual Studio.

To build the Windows version of the sample:

-   **Internet (Client):** The sample can use the Internet for client operations (outbound-initiated access). This allows the app to retrieve feeds from s server on the Internet. This is represented by the **Capability name = "internetClient"** tag in the app manifest.

-   If the sample is used to connect to a server located on a home or work network (a local intranet), the **Private Networks (Client & Server)** capability must be set. This is represented by the **Capability name = "privateNetworkClientServer"** tag in the app manifest.

To build the Windows Phone version of the sample:

-   **Internet (Client & Server):** This sample has complete access to the network for both client operations (outbound-initiated access) and server operations (inbound-initiated access). This allows the app to retrieve feeds from a server on the Internet or on a local intranet. This is represented by the **Capability name = "internetClientServer"** tag in the app manifest.

    **Note**  On Windows Phone, there is only one network capability which enables all network access for the app.

For more information on network capabilities, see [How to set network capabilities](http://msdn.microsoft.com/library/windows/apps/hh770532).

To obtain an evaluation copy of Windows 8.1, go to [Windows 8.1](http://go.microsoft.com/fwlink/p/?linkid=301696).

To obtain an evaluation copy of Microsoft Visual Studio 2013 Update 2, go to [Microsoft Visual Studio 2013](http://go.microsoft.com/fwlink/p/?linkid=301697).

**Note**  For Windows 8 app samples, download the [Windows 8 app samples pack](http://go.microsoft.com/fwlink/p/?LinkId=301698). The samples in the Windows 8 app samples pack will build and run only on Microsoft Visual Studio 2012.

Related topics
--------------

**Other - C\#/VB/C++ and XAML**

[Accessing and managing syndicated content (XAML)](http://msdn.microsoft.com/library/windows/apps/hh452981)

[Adding support for networking (XAML)](http://msdn.microsoft.com/library/windows/apps/hh452751)

[How to set network capabilities (XAML)](http://msdn.microsoft.com/library/windows/apps/jj835817)

[How to access a web feed (XAML)](http://msdn.microsoft.com/library/windows/apps/hh452994)

**Other - JavaScript and HTML**

[Accessing and managing syndicated content (HTML)](http://msdn.microsoft.com/library/windows/apps/hh452973)

[Adding support for networking (HTML)](http://msdn.microsoft.com/library/windows/apps/hh452752)

[How to set network capabilities (HTML)](http://msdn.microsoft.com/library/windows/apps/hh770532)

[How to access a web feed (HTML)](http://msdn.microsoft.com/library/windows/apps/hh700374)

**Reference**

[**SyndicationClient**](http://msdn.microsoft.com/library/windows/apps/br243456)

[**SyndicationItem**](http://msdn.microsoft.com/library/windows/apps/br243533)

[**Windows.Web.AtomPub**](http://msdn.microsoft.com/library/windows/apps/br210609)

[**Windows.Web.Syndication**](http://msdn.microsoft.com/library/windows/apps/br243632)

**Samples**

[AtomPub sample](http://go.microsoft.com/fwlink/p/?linkid=245061)

[Windows 8 app samples](http://go.microsoft.com/fwlink/p/?LinkID=227694)

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
2.  Go to the directory to which you unzipped the sample. Go to the directory named for the sample, and double-click the Visual Studio 2013 Update 2 Solution (.sln) file.
3.  Follow the steps for the version of the sample you want:
    -   To build the Windows version of the sample:

        1.  Select **Syndication.Windows** in **Solution Explorer**.
        2.  Press Ctrl+Shift+B, or use **Build** \> **Build Solution**, or use **Build** \> **Build Syndication.Windows**.
    -   To build the Windows Phone version of the sample:

        1.  Select **Syndication.WindowsPhone** in **Solution Explorer**.
        2.  Press Ctrl+Shift+B or use **Build** \> **Build Solution** or use **Build** \> **Build Syndication.WindowsPhone**.

Run the sample
--------------

**Note**  To successfully run this sample requires Internet or intranet access.

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

**Deploying the sample**

-   To deploy the built Windows version of the sample:

    1.  Select **Syndication.Windows** in **Solution Explorer**.
    2.  Use **Build** \> **Deploy Solution** or **Build** \> **Deploy Syndication.Windows**.
-   To deploy the built Windows Phone version of the sample:

    1.  Select **Syndication.WindowsPhone** in **Solution Explorer**.
    2.  Use **Build** \> **Deploy Solution** or **Build** \> **Deploy Syndication.WindowsPhone**.

**Deploying and running the sample**

-   To deploy and run the Windows version of the sample:

    1.  Right-click **Syndication.Windows** in **Solution Explorer** and select **Set as StartUp Project**.
    2.  To debug the sample and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.
-   To deploy and run the Windows Phone version of the sample:

    1.  Right-click **Syndication.WindowsPhone** in **Solution Explorer** and select **Set as StartUp Project**.
    2.  To debug the sample and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.

