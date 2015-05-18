# Accounts sample

This sample demonstrates the way to get Tokens to access different accounts 

Specifically, this sample demonstrates the following scenarios:

-   How to get a token for a Microsoft account
-   How to show UI and get a token for a single Microsoft account, Azure AD, or other account
-   How to show UI to add, manage, and remove many Microsoft accounts, Azure AD accounts, or custom accounts

**Note** The Universal Windows app samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10, go to [Windows 10](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Reference

## System requirements

**Client:** Windows 10 Technical Preview

**Phone:** Windows 10 Technical Preview

In order for the sample to get both MSA and AAD tokens, you must register a new application with Microsoft account, as well as with Azure AD.
You must then update the Azure AD ClientID in the individual scenarios, as well as the identity in the package.appxmanifest.

## Build the sample

1. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2. Go to the directory to which you unzipped the sample. Then go to the subdirectory containing the sample in the language you desire - either C++, C#, or JavaScript. Double-click the Visual Studio 2015 Solution (.sln) file. 
3. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**. 

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or select Debug > Start Without Debugging. 

### Known Issues

You may see an Access Denied error when trying to get Microsoft account or Azure AD tokens. For additional information see https://social.msdn.microsoft.com/Forums/windowsapps/en-US/home.


