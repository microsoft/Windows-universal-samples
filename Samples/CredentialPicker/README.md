<!---
  category: IdentitySecurityAndEncryption
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620526
--->

# Credential picker sample

This sample shows how to use the Windows.Security.Credentials.UI.CredentialPicker class to retrieve credentials, which can then be passed to APIs that may require credentials (for example, HttpClient). This can be used in support of single sign on (SSO).

**Note**  This sample was created using one of the universal app templates available in Visual Studio. It shows how its solution is structured so it can run on both Windows 10 and Windows Phone 10.

This sample allows the user launch different types of credential prompts. You can choose one of three scenarios: 
- Message
- Message and Caption
- Message, Caption, Save Credential Option, and a type of protocol.

Operating system requirements
-----------------------------

Windows 10

Related topics
---------------

[Windows.Security.Credentials] (https://msdn.microsoft.com/library/windows/apps/br227089)
[Windows.Security.Credentials.UI] (https://msdn.microsoft.com/library/windows/apps/hh701356)

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