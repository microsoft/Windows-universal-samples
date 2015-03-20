# Speech and text-to-speech sample

This sample demonstrates the usage of Speech Recognition and Speech Synthesis (Text to speech) within Universal Applications

Specifically, this sample covers:

-   Unconstrained grammars for dictation and web searches
-   Constrained grammars using List constraints and SRGS constraint files
-   Recognition with and without UI
-   Text to Speech/Speech Synthesis
-   Synthesis using SSML files
-   Synthesis voice configuration

## Related topics

-  [Speech Recognition](https://msdn.microsoft.com/en-us/library/windows.media.speechrecognition.aspx)
-  [Speech Synthesis](https://msdn.microsoft.com/en-us/library/windows/apps/windows.media.speechsynthesis.aspx)

## System requirements

**Hardware:** Speech recognition requires an appropriate audio input device. 

**Client:** Windows 10 Technical Preview

**Server:** Windows 10 Technical Preview

**Phone:**  Windows 10 Technical Preview

**Known Issues:**

- RecognizeWithUIAsync is currently not available on Windows Desktop, and will be available in later releases of Windows 10.
- SpeechRecognition fails to initialize in the Windows Phone emulator. It does work on physical Windows Phone devices.

## Build the sample

1. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2. Go to the directory to which you unzipped the sample. Then go to the subdirectory containing the sample in the language you desire - either C++, C#, or JavaScript. Double-click the Visual Studio 2015 Solution (.sln) file. 
3. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**. 

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 
 