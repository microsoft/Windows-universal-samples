<!---
  category: AudioVideoAndCamera
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620629
--->

# Windows audio session (WASAPI) sample

Demonstrates how to do various audio related tasks using the [Windows Audio Session API (WASAPI)](http://msdn.microsoft.com/library/windows/apps/dd371455).

Specifically, this sample covers:

-   Enumerating audio playback devices attached to the system and retrieve additional properties.
-   Demonstrates how to opt-in to hardware audio offload on supported devices.
-   Demonstrates how to implement the basic media transport controls in order to properly support background audio playback.
-   Playback of audio using the **Windows Audio Session APIs**.
-   Capture of PCM audio using the **Windows Audio Session APIs**.
-   Low latency audio playback and capture.

For more information on adding audio to your Windows Store app, see [Quickstart: adding audio to an app](http://msdn.microsoft.com/library/windows/apps/hh452730).

Playing audio in the background is supported by the Windows Audio Session API
only in communication scenarios as demonstrated by the [VoIP](/Samples/VoIP) sample.
Instead, for general background audio playback of media,
use the BackgroundMediaPlayer class demonstrated in the [Background Audio](/Samples/BackgroundAudio) sample.

## Related topics

### Samples

[Background Audio](/Samples/BackgroundAudio)

[VoIP](/Samples/VoIP)

### Roadmaps

[Audio, video, and camera](https://msdn.microsoft.com/library/windows/apps/mt203788)

[Designing UX for apps](http://msdn.microsoft.com/library/windows/apps/hh767284)

[Roadmap for apps using C\# and Visual Basic](http://msdn.microsoft.com/library/windows/apps/br229583)

[Roadmap for apps using C++](http://msdn.microsoft.com/library/windows/apps/hh700360)

[Roadmap for apps using JavaScript](http://msdn.microsoft.com/library/windows/apps/hh465037)

### Reference

[Windows Audio Session API (WASAPI)](http://msdn.microsoft.com/library/windows/apps/dd371455)

[Core Audio APIs](http://msdn.microsoft.com/library/windows/apps/dd370802)

[Media Foundation](http://msdn.microsoft.com/library/windows/apps/ms694197)

## Operating system requirements

**Client:** Windows 10

**Phone:** Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

To run this sample after building it, press F5 (run with debugging enabled) or Ctrl-F5 (run without debugging enabled) from Visual Studio 2013 for Windows 8.1 (any SKU). (Or select the corresponding options from the Debug menu.)