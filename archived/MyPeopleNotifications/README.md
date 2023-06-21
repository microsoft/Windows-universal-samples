---
topic: sample
languages:
- csharp
- cpp
- cppcx
products:
- windows
- windows-uwp
urlFragment: MyPeopleNotifications
extendedZipContent:
- path: SharedContent
  target: SharedContent
- path: LICENSE
  target: LICENSE
description: "Shows how to send emotive notifications to contacts pinned on the taskbar."
---

<!---
  category: ContactsAndCalendar
  samplefwlink: http://go.microsoft.com/fwlink/?LinkID=859519
--->

# My People Notifications Sample

Shows how to send [emotive notifications](https://docs.microsoft.com/en-us/windows/uwp/contacts-and-calendar/my-people-notifications) to contacts pinned on the taskbar.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> You can download this sample as a standalone ZIP file
> [from docs.microsoft.com](https://docs.microsoft.com/samples/microsoft/windows-universal-samples/mypeoplenotifications/),
> or you can download the entire collection as a single
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

The sample demonstrates the following tasks:

- Send a local static image as a notification to a pinned contact.
- Send a remote image as a notification to a pinned contact.
- Send a notification to a pinned contact identified by phone number.
- Send a notification to a pinned contact identified by remote ID.
- Send an animated gif as a notification to a pinned contact.
- Send a spritesheet as an animated notification to a pinned contact.

**Note** The Windows universal samples require Visual Studio to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio and the tools for developing Windows apps, go to [Visual Studio](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples
[ContactPanel] (../ContactPanel)

### Notifications

[Notifications](/Samples/Notifications)  

### Reference

[Tiles, badges, and notifications](https://docs.microsoft.com/en-us/windows/uwp/controls-and-patterns/tiles-badges-notifications)  
[My People Notifications](https://docs.microsoft.com/en-us/windows/uwp/contacts-and-calendar/my-people-notifications)  
[ContactPicker class](http://msdn.microsoft.com/library/windows/apps/br224913)  
[Contacts, My People, and calendar](https://msdn.microsoft.com/library/windows/apps/mt269388)  

## System requirements

**Client:** Windows 10

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

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or select Debug > Start Without Debugging. 

### Shared Code

The sample shares XAML between the c++ and c# projects. The shared XAML can be found in the *SharedXaml* directory. It also shares assets between the projects, which can be found in the *SharedAssets* directory.

