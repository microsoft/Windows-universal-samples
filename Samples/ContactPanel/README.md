---
page_type: sample
languages:
- csharp
products:
- windows
- windows-uwp
urlFragment: ContactPanel
extendedZipContent:
- path: SharedContent
  target: SharedContent
- path: LICENSE
  target: LICENSE
description: "Shows how to host your app within the My People Contact Panel."
---

<!---
  category: ContactsAndCalendar
  samplefwlink: http://go.microsoft.com/fwlink/?LinkID=866040
-->

# Contact Panel sample

Shows how to host your app within the My People Contact Panel.

Specifically, this sample shows how to:

- Create a contact that is annotated for My People and can be pinned to the taskbar.
- Allow your app to be activated within the My People Contact Panel on the taskbar.
- Communicate information as to which contact was activated.

**Note** The Windows universal samples require Visual Studio to build and Windows 10 to execute.

To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio and the tools for developing Windows apps, go to [Visual Studio](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples

[MyPeopleNotifications] (../MyPeopleNotifications)  
[ContactPicker] (../ContactPicker)  

### Reference

[ContactPanel Class](https://docs.microsoft.com/en-us/uwp/api/windows.applicationmodel.contacts.contactpanel)

[ContactStore Class](https://docs.microsoft.com/en-us/uwp/api/windows.applicationmodel.contacts.contactstore)

### Conceptual

[Adding My People support to an application](https://docs.microsoft.com/en-us/windows/uwp/contacts-and-calendar/my-people-support)

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
