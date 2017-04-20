<!---
  category: IdentitySecurityAndEncryption
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620621
--->

# Web account management sample

Shows how to get tokens to access different accounts.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample demonstrates the following scenarios:

-   How to get a token for a Microsoft account
-   How to get a token for an Azure AD account (Work or school account)
-   How to show UI and get a token for a single Microsoft account, Azure AD, or other account
-   How to show UI to add, manage, and remove many Microsoft accounts, Azure AD accounts, or custom accounts
-   Adding an account from a custom provider

**Note** The Universal Windows app samples require Visual Studio 2015 to build and Windows 10 to execute.

**Note 2** The app as provided will not be able to get tokens without having it's app manifest being modified to use the App Identity of a registered Microsoft Store/registered AAD app. See 'Related Topics' for instructions on how to register an app.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

Registration of application to use a Microsoft account:
[Preparing your account to use Windows Live Services in your Windows Store apps](https://msdn.microsoft.com/library/windows/apps/xaml/hh770854.aspx)

Develop Windows Universal Apps with Azure AD and the Windows 10 Identity API:
[Using WebAccountManager to Integrate with Azure AD](http://blogs.technet.com/b/ad/archive/2015/08/03/develop-windows-universal-apps-with-azure-ad-and-the-windows-10-identity-api.aspx)

Azure sample:
[Universal Windows Platform app calling the directory Graph API](https://github.com/Azure-Samples/active-directory-dotnet-native-uwp-wam/tree/master/NativeClient-UWP-WAM/)

[Lunch Scheduler app sample](https://github.com/Microsoft/Windows-appsample-lunch-scheduler)  

[Customers Orders Database sample](https://github.com/Microsoft/Windows-appsample-customers-orders-database)  

### Reference

## System requirements

**Client:** Windows 10

**Phone:** Windows 10

In order for the sample to get both MSA and AAD tokens, you must register your app under the Microsoft Store with a Microsoft developer account, as well as with Azure AD.
You must then update the Azure AD ClientID in the individual scenarios, as well as the identity in the package.appxmanifest.

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

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or select Debug > Start Without Debugging. 

### Known Issues


