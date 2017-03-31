<!---
  category: IdentitySecurityAndEncryption
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620031
--->

# Enterprise data protection sample

Shows Enterprise Data Protection (EDP) scenarios: File and buffer protection, clipboard protection,
data protection under lock, and protected access to network resources.
All APIs except File and Buffer protection APIs require Enterprise data policy to be set for a specific identity.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

If you have EDP policy set for a specific identity, you should expect the IsIdentityManaged API to return true. If the API returns false, all APIs except the File and Buffer APIs will not work as expected. Each API has a detailed scenario description that describes the purpose and behavior of the API.

You need to add a [special-use capability](https://msdn.microsoft.com/library/windows/apps/mt270968#special_and_restricted_capabilities)
capability called EnterpriseDataPolicy into your application manifest file to mark your app enlightened.
This sample will not pass WACK because it uses a special-use capability.

## Related Topics

[Enterprise Data Protection Overview](https://msdn.microsoft.com/library/Dn985838.aspx)  

## System requirements

Windows 10 build 10500 or higher

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

## Deploying the sample

1.  Select **Build** \> **Deploy Solution**.

## Deploying and running the sample

1.  To debug the sample and then run it, press F5 or select **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or select**Debug** \> **Start Without Debugging**.

