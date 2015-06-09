# AllJoyn Client Experiences Sample

This sample demonstrates how to create an AllJoyn Universal app using Code Generation with Introspection XML and Windows.Devices.AllJoyn.

Specifically, this sample covers:

**Scenario 1**
-   Creating and launching a Secure AllJoyn Client.
-   Implementing method call, property and signal from generated code.

**Scenario 2**
-   Discovering and connecting to an Onboardee's SoftAP.
-   Implementing method calls to get an Onboardee's network scan, configuring an Onboardee with WiFi credentials etc.

**Note** 
Scenario 2 - For onboarding physical devices, there is a known issue with the WiFi scan/connect APIs which prevent joining networks as private. This will be fixed for RTM. In the meanwhile developers can
-   Either manually change the network type to private after joining the Onboarding access point, by going to Settings->Network and Internet->Wi-Fi->Advanced options->Change the “Find devices and content” switch to On position.
-   Or use the Onboarding producer on the same subnet to emulate the Onboarding experience.

For more info on how to change network to private or how to make sure the machines are on the same subnet, please refer to [Troubleshooting AllJoyn blog](http://channel9.msdn.com/Blogs/Internet-of-Things-Blog/Troubleshooting-AllJoyn-with-Windows-10-Insider-Preview-Builds)

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10, go to [Windows 10](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples

[AllJoyn Server Experiences](https://github.com/Microsoft/Windows-universal-samples/tree/master/AllJoynServerExperiences)
[AllJoyn Client Experiences](https://github.com/Microsoft/Windows-universal-samples/tree/master/AllJoynClientExperiences)

The AllSeen Alliance has samples in their [Windows SDK](https://allseenalliance.org/developers/download)

### Reference

[MSDN Reference](https://msdn.microsoft.com/en-us/library/windows/apps/windows.devices.alljoyn.aspx)
[AllJoyn Reference] (https://allseenalliance.org/developers/develop/api-reference)

## System requirements

ARM, ARM64, x86, or amd64 system

**Client:** Windows 10 Technical Preview

**Server:** Windows 10 Technical Preview

**Phone:**  Windows 10 Technical Preview

## Build the sample

1. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2. Go to the directory to which you unzipped the sample. Then go to the subdirectory containing the sample in the C# language. Double-click the Visual Studio 2015 Solution (.sln) file. 
3. Set the active solution configuration and platform to the desired values under **Build** \> **Configuration Manager**.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**. 

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 