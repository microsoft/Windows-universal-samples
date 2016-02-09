<!---
  category: TilesToastAndNotifications
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620577
--->

# Notifications sample

This sample demonstrates the use of the various Toast and LiveTile related API, such as Windows.UI including Notifications, Popups, and StartScreen namespaces.

There are many scenarios in this sample broken down to three major sections:
 
- Badge
- LiveTile
- Toast

### Badge

**Badges** are optional numbers of glyphs that are applied to **LiveTiles**. They enable the app to communicate whether the user has certain amount of notifications, or their attention is needed due to an event. They can also convey user status in a chat application.

**Scenarios include:**

- Setting a numerical badge onto a primary tile 
- Setting a glyph badge onto a primary tile
- Clearing badges
- Setting badges to secondary tiles

### LiveTile

There are two kinds of **LiveTiles**, Primary and Secondary tiles. Primary tiles are persistent and only one exist for each individual application. Secondary tiles can be more numerous and be used as shortcuts to various locations within the app. **LiveTiles** need to be pinned to the Start Menu.

**Scenarios include:**

- Pinning and Unpinning
- Adaptive Templates
  - Branding
  - Names and Logos
  - Hint Presentation
    - People, Photos, Contacts
  - Images
    - From AppData, AppPackage, Internet
  - TextStyles
  - TextWrap
  - Custom
- Basic State
  - Background Color
  - Logos
- FindAllSync API
- Notifications
  - Updating Secondary Tiles
  - Updating Primary Tile
  - Clearing Primary Tile
  - Expiring Notifications 

### Toast

**Toast** notifications are small pop ups that appear on the screen for few seconds. They convey messages and can be customized to even play different sounds. New to Windows 10 are actionable toasts where a user has a choice to interact with the notifications by use of a button, for example.  

**Scenarios Include:**

- Actions
  - Quick Reply
  - Text
  - Text and Image
- Input Text
  - Default Input
  - Placeholder Content
  - Title
- System Commands
  - Snooze
  - Dismiss
- Activation Types
  - Protocol
  - System
  - Background Activation With App Closed
  - Background Activation With App Open
  - Foreground Activation With App Closed
  - Foreground Activation With App Open
- Adaptive Templates
  - Image Sources
    - From App, From AppPackage, From  Internet
- Audio
  - Default Audio
  - Email Sound
  - Looping Audio
  - Silent
- Update Toasts
- Expiring Toasts
- Ghost/Suppressed Toasts
- HistoryChangedTrigger API Sample
  - Badge Control
  - Responding To Toasts
- Various Scenarios
  - Default
  - Alarm
  - Incoming Call
  - Reminder
- ToastNotificationManagerHistory API
  - Get History
  - Remove by Tag
  - Remove by Group
  - Clear All


## System requirements

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
