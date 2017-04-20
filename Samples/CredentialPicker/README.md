<!---
  category: IdentitySecurityAndEncryption
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620526
--->

# Credential picker sample

Shows how to use the Windows.Security.Credentials.UI.CredentialPicker class to retrieve credentials, 
which can then be passed to APIs that may require credentials (for example, HttpClient). This can be used in support of single sign on (SSO).

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

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