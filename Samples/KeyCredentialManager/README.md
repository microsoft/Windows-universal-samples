<!---
  category: IdentitySecurityAndEncryption
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620557
--->

# KeyCredentialManager sample

Shows how to use the [Windows.Security.Authentication.Credentials.KeyCredentialManager](https://msdn.microsoft.com/library/windows/apps/windows.security.credentials.keycredentialmanager.aspx)
API for utlizing Windows Passport as a user authentication method.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

The currently existing system of using passwords as authentication is prevalent and as such this sample stands as an example of how Windows Passport 
can easily be upsold to and integrated into existing authentication applications.

Shows the account lifecycle for an existing app user utilizing Passport. This includes: 
- Provisioning Passport for the user
- Handling both Passport and password methods of login
- Checking Passport to authenticate the user
- Deleting the Passport for a logged in user.

Related topics
--------------

[Windows.Security.Credentials](http://msdn.microsoft.com/library/windows/apps/br227089)  

Operating system requirements
-----------------------------

Windows 10

Build the sample
----------------

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

Run the sample
--------------

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

**Deploying the sample**

1.  Select **Build** \> **Deploy Solution**.

**Deploying and running the sample**

1.  To debug the sample and then run it, press F5 or select **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or select**Debug** \> **Start Without Debugging**.
