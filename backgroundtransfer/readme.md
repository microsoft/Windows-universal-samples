Background Transfer sample
==========================

This sample shows how to use the Background Transfer API to download and upload files in the background in Windows Runtime apps.

**Note**  This sample was created using one of the universal app templates available in Visual Studio. It shows how its solution is structured so it can run on both Windows 8.1 and Windows Phone 8.1. For more info about how to build apps that target Windows and Windows Phone with Visual Studio, see [Build apps that target Windows and Windows Phone 8.1 by using Visual Studio](http://msdn.microsoft.com/library/windows/apps/dn609832).

Classes in the [**Windows.Networking.BackgroundTransfer**](http://msdn.microsoft.com/library/windows/apps/br207242) namespace provide a power-friendly, cost-aware, and flexible API for transferring files in the background. This sample shows how to download and upload files using the Background Transfer API.

For the download scenario, the sample first uses methods on [**BackgroundDownloader**](http://msdn.microsoft.com/library/windows/apps/br207126) class to enumerate any downloads that were going on in the background while the app was closed. An app should enumerate these downloads when it gets started so it can attach a progress handler to these downloads to track progress and prevent stale downloads. Then other methods on the **BackgroundDownloader** and related classes are used to start new downloads to the local Pictures Library. The sample also shows how to pause downloads and change the priority of a download.

For the upload scenario, the sample first uses methods on [**BackgroundUploader**](http://msdn.microsoft.com/library/windows/apps/br207140) class to enumerate any uploads that were going on in the background while the app was closed. An app should enumerate these uploads when it gets started so it can attach a progress handler to these uploads to track progress and prevent stale uploads. Then other methods on the **BackgroundUploader** and related classes are used to start new uploads. The sample also shows how to set a content header and use a multipart upload.

The sample also shows how to configure and use toast and tile notifications to inform the user when all transfers succeed or when at least one transfer fails.

**Note**  Background transfer is primarily designed for long-term transfer operations for resources like video, music, and large images. For short-term operations involving transfers of smaller resources (i.e. a few KB), the HTTP APIs are recommended. [**HttpClient**](http://msdn.microsoft.com/library/windows/apps/dn298639) is preferred and can be used in all languages supported by Windows Runtime apps. [**XHR**](http://msdn.microsoft.com/library/windows/apps/br229787) can be used in JavaScript. [IXHR2](http://msdn.microsoft.com/library/windows/apps/hh770550) can be used in C++.

This sample requires the following capabilities:

-   **Internet (Client & Server)** - Needed to send requests to download or upload files to HTTP or FTP servers on the Internet.
-   **Private Networks (Client & Server)** - Needed to send requests to download or upload files to HTTP or FTP servers on a home or work intranet.
-   **Pictures Library** - Needed to downloads files to the Pictures library.

**Note**  The Windows version of this sample by default requires network access using the loopback interface.

**Network capabilities**

This sample requires that network capabilities be set in the *Package.appxmanifest* file to allow the app to access the network at runtime. These capabilities can be set in the app manifest using Microsoft Visual Studio.

To build the Windows version of the sample, set the following network capabilities:

-   **Internet (Client)**: The sample has outbound-initiated network access to the Internet. This allows the app to send requests to download or upload files to HTTP or FTP servers on the Internet. This is represented by the **Capability name = "internetClient"** tag in the app manifest.

    **Private Networks (Client & Server)**: The sample has inbound and outbound network access on a home or work network (a local intranet). This allows the app to send requests to download or upload files to HTTP or FTP servers on a local home or work Intranet. The **Private Networks (Client & Server)** capability is represented by the **Capability name = "privateNetworkClientServer"** tag in the app manifest.

To build the Windows Phone version of the sample, set the following network capabilities:

-   **Internet (Client & Server):** This sample has complete access to the network for both client operations (outbound-initiated access) and server operations (inbound-initiated access). This allows the app to send requests to download or upload files to HTTP or FTP servers on the Internet or on a local intranet. This is represented by the **Capability name = "internetClientServer"** tag in the app manifest.

    **Note**  On Windows Phone, there is only one network capability which enables all network access for the app.

For more information on network capabilities, see [How to set network capabilities](http://msdn.microsoft.com/library/windows/apps/hh770532).

**Note**  Network communications using an IP loopback address cannot normally be used for interprocess communication between a Windows Runtime app and a different process (a different Windows Runtime app or a desktop app) because this is restricted by network isolation. Network communication using an IP loopback address is allowed within the same process for communication purposes in a Windows Runtime app. For more information, see [How to set network capabilities](http://msdn.microsoft.com/library/windows/apps/hh770532).

**Note**  Background Transfer is only enabled on Windows if at least one of the networking capabilities is set: **Internet (Client)**, **Internet (Client & Server)**, or **Private Networks (Client & Server)**.

This sample is currently provided in the JavaScript, C\#, VB, and C++ programming languages.

To obtain an evaluation copy of Windows 8.1, go to [Windows 8.1](http://go.microsoft.com/fwlink/p/?linkid=301696).

To obtain an evaluation copy of Microsoft Visual Studio 2013 Update 2, go to [Microsoft Visual Studio 2013](http://go.microsoft.com/fwlink/p/?linkid=301697).

**Note**  For Windows 8 app samples, download the [Windows 8 app samples pack](http://go.microsoft.com/fwlink/p/?LinkId=301698). The samples in the Windows 8 app samples pack will build and run only on Microsoft Visual Studio 2012.

Related topics
--------------

**Other resources**

[Adding support for networking](http://msdn.microsoft.com/library/windows/apps/hh452752)

[How to configure network isolation capabilities](http://msdn.microsoft.com/library/windows/apps/hh770532)

[Quickstart: Downloading a file](http://msdn.microsoft.com/library/windows/apps/hh700370)

[Quickstart: Uploading a file](http://msdn.microsoft.com/library/windows/apps/hh700372)

[Transferring a file from a network resource](http://msdn.microsoft.com/library/windows/apps/hh761434)

**Reference**

[**BackgroundDownloader**](http://msdn.microsoft.com/library/windows/apps/br207126)

[**BackgroundUploader**](http://msdn.microsoft.com/library/windows/apps/br207140)

[**HttpClient**](http://msdn.microsoft.com/library/windows/apps/dn298639)

[IXHR2](http://msdn.microsoft.com/library/windows/apps/hh770550)

[**Windows.Networking.BackgroundTransfer**](http://msdn.microsoft.com/library/windows/apps/br207242)

[**Windows.Storage**](http://msdn.microsoft.com/library/windows/apps/br227346)

[**XHR**](http://msdn.microsoft.com/library/windows/apps/br229787)

**Samples**

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

        1.  Select **BackgroundTransfer.Windows** in **Solution Explorer**.
        2.  Press Ctrl+Shift+B, or use **Build** \> **Build Solution**, or use **Build** \> **Build BackgroundTransfer.Windows**.
    -   To build the Windows Phone version of the sample:

        1.  Select **BackgroundTransfer.WindowsPhone** in **Solution Explorer**.
        2.  Press Ctrl+Shift+B or use **Build** \> **Build Solution** or use **Build** \> **Build BackgroundTransfer.WindowsPhone**.

Run the sample
--------------

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

**Deploying the sample**

-   To deploy the built Windows version of the sample:

    1.  Select **BackgroundTransfer.Windows** in **Solution Explorer**.
    2.  Use **Build** \> **Deploy Solution** or **Build** \> **Deploy BackgroundTransfer.Windows**.
-   To deploy the built Windows Phone version of the sample:

    1.  Select **BackgroundTransfer.WindowsPhone** in **Solution Explorer**.
    2.  Use **Build** \> **Deploy Solution** or **Build** \> **Deploy BackgroundTransfer.WindowsPhone**.

**Deploying and running the Windows version of the sample**

This sample requires that a web server is available for the app to access for uploading and downloading files. The web server must be started before the app is run. The web server must also have a *BackgroundTransferSample* path available for uploads and downloads. The sample includes a PowerShell script that will install IIS on the local computer, create the *BackgroundTransferSample* folder on the server, copy files to this folder, create firewall rules to allow access, and enable IIS.

The easiest way to run the sample is to use the provided web server scripts. Browse to the *Server* folder in your sample folder to setup and start the web server. There are two options possible.

-   Start PowerShell elevated (Run as administrator) and run the following command:

    **.\\SetupServer.ps1**

    Note that you may also need to change script execution policy.

-   Start an elevated Command Prompt (Run as administrator) and run following command:

    **PowerShell.exe -ExecutionPolicy Unrestricted -File SetupServer.ps1**

When the web server is not needed anymore, please browse to the *Server* folder in you sample folder and run one of the following:

-   Start PowerShell elevated (Run as administrator) and run the following command:

    **.\\RemoveServer.ps1**

    Note that you may also need to change script execution policy.

-   Start an elevated Command Prompt (Run as administrator) and run following command:

    **PowerShell.exe -ExecutionPolicy Unrestricted -File RemoveServer.ps1**

The sample can run using any web server, not only the one provided with the sample. If IIS is used on a different computer, then the previous scripts can be used with minor changes.

-   Copy the *Server* folder to the device where IIS will be run.
-   Run the above scripts to install IIS, create the *BackgroundTransferSample* folder on the server, copy files to this folder, and enable IIS.

The sample must also be updated when run against a non-localhost web server. To configure the sample for use with IIS on a different device:

-   The hostname of the server to connect to needs to be updated. This can be handled in two ways. The **AddressField** element in the HTML or XAML files can be edited so that "localhost" is replaced by the hostname or IP address of the web server. Alternately when the app is run, enter the hostname or IP address of the web server instead of the "localhost" value in the **Remote address** textbox.

**Note**  IIS is not available on ARM builds nor on Windows Phone. Instead, set up the web server on a separate 64-bit or 32-bit computer and follow the steps for using the sample against a non-localhost web server.

**Note**  When used with the supplied scripts, this Windows Runtime app sample communicates with another process (IIS server which is a desktop app) on the same machine over loopback for demonstration purposes only. A Windows Runtime app that communicates over loopback to another process that represents a Windows Runtime app or a desktop app is not allowed and such apps will not pass Store validation. For more information, see [How to enable loopback and troubleshoot network isolation](http://msdn.microsoft.com/library/windows/apps/hh780593).

However if a server different than IIS is used, then this requires some special configuration of the server to create the *BackgroundTransferSample* folder.

Copy the *Server\\webSite* directory to the *BackgroundTransferSample* folder on the web server and configure the server to allow download and upload requests.

To configure the sample for use with a web server different than IIS not using localhost:

The remote server address and local filename fields should be updated. This can be handled in two ways. The **serverAddressField** and **fileNameField** elements in the HTML or XAML files can be edited so that the remote server address and filename are replaced by a server address and filename for the non-IIS server. Alternately when the app is run, enter the remote address and local filename to access on the web server instead of the default values in the **Remote address** and **Local file name** fields displayed.

-   To deploy and run the Windows version of the sample:

    1.  Right-click **BackgroundTransfer.Windows** in **Solution Explorer** and select **Set as StartUp Project**.
    2.  To debug the sample and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.

**Deploying and running the Windows Phone version of the sample**

IIS is not available on Windows Phone. For the app to access a web server, there are two options:

-   The easiest way to run the sample is to use the provided web server scripts on a separate 64-bit or 32-bit device that can run IIS. Follow the instructions for deploying and running the Windows version of the sample using IIS on a different device.
-   Use a web server different than IIS on a separate device. Follow the instructions for deploying and running the Windows version of the sample using a non-IIS web server.

-   To deploy and run the Windows Phone version of the sample:

    1.  Right-click **BackgroundTransfer.WindowsPhone** in **Solution Explorer** and select **Set as StartUp Project**.
    2.  To debug the sample and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.

