---
page_type: sample
languages:
- csharp
- cpp
- cppcx
- cppwinrt
products:
- windows
- windows-uwp
urlFragment: SmartCard
extendedZipContent:
- path: SharedContent
  target: SharedContent
- path: LICENSE
  target: LICENSE
description: "Shows how to use the Windows.Devices.SmartCards API to work with smart cards and smart card readers programmatically."
---

<!---
  category: IdentitySecurityAndEncryption
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620604
--->

# Smart cards sample

Shows how to use the Windows.Devices.SmartCards API to work with smart cards and smart card readers programmatically.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> You can download this sample as a standalone ZIP file
> [from docs.microsoft.com](https://docs.microsoft.com/samples/microsoft/windows-universal-samples/smartcard/),
> or you can download the entire collection as a single
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample shows:

- Creating a virtual smart card
- Changing the PIN on a virtual smart card
- Resetting the PIN on a virtual smart card
- Changing the admin key
- Verifying the response
- Deleting a virtual smart card
- Listing all the smart cards on the machine
- Transmitting

**Note:**  In order to create a virtual smart card using a UWP app, the user running the app must be a member of the administrators group. If the user is not a member of the administrators group, virtual smart card creation will fail.

## Related topics

### Reference

[Windows.Devices.SmartCards](http://msdn.microsoft.com/library/windows/apps/dn263949)  

### Related samples

* [SmartCard sample](/archived/SmartCard/) for JavaScript (archived)

## System requirements

* Windows 10

Build the sample
----------------

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

Run the sample
--------------

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

**Deploying the sample**

1.  Select **Build** \> **Deploy Solution**.

**Deploying and running the sample**

1.  To debug the sample and then run it, press F5 or select **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or select**Debug** \> **Start Without Debugging**.
