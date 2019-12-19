---
page_type: sample
languages:
- csharp
products:
- windows
- windows-uwp
urlFragment: HotspotAuthentication
extendedZipContent:
- path: SharedContent
  target: SharedContent
- path: LICENSE
  target: LICENSE
description: "Shows how to provision and register a hotspot profile and perform either WISPr or custom authentication."
---

<!---
  category: NetworkingAndWebServices
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=2019043
--->

# Hotspot authentication sample

Shows how to provision and register a hotspot profile and perform either WISPr or custom authentication.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> You can download this sample as a standalone ZIP file
> [from docs.microsoft.com](https://docs.microsoft.com/samples/microsoft/windows-universal-samples/hotspotauthentication/),
> or you can download the entire collection as a single
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample demonstrates the following operations:

- Provision a hotspot profile and register the corresponding background task.
- Authenticate as a foreground app with default WISPr authentication or custom authentication.
- Authenticate in a background task

The background task requires that the profile be provisioned and registered.
The task is triggered once the Wi-Fi is connected to the hotspot as a part of post Wi-Fi connection.
When a device initially connects, the hotspot usually has a limited connection,
and post-connection authentication is done by a background task to enable the full Internet connection.

In order to invoke the hotspot authentication flow (background task),
the access point being connected to must provide a captive portal response
with WISPr support claimed in the XML blob being returned to the client.

Authentication can be completed in one of two ways.
- Issuing WISPr credentials using IssueCredentialsAsync API, which uses the default WISPr implementation.
- Performing a custom WISPr authentication using the information obtained through HotspotAuthenticationContext.TryGetAuthenticationContext. In this case, you must call  HotspotAuthenticationContext.SkipAuthentication API to skip the default WISPr authentication process once the custom authentication is complete.

## Related topics

### Conceptual

[Windows 8 hotspot authentication sample](https://code.msdn.microsoft.com/windowsapps/Wi-Fi-hotspot-authenticatio-943569eb)

### Reference

[Mobile Broadband on the Windows Hardware Dev Center](https://docs.microsoft.com/en-us/windows-hardware/drivers/mobilebroadband/index)

[HotspotAuthenticationContext](https://docs.microsoft.com/en-us/uwp/api/Windows.Networking.NetworkOperators.HotspotAuthenticationContext)

[HotspotAuthenticationEventDetails ](https://docs.microsoft.com/en-us/uwp/api/Windows.Networking.NetworkOperators.HotspotAuthenticationEventDetails)

[IBackgroundTask](https://docs.microsoft.com/en-us/uwp/api/Windows.ApplicationModel.Background.IBackgroundTask)

[IBackgroundTaskInstance](https://docs.microsoft.com/en-us/uwp/api/Windows.ApplicationModel.Background.IBackgroundTaskInstance) 

## System requirements

The system must have a Wi-Fi adapter for this sample to be meaningful.

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for the programming language. Double-click the Visual Studio Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The Wi-Fi hotspot authentication sample has following requirements:

Windows must trigger the application when authenticating with a certain hotspot. This is achieved by provisioning a WLAN profile with a corresponding configuration.

1. The app must declare background task and corresponding entry point must match with the actual background task.
2. The app must declare networkConnectionManagerProvisioning capability to provision hotspot profile xml. The capability belongs to restrictedcapabilities namespace and needs to be manually added to the Package.appxmanifest file since Visaul Studio UI doesn't support it.
3. The provisioning XML built into this application must be modified to match the hotspot's SSID.

The steps for completing these requirements and running the sample are described below:

1. Modify the Provisioning Metadata XML file to match your hotspot by opening ProvisioningData.xml and adjusting the following fields: 

* CarrierProvisioning\Global\CarrierID: If writing a mobile broadband operator app, use the same GUID that you specified as the Service Number in the metadata authoring wizard. 
If writing a hotspot-only app, generate a new GUID with Visual Studio. On the Tools menu, click Create GUID. Click Copy to transfer the new GUID to the Clipboard.

* CarrierProvisioning\Global\SubscriberID: If writing a mobile broadband operator app, use the International Mobile Subscriber Identity (IMSI) of the mobile broadband SIM. 
If writing a hotspot-only app, use any unique identifier appropriate to your service, such as a username or account number.

* CarrierProvisioning\WLANProfiles\WLANProfile\name: Name of your service or test Access Point (AP). 

* CarrierProvisioning\WLANProfiles\WLANProfile\SSIDConfig\SSID\name: Configured SSID of your test hotspot. 

* CarrierProvisioning\WLANProfiles\WLANProfile\MSM\security\HotspotProfile\ExtAuth\ExtensionId:
Package family name of the application. To retrieve this name, open package.appmanifest in Visual Studio. Click on the Packaging tab. Copy the Package Family Name.

2. Provide appropriate credentials for your test AP 

* If the test AP uses fixed credentials: Modify HotspotAuthenticationTask\ConfigStore.cs to return a valid username and password for your test AP. 
* If the test AP uses dynamic credentials: In HotspotAuthenticationTask\BackgroundTask.cs, replace the reference to ConfigStore with your own business logic to generate/retrieve appropriate credentials for the network. 

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

**Deploying the sample**
1.  Select **Build** \> **Deploy Solution**.

**Deploying and running the sample**
1.  To debug the sample and then run it, press F5 or select **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or select **Debug** \> **Start Without Debugging**.


