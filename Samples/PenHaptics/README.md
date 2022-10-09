---
page_type: sample
languages:
- csharp
- cpp
- cppwinrt
products:
- windows
- windows-uwp
urlFragment: PenHaptics
extendedZipContent:
- path: SharedContent
  target: SharedContent
- path: LICENSE
  target: LICENSE
description: "Shows how to use pen haptics in Universal Windows apps."
---

<!---
  category: CustomUserInteractions Pen
-->

# Pen haptics sample

This sample shows how to use Windows 11 pen haptics API to trigger haptic feedback on a pen that supports haptics.

Specifically, this sample shows how to:

- **Get SimpleHapticsController from pen input:** this sample shows how to go from pointer ID to PenDevice and then to SimpleHapticsController. This requires haptics support from both the pen and a compliant machine that supports the particular pen.
- **Check pen haptics capabilities:** SimpleHapticsController has properties for pen hardware capabilities, such as IsIntensitySupported, IsPlayCountSupported, SupportedFeedback, etc.
- **Start and stop haptic feedback:** start and stop feedback using variations of SendHapticFeedback and StopFeedback API
- **Trigger both inking and interaction haptic feedback:** the code shows how to trigger inking feedback for inking scenarios and interaction feedback for user interactions

**Note** The Windows universal samples require Visual Studio to build and Windows 11 to execute.

To obtain information about Windows 11 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio and the tools for developing Windows apps, go to [Visual Studio](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples

[BasicInput](/Samples/BasicInput)

[ComplexInk](/Samples/ComplexInk)

### Reference

[PenDevice](https://docs.microsoft.com/en-us/uwp/api/windows.devices.input.pendevice)

[SimpleHapticsController](https://docs.microsoft.com/en-us/uwp/api/windows.devices.haptics.simplehapticscontroller)

[Windows pen and ink interactions](https://docs.microsoft.com/en-us/windows/apps/design/input/pen-and-stylus-interactions)

## System requirements

**Client:** Windows 11

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build.
2. Start Microsoft Visual Studio and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution.

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging.
