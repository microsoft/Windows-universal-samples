This sample demonstrates the usage of Speech Recognition and Speech Synthesis (Text to speech) within Universal Applications

Specifically, this sample covers:

-   Unconstrained grammars for dictation and web searches
-   Constrained grammars using List constraints and SRGS constraint files
-   Recognition with and without UI
-   Text to Speech/Speech Synthesis
-   Synthesis using SSML files
-   Synthesis voice configuration

Related topics
--------------

**Reference**

-  [Speech Recognition](https://msdn.microsoft.com/en-us/library/windows.media.speechrecognition.aspx)
-  [Speech Synthesis](https://msdn.microsoft.com/en-us/library/windows/apps/windows.media.speechsynthesis.aspx)

System requirements
-----------------------------
Speech recognition requires an appropriate audio input device. 


Known Issues
-----------------------------
-   RecognizeWithUIAsync is currently not available on Windows Desktop, and will be available in later releases of Windows 10.
-   SpeechRecognition fails to initialize in the Windows Phone emulator. It does work on physical Windows Phone devices. 