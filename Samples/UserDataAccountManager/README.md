---
page_type: sample
languages:
- csharp
- cpp
- cppcx
products:
- windows
- windows-uwp
urlFragment: UserDataAccountManager
extendedZipContent:
- path: SharedContent
  target: SharedContent
- path: LICENSE
  target: LICENSE
description: "Shows how to add, manage, and fix errors for accounts that provide email, calendar, and contacts."
---

<!---
  category: ContactsAndCalendar 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620616
--->

# UserDataAccountManager sample

Shows how to use the [UserDataAccountManager](https://msdn.microsoft.com/library/windows/apps/windows.applicationmodel.userdataaccounts.userdataaccountmanager.aspx) class
to let the user add, manage, and fix errors for accounts that provide email, calendar, and contacts.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> You can download this sample as a standalone ZIP file
> [from docs.microsoft.com](https://docs.microsoft.com/samples/microsoft/windows-universal-samples/userdataaccountmanager/),
> or you can download the entire collection as a single
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample requires the following capabilities:

- appointments
- contacts
- email 

For more info, see [App capability declarations](https://msdn.microsoft.com/library/windows/apps/mt270968).

### Remarks

- This sample will not pass WACK because it uses a special-use capability.

## Related topics

### Related samples

* [UserDataAccountManager sample](/archived/UserDataAccountManager/) for JavaScript (archived)

## System requirements

* Windows 10

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

