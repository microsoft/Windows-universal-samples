<!---
  category: IdentitySecurityAndEncryption
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620557
--->

# KeyCredentialManager sample

This sample demonstrates use of the Windows.Security.Authentication.Credentials.KeyCredentialManager API for utlizing Windows Passport as a user authentication method.

The currently existing system of using passwords as authentication is prevalent and as such this sample stands as an example of how Windows Passport can easily be upsold to and integrated into existing authentication applications.

**Note**  This sample was created using one of the universal app templates available in Visual Studio. It shows how its solution is structured so it can run on both Windows 10 and Windows Phone 10.

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
