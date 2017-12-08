<!---
  category: Holographic
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=844964
--->

# Holographic Voice Input Sample

Shows how to implement continuous voice recognition and Speech Synthesis, using a keyword-based constraint list, and the Speech Synthesizer to provide an spoken prompt. The event handler is set to change the color of the cube to a different color, based on voice input (eg, the user saying "red", "green", "blue".) Due to the lack of directional lighting, this produces a uniformly colored/lit cube of the requested color.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

The sample demonstrates how to construct a constraint list, compile it, start continuous recognition, handle events in response to incoming speech, evaluate the confidence that the speech engine has provided, and handle errors. It also demonstrates how to construct a speech synthesizer, and hook it up to XAudio2 for output to the user.

This sample uses the microphone app capability in order to access the user's voice input.

### 
### Additional remarks

**Note** The Windows universal samples for Windows 10 Holographic require Visual Studio 2017 Update 3
to build, and a Windows Holographic device to execute. Windows Holographic devices include the
Microsoft HoloLens and the Microsoft HoloLens Emulator.

To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421).

To obtain information about the tools used for Windows Holographic development, including
Microsoft Visual Studio 2017 Update 3 and the Microsoft HoloLens Emulator, go to
[Install the tools](https://developer.microsoft.com/windows/holographic/install_the_tools).

### Reference

The following types are used in this code sample:
* [Windows.Media.SpeechRecognition namespace](https://docs.microsoft.com/uwp/api/windows.media.speechrecognition)
  * [SpeechRecognizer class](https://docs.microsoft.com/uwp/api/Windows.Media.SpeechRecognition.SpeechRecognizer)
* [SpeechContinuousRecognitionSession class](https://docs.microsoft.com/uwp/api/windows.media.speechrecognition.speechcontinuousrecognitionsession)
  * [SpeechRecognitionCompilationResult class](https://docs.microsoft.com/uwp/api/windows.media.speechrecognition.speechrecognitioncompilationresult)
  * [SpeechRecognitionResult class](https://docs.microsoft.com/uwp/api/windows.media.speechrecognition.speechrecognitionresult)
  * [SpeechRecognitionListConstraint class](https://docs.microsoft.com/uwp/api/windows.media.speechrecognition.speechrecognitionlistconstraint)
  * [SpeechSynthesizer class](https://docs.microsoft.com/uwp/api/windows.media.speechsynthesis.speechsynthesizer)
 
## System requirements

**Client:** Windows 10 Holographic build 14393

**Phone:** Not supported

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with
   the sample you want to build.
2. Start Microsoft Visual Studio 2017 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the
   subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or
   JavaScript). Double-click the Visual Studio Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and
run it.

### Deploying the sample to the Microsoft HoloLens emulator

- Click the debug target drop-down, and select **Microsoft HoloLens Emulator**.
- Select **Build** \> **Deploy** Solution.

### Deploying the sample to a Microsoft HoloLens

- Developer unlock your Microsoft HoloLens. For instructions, go to [Enable your device for development]
  (https://msdn.microsoft.com/windows/uwp/get-started/enable-your-device-for-development#enable-your-windows-10-devices).
- Find the IP address of your Microsoft HoloLens. The IP address can be found in **Settings**
  \> **Network & Internet** \> **Wi-Fi** \> **Advanced options**. Or, you can ask Cortana for this
  information by saying: "Hey Cortana, what's my IP address?"
- Right-click on your project in Visual Studio, and then select **Properties**.
- In the Debugging pane, click the drop-down and select **Remote Machine**.
- Enter the IP address of your Microsoft HoloLens into the field labelled **Machine Name**.
- Click **OK**.
- Select **Build** \> **Deploy** Solution.

### Pairing your developer-unlocked Microsoft HoloLens with Visual Studio

The first time you deploy from your development PC to your developer-unlocked Microsoft HoloLens,
you will need to use a PIN to pair your PC with the Microsoft HoloLens.
- When you select **Build** \> **Deploy Solution**, a dialog box will appear for Visual Studio to
  accept the PIN.
- On your Microsoft HoloLens, go to **Settings** \> **Update** \> **For developers**, and click on
  **Pair**.
- Type the PIN displayed by your Microsoft HoloLens into the Visual Studio dialog box and click
  **OK**.
- On your Microsoft HoloLens, select **Done** to accept the pairing.
- The solution will then start to deploy.

### Deploying and running the sample

- To debug the sample and then run it, follow the steps listed above to connect your
  developer-unlocked Microsoft HoloLens, then press F5 or select **Debug** \> **Start Debugging**.
  To run  the sample without debugging, press Ctrl+F5 or select **Debug** \> **Start Without Debugging**.

