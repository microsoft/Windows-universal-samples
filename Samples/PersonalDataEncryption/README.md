---
page_type: sample
languages:
- csharp
- cplusplus
- cppwinrt
products:
- windows
- windows-uwp
urlFragment: PersonalDataEncryption
extendedZipContent:
- path: SharedContent
  target: SharedContent
- path: LICENSE
  target: LICENSE
description: "Demonstrates the usage of Personal Data Encryption (Windows Data Protection)."
---

# Personal Data Encryption sample

Shows how to use [Personal Data Encryption](https://learn.microsoft.com/windows/security/operating-system-security/data-protection/personal-data-encryption/)
for protecting user files and memory buffers.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> You can download this sample as a standalone ZIP file
> [from docs.microsoft.com](https://docs.microsoft.com/samples/microsoft/windows-universal-samples/resizeappview/),
> or you can download the entire collection as a single
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Personal Data Encryption can protect data either while a user is signed ("Level 1" or "L1") out or while a system is locked ("Level 2" or "L2").

The sample shows the following:

- Marking a folder so that all new files are protected at a particular level, or removing protection.
- Protecting a file at a particular level, or removing protection.
- Protecting a memory buffer at a particular level, or removing protection.

**Note** The Windows universal samples require Visual Studio to build and Windows 11 to execute.
 
To obtain information about Windows development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio and the tools for developing Windows apps, go to [Visual Studio](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Reference

[UserDataProtectionManager class](https://learn.microsoft.com/en-us/uwp/api/windows.security.dataprotection.userdataprotectionmanager)

### Related samples

* [EnterpriseDataProtection](/Samples/EnterpriseDataProtection)

## System requirements

* Windows 11 version 22H2 or higher

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
- In order for the sample to exercise Personal Data Encryption, [it must be configured](https://learn.microsoft.com/windows/security/operating-system-security/data-protection/personal-data-encryption/configure).
