# Speech and text-to-speech sample

This sample demonstrates the usage of Speech Recognition and Speech Synthesis (Text to speech) within Universal Applications

Specifically, this sample covers:

-   Unconstrained grammars for dictation and web searches
-   Constrained grammars using List constraints and GRXML constraint files
-   Recognition with and without UI
-   Continuous recognition, for dictation or commanding.
-   Text to speech/speech Synthesis
-   Synthesis using SSML files
-   Synthesis voice configuration

## Security and Privacy

Some speech recognition features require acceptance of the Microsoft Privacy Policy. Information about this privacy policy can be found in the Settings app, under Privacy -> Speech, Inking and Typing. You must view the privacy policy in order to accept it. To view the privacy policy, press the Privacy Policy link on the Speech, Inking and Typing settings page. 

You can disable functionality that requires accepting this policy by turning off "Getting to know you" under Settings -> Privacy -> Speech, Inking and Typing. The samples will indicate to you if the privacy policy has not been accepted where necessary.

In the samples, the scenarios that require acceptance of the privacy policy are the unconstrained Dictation and Web Search scenarios, and the Continuous Dictation scenarios. The other scenarios use local recognition only, and can function without being required to accept the privacy policy.

## Known Issues

On small screen devices, part of the UI may be rendered off the right side of the screen. This is due to a layout issue with the sample app, and will be fixed in upcoming releases, and should not significantly impact functionality. It is not recommended to use the sample in landscape mode.



## Related topics

-  [Speech Recognition](https://msdn.microsoft.com/en-us/library/windows.media.speechrecognition.aspx)
-  [Speech Synthesis](https://msdn.microsoft.com/en-us/library/windows/apps/windows.media.speechsynthesis.aspx)

## System requirements

**Hardware:** Speech recognition requires an appropriate audio input device. 

**Client:** Windows 10 Technical Preview

**Server:** Windows 10 Technical Preview

**Phone:**  Windows 10 Technical Preview

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
 
