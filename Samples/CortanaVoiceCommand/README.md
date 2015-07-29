<!---
category: SpeechAndCortana 
--->
# Cortana Voice Command Sample

This sample demonstrates integrating with Cortana to provide Voice Command Definitions (VCDs) that allow an application to be invoked in a variety of ways.

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

The sample also includes a basic application in order to demonstrate how to integrate this functionality with applications. 

## Setup

1. Ensure Cortana is signed in with an MSA account. This can be achieved by opening Cortana once and following the sign-in process. 
2. Run the application normally once (eg, via F5 debug or deploy/launch). This installs the voice command definitions.
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

-  [Cortana design guidelines](https://msdn.microsoft.com/en-us/library/windows/apps/xaml/dn974233.aspx)
-  [Cortana interactions (XAML)](https://msdn.microsoft.com/en-us/library/windows/apps/xaml/dn974230.aspx)
-  [Cortana interactions (HTML)](https://msdn.microsoft.com/en-us/library/windows/apps/dn974231.aspx)

## System requirements

Cortana requires an appropriate recording device, and the system must be associated with a Microsoft Account in order for Cortana to function.