<!---
  category: SpeechAndCortana 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=619899
--->

# Cortana voice command sample

Shows how to integrate with Cortana by providing Voice Command Definitions (VCDs) that allow an app to be invoked in a variety of ways.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample covers:

**C#**:

-   Installing a VCD file. (AdventureWorksCommands.xml, App.xaml.cs:OnLaunched() )
-   Updating a phrase list within a VCD file at runtime. (TripViewModel.cs:UpdateDestinationPhraseList())
-   Handling App.OnActivation() being called with details of a voice command. (App.xaml.cs:OnActivation())  
-   Handling registration of a background task and hooking it up to a voice command. (Package.appxmanifest, AdventureWorksCommands.xml)
-   Implementing the headless invocation APIs for Cortana to acknowledge the invocation, handle cancellation, and completion of the headless invocation to maintain the contract with Cortana, and avoid being terminated. (AdventureWorksVoiceCommandService.cs:Run())
-   Providing various responses to Cortana, which Cortana will display within its own UI, including text, images, app activation URIs, etc. (AdventureWorksVoiceCommandService.cs:SendCompletionMessageForDestination())
-   Implements disambiguation, providing a user with multiple choices, which can either be chosen via voice, or other input mechanisms. (AdventureWorksVoicecommandService.cs:DisambiguateTrips())
-   Implements a confirmation dialog in Cortana, ensuring a user can review a destructive action that will be taken (AdventureWorksVoiceCommandService.cs:SendCompletionMessageForCancellation())
-   Implements progress screens, ensuring that Cortana does not time out during lengthy I/O operations (AdventureWorksVoiceCommandService.cs:ShowProgressScreen())
-   Infix/Suffix Voice Command support, allowing for more natural phrases to be used. 


**WinJS:**

- Installing a VCD file. (default.js, AdventureWorksCommands.xml)
- Updating a phrase list within a VCD file at runtime. (default.js, tripDetails.js)
- Registration of a background task and protocol handler for Cortana (Package.appxmanifest, AdventureWorksCommands.xml)
- Implement a background task and Voice Command services for Cortana (voiceCommandService.js), including:
  - Simple headless Cortana background activation for displaying content within Cortana (when is my trip to *destination*)
  - Confirmation dialogs (cancel trip to *destination*)
  - Disambiguation of multiple items with automatic reprompting.
  - Resolution scaling images
  - Progress dialogs for lengthy operations.
  
**C++/CX**

-  Installing a VCD file. (adventureworkscommands.xml, App.xaml.cpp:OnLaunched() )
-  Updating a phrase list within a VCD file at runtime. (TripViewModel.cpp:UpdateDestinationPhraseList())
-  Handling App::OnActivation() being called with details of a voice command (App.xaml.cs:OnActivation())
-  Handling registration of a background task and hooking it up to a voice command. (Package.appxmanifest, AdventureWorksCommands.xml)
-  Implementing the headless invocation APIs for Cortana to acknowledge the invocation, handle cancellation, and completion of the headless invocation to maintain the contract with Cortana, and avoid being terminated. (AdventureworksVoiceCommandService.cpp:Run())
-  Providing various responses to Cortana, which Cortana will display within its own UI, including text, images, app activation URIs, etc. (AdventureworksVoiceCommandService.cpp:SendCompletionMessageForDestination())
-   Implements disambiguation, providing a user with multiple choices, which can either be chosen via voice, or other input mechanisms. (AdventureworksVoiceCommandService.cpp:DisambiguateTrips())
-   Implements a confirmation dialog in Cortana, ensuring a user can review a destructive action that will be taken (AdventureworksVoiceCommandService.cpp:SendCompletionMessageForCancellation())
-   Implements progress screens, ensuring that Cortana does not time out during lengthy I/O operations (AdventureworksVoiceCommandService.cpp:ShowProgressScreen())
-   Infix/Suffix Voice Command support, allowing for more natural phrases to be used. 

The sample also includes a basic application in order to demonstrate how to integrate this functionality with applications. 

## Setup

1. Ensure Cortana is signed in with an MSA account. This can be achieved by opening Cortana once and following the sign-in process. 
2. Follow the steps in the "Build the sample" section below, then run the application normally once (using either F5 to debug or deploying and then launching normally). This installs the voice command definitions.
3. Close the app.
4. Click on the microphone icon in Cortana's search bar. 
5. Say one of the supported voice commands (see below)

(Note: it may take a small amount of time for Cortana to refresh its installed voice commands.)

## Usage

When Cortana is listening, any of the following voice commands are supported. By default, "London", "Melbourne", and "Yosemite National Park" are provided as sample destinations. New destinations can be added in the app.

- "Adventure Works, show trip to London"
- "Adventure Works, when is my trip to London"
- "Adventure Works, cancel trip to London"

Infix and suffix forms are also supported.

- "Show trip to London in Adventure Works"
- "Show my Adventure Works trip to London"

Duplicates can also be handled. For an example of handling disambiguation, add a second trip to London in the app and try the following:

- "Adventure Works, cancel trip to London"

## Known Issues

* WinJS version of the sample app is not localized. It should handle dates correctly in alternate regions, but currently does not install a localized VCD, or provide localized strings to Cortana.

## Related topics

[Cortana design guidelines](https://msdn.microsoft.com/library/windows/apps/xaml/dn974233.aspx)  
[Cortana interactions (XAML)](https://msdn.microsoft.com/library/windows/apps/mt185598)  
[Cortana interactions (HTML)](https://msdn.microsoft.com/library/windows/apps/dn974231.aspx)  

## See also

[Family Notes sample](https://github.com/Microsoft/Windows-appsample-familynotes)  
[Hue Lights sample](https://github.com/Microsoft/Windows-appsample-huelightcontroller)  

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

Cortana requires an appropriate recording device, and the system must be associated with a Microsoft Account in order for Cortana to function.

## Build the sample

**Note:** This sample has special instructions in the Setup section above. 

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

