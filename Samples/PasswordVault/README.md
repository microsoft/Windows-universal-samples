The Credential Locker provides a way for you to store user credentials (username, password) in a secure fashion for your app. Usernames and passwords stored using the credential locker are encrypted and saved locally. Once you have the credentials stored, then you can automatically sign users in for a more convenient user experience. Additionally, user credentials stored in the Credential Locker roam with the user's Microsoft Account for added convenience. For more information, see Storing user credentials.

This sample demonstrates how to use the Credential Locker and typical login sequences to store web credentials. Specific scenarios include a single user with a single resource, multiple users with a single resource, multiple users with multiple resources, and deleting stored credentials.

**Note**  This sample was created using one of the universal app templates available in Visual Studio. It shows how its solution is structured so it can run on both Windows 10 and Windows Phone 10.

This sample allows the user different types of ways to interact with credentials in the vault.
- Saving credentials
- Reading credentials
- Deleting credentials

Related Topics
--------------
[Storing user credentials](http://msdn.microsoft.com/library/windows/apps/hh465060)
[Windows.Security.Credentials](http://msdn.microsoft.com/library/windows/apps/br227089)

Operating system requirements
-----------------------------

Windows 10

Build the sample
----------------

1.  Start Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2.  Go to the directory to which you unzipped the sample. Then go to the subdirectory containing the sample in the language you desire - either C++ or C\#. Double-click the Visual Studio 2015 Solution (.sln) file.
3.  Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

Run the sample
--------------

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

**Deploying the sample**

1.  Select **Build** \> **Deploy Solution**.

**Deploying and running the sample**

1.  To debug the sample and then run it, press F5 or select **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or select**Debug** \> **Start Without Debugging**.
