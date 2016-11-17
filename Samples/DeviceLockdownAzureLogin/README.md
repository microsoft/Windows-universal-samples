<!--
  category: IdentitySecurityAndEncryption
  samplefwlink: http://go.microsoft.com/fwlink/?LinkID=703786
-->

# Device lockdown with Azure login sample

Shows how to create a sample login application
for Windows Mobile which utilizes the Enterprise Device Lockdown APIs
and optionally authenticates with Azure Active Directory.
The app displays the list of Assigned Access roles on the device,
and clicking on a role will prompt the user to sign in
with credentials from an Azure Active Directory tenant.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample shows how to:

- Call Enterprise Device Lockdown APIs to implement a login application
- Authenticate user credentials with Azure Active Directory

### Setup instructions on the device

This step is needed regardless of whether you intend to use Azure Authentication.

**Warning**: Once Enterprise Assigned Access is deployed to the device,
it cannot be removed.
To get rid of it, you must reflash the device.

1) The Windows Mobile device should be configured with
[Enterprise Assigned Access CSP](https://msdn.microsoft.com/library/windows/hardware/mt157024.aspx).
The wehlockdown.xml file should contain roles with role GUIDs
that will be registered in the Azure Active Directory.
(If you are not using Azure Active Directory,
then you can make up your own role GUIDs.)
For more information, see the references below.

**Remember**: Once Enterprise Assigned Access is deployed to the device,
it cannot be removed.
To get rid of it, you must reflash the device.

### Setup instructions on Azure Active Directory

This step is needed if you intend to use Azure Authentication.

Sign up for an Azure subscription and configure the account at
[Windows Azure Management Portal](https://manage.windowsazure.com).
Your account should come with a Default directory which will be configured in this sample.

1. Add one or more groups to the domain:
  * *Active Directory > Default Directory > Groups > Add Group*
  * Each group contains an Object ID on the Properties page.
    This is the GUID to use for each role in wehlockdown.xml.
2. Add one of more users to the domain:
  * *Active Directory > Default Directory > Users > Add user*
3. Add the users to the groups above as appropriate.
4. Create a native application
  * *Active Directory > Default Directory > Applications > New*
5. Determine your native app's Client ID
  * *Active Directory > Default Directory > Applications > (name of Native Client App) > Configure*
  * In Scenario1_SignIn.cs, set the clientId variable to the Client ID from this page.
6. Get the app's Redirect URI
  * Set a breakpoint on the Scenario1_SignIn constructor method in Scenario1_SignIn.cs
    and inspect the value of the RedirectURI member.
    This will be unique for your app.
7. Set the Redirect URI in Azure Active Directory.
  * *Active Directory > Default Directory > Applications > (name of Native Client App) > Configure*
  * Enter your unique RedirectURI here.

### Setup instructions for the app

This step is needed if you intend to use Azure Authentication.

1. You will need to change the following variables in Scenario1_SignIn.cs to match the values in your Azure Active directory tenant created above.
  * Replace the provided value of "tenant" with your actual tenant address for your Azure account.
  * Replace the provided  value of "clientId" with the actual value of your native application created in Azure Active Directory.
    (See step 5 above.)

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.

To obtain information about Windows 10, go to [Windows 10](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Reference

[Windows.Embedded.DeviceLockdown](https://msdn.microsoft.com/library/windows/apps/windows.embedded.devicelockdown)  
[Windows Azure Management Portal](https://manage.windowsazure.com)  
[Windows app samples](http://go.microsoft.com/fwlink/p/?LinkID=227694)  
[Enterprise Assigned Access CSP](https://msdn.microsoft.com/library/windows/hardware/mt157024.aspx)  
[Lockdown XML](https://msdn.microsoft.com/library/dn798305.aspx)  
[Locking down a device](https://msdn.microsoft.com/library/dn798313.aspx)  

## System requirements

Client: Not supported

Server: Not supported

**Phone:**  Windows 10

## Build the sample

1. Start Microsoft Visual Studio 2015 and select **File** > **Open** > **Project/Solution**.
2. Go to the directory to which you unzipped the sample. Then go to the subdirectory containing the sample in the language you desire - either C++, C#, or JavaScript. Double-click the Visual Studio 2015 Solution (.sln) file. 
3. Press Ctrl+Shift+B, or select **Build** > **Build Solution**. 

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select **Build** > **Deploy Solution**. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select **Debug** > **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or select **Debug** > **Start Without Debugging**. 
