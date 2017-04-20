<!---
  category: SpeechAndCortana 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=619897
--->

# Speech recognition and synthesis sample

Shows how to use Speech Recognition and Speech Synthesis (Text-to-speech) in UWP apps. 

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample covers the following scenarios:

1. Synthesizing text to speech (TTS)
2. Synthesizing Speech Synthesis Markup Language (SSML)
3. One-shot recognition using the predefined dictation grammar
4. One-shot recognition using the predefined webs search grammar
5. One-shot recognition using a custom list-based grammar
6. One-shot recognition using a custom SRGS/GRXML grammar
7. Continuous dictation
8. Continuous recognition using a custom list-based grammar
9. Continuous recognition using a custom SRGS/GRXML grammar
10. Pausing and resuming continuous recognition 

In addition, translations are shown for speech recognition and text-to-speech for supported languages. Translations provided may not be using ideal phrasing and are provided for demonstration purposes only.

## Privacy Policy

Some speech recognition features require acceptance of the Microsoft Privacy Policy. Information about this privacy policy can be found in the Settings app, under Privacy -> Speech, Inking and Typing. You must view the privacy policy in order to accept it. To view the privacy policy, press the Privacy Policy link on the Speech, Inking and Typing settings page. 

You can disable functionality that requires accepting this policy by turning off "Getting to know you" under Settings -> Privacy -> Speech, Inking and Typing. The samples will indicate to you if the privacy policy has not been accepted where necessary.

## Related topics

[Speech recognition](https://msdn.microsoft.com/library/windows.media.speechrecognition.aspx)  
[Speech synthesis](https://msdn.microsoft.com/library/windows/apps/windows.media.speechsynthesis.aspx)  
[Speech design guidelines](https://msdn.microsoft.com/library/windows/apps/dn596121.aspx)  
[Speech interactions](https://msdn.microsoft.com/library/windows/apps/mt185614)  
[Responding to speech interactions (HTML)](https://msdn.microsoft.com/library/windows/apps/dn720491.aspx)  

## Related samples

[Family Notes sample](https://github.com/Microsoft/Windows-appsample-familynotes)  

## System requirements

**Hardware:** Speech recognition requires an appropriate audio input device. 

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 

### Known Issues

 - The sample requires Media Player components to be available. If media player has been uninstalled, or when using an 'N' SKU of windows without media player components, the sample will not function. Note, however, that Speech Synthesis and Speech Recognition do not require media player directly, but other components of the samples do (Such as playback of synthesized text, or checking to see if a microphone is present and the app has permission to use it.) Developers should make sure their app is aware of it and handles this gracefully.
 
 