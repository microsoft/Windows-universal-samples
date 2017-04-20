<!---
  category: IdentitySecurityAndEncryption
--->

# Microsoft Passport and Windows Hello sample

Shows how apps can incorporate Microsoft Passport and Windows Hello
into their normal sign-in flow to offer convenient and secure multi-factor authentication.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Major steps include:

- Setting up Windows Hello
  - Detecting Microsoft Passport compatibility on the user's device.
  - Offering to start using Microsoft Passport instead of traditional passwords.
  - Creating a Microsoft Passport public/private key pair on the device's Trusted Platform Module (TPM) chip.
  - Registering the public key with the server.
- Signing in with Windows Hello
  - Requesting a challenge from the server.
  - Using Windows Hello to sign the challenge with the private key.
  - Sending the result to the server to complete the sign-in process.
- Unregistering the user and device from the server and the device's TPM chip,
  thereby returning to traditional password sign-in.

## Important notes and warnings

**NOTE**
In order to run this sample, you also need to run the server on a computer on your network.
To do this, load the Server project into a second instance of Visual Studio and press F5 to build and run it.

**NOTE**
The sample assumes that the server is running on the same computer.
If you have deployed the sample to a device that is not a PC,
you will need to edit the serverBaseUri variable to refer to the
device on which the server is running.

**WARNING**
The Server project is for illustration only
and is *not suitable for production use* for many reasons, including the following:

* The sample server uses http instead of https so that you can use network tracing tools
  to observe the communication between the client and the server.
* It stores registered devices in memory only.
  Shutting down the server causes it to lose all memory of registered devices.
* The challenge is very simple.
* After successful authentication, no token is provided to identify the authentication context.

**NOTE**
This sample requires the Windows 10 Build 10586 SDK.
If required, it can be retargeted for the Windows 10 Build 10240 SDK
by changing the client project's target and minimum version in the project properties.
Note, however, that if you retarget to the Windows 10 Build 10240 SDK,
then the app will not run on Windows 10 Build 10586.

**NOTE**
To use Microsoft Passport,
the device must have a valid Microsoft Account or Azure Active Directory account configured in Windows settings.
Microsoft Passport can be enabled by setting a PIN in Windows Settings under **Accounts > Sign-in options**

## Related content

[Microsoft Passport and Windows Hello](http://go.microsoft.com/fwlink/p/?LinkId=522066)  
[Microsoft Passport guide](https://technet.microsoft.com/itpro/windows/keep-secure/microsoft-passport-guide)  
[Windows Hello overview](http://windows.microsoft.com/windows-10/getstarted-what-is-hello)  
[Implementation details for Microsoft Passport and Windows Hello](https://msdn.microsoft.com/library/windows/apps/xaml/mt589441.aspx)  

## System requirements

**Development machine**: Windows 10, Version 1511 and Microsoft Visual Studio 2015 Update 1 with the Windows 10 Build 10586 SDK

**Client**: Windows 10, Version 1511

**Phone**: Windows 10, Version 1511

## Build the sample

1. Download and extract the samples ZIP
2. Start Microsoft Visual Studio 2015 and select **File > Open > Project/Solution**
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample. Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build > Build Solution**.
5. Repeat with the Server solution in the same directory.

## Run the sample
1. Open the Server solution, then press F5 or select **Debug > Start Debugging**.
2. Wait for the browser window to indicate that the server has successfully started.
3. Open a new instance of Visual Studio, open the solution for this sample, then press F5 or select **Debug > Start Debugging**.

## How to use the sample
1. Start the Server solution as described above.
2. Start the sample.
3. In the sample, sign in for the first time. Any username and password can be used on the registration screen, as we're not storing these in this sample.
4. If the device is set up to use Microsoft Passport, a new page will display, prompting the user to start using Windows Hello.
5. Click on **Start using Windows Hello!** to complete the registration process with Microsoft Passport and Windows Hello.
6. Sign out.
7. If you registered to use Windows Hello, you can sign in with Windows Hello instead of a password.
8. Instead of signing in with Windows Hello, you can opt to sign in as another user.
   This will unregister from Windows Hello and return you to the password-based sign in page.

**NOTE** As mentioned previously, the registrations are stored in memory,
which means that the registrations are lost when the server is stopped.
If this occurs, you must unregister from Windows Hello and then re-register.
