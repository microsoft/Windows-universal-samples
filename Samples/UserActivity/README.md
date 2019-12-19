---
page_type: sample
languages:
- csharp
products:
- windows
- windows-uwp
urlFragment: UserActivity
extendedZipContent:
- path: SharedContent
  target: SharedContent
- path: LICENSE
  target: LICENSE
description: "Shows how to use the UserActivity class to to identify a user work stream that can be continued on the same or another device."
---

<!---
  category: DeepLinksAndAppToAppCommunication
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=873257
-->

# UserActivity sample

Shows how to use the [UserActivity](https://docs.microsoft.com/uwp/api/windows.applicationmodel.useractivities.useractivity) 
class to identify a user work stream that can be continued on the same or another device.

Specifically, this sample shows how to:

- Create basic UserActivity objects, and publish them to the user's UserActivityChannel
- Populate UserActivity objects with advanced metadata such as ContentUri, FallbackUri, Attribution, BackgroundColor and Description.
- Associate an AdaptiveCard with a UserActivity.

**Note** The Windows universal samples require Visual Studio to build and Windows 10 to execute.

To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio and the tools for developing Windows apps, go to [Visual Studio](http://go.microsoft.com/fwlink/?LinkID=532422)

## System requirements

**Client:** Windows 10 

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10 

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
