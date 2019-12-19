---
page_type: sample
languages:
- csharp
products:
- windows
- windows-uwp
urlFragment: PlayReady
extendedZipContent:
- path: SharedContent
  target: SharedContent
- path: LICENSE
  target: LICENSE
description: "Shows several common scenarios to playback protected content with PlayReady."
---

<!---
  category: PlayReady
  samplefwlink: https://aka.ms/playreadyuwp
--->

# PlayReady sample

Shows several common scenarios to playback protected content with PlayReady.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> You can download this sample as a standalone ZIP file
> [from docs.microsoft.com](https://docs.microsoft.com/samples/microsoft/windows-universal-samples/playready/),
> or you can download the entire collection as a single
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

***C# Scenarios***

**Scenario 1: Reactive License Request(C#):**    
This example shows the basics of configuring the MediaElement's MediaProtectionManager property to support PlayReady and reactively handle license service requests. In this configuration license service requests are triggered automatically by PlayReady when the DRM does not have a media license available.

**Scenario 2: Proactive License Request(C#):**    
This example shows how an application can actively manage DRM license requests and the individualization process. Individualization is a onetime configuration process that enables an application to able to make license requests for protected playback.

**Scenario 3: Hardware DRM(C#):**    
This example shows how to configure a device to use either software or hardware DRM. Hardware DRM provides higher security and performance whereas software based DRM provides better compatibility on older devices.

**Scenario 4: Secure Stop(C#):**    
Secure Stop enables a service to better manage concurrent streams for an account. The application can send a secure stop message from PlayReady that notifies a service that playback has stopped and a temporary playback license is no longer available. 

**XBox One:**
In order to use PlayReady DRM within a Universal Application on Xbox One, an additional <DeviceCapability> needs to be added to the app manifest. The capability will need to be added manually to the manifest as a setting is not yet available from within the App Manifest Designer. 

-	Right-click on the package.appxmanifest from Visual Studio and edit with the XML (text) Editor. 
-	Add the following <DeviceCapability> and save:

```xml
<Capabilities>
    ...
    <DeviceCapability Name="6a7e5907-885c-4bcb-b40a-073c067bd3d5" />
</Capabilities>
```

**Note** The Windows universal samples require Visual Studio to build and Windows 10 to execute.

To obtain information about Windows 10 development, go to the [Windows Dev Center](https://dev.windows.com)

To obtain information about Microsoft Visual Studio and the tools for developing Windows apps, go to [Visual Studio](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Reference

[Windows.Media.Protection namespace](https://msdn.microsoft.com/library/windows/apps/br207056.aspx)  
[Windows.Media.Protection.PlayReady namespace](https://msdn.microsoft.com/library/windows/apps/dn986454.aspx)  
[Windows.Media.Streaming.Adaptive namespace](https://msdn.microsoft.com/library/windows/apps/windows.media.streaming.adaptive.aspx)  

### Conceptual

[PlayReady DRM](https://msdn.microsoft.com/windows/uwp/audio-video-camera/playready-client-sdk)  
[Hardware DRM](https://msdn.microsoft.com/windows/uwp/audio-video-camera/hardware-drm)  
[Adaptive Streaming with PlayReady](https://msdn.microsoft.com/windows/uwp/audio-video-camera/adaptive-streaming-with-playready)  

### Related samples

* [AdaptiveStreaming](../AdaptiveStreaming)
* [PlayReady sample](/archived/PlayReady/) for JavaScript (archived)

## System requirements

* Windows 10 

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

**Note** Ensure that the CPU is selected that matches the machine running the sample. For example, if its Windows on x64 then x64 needs to be selected. If it does not match than the Protected Media Path will not work and playback will fail.

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or select Debug > Start Without Debugging. 
