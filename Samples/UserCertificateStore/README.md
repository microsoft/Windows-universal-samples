<!---
  category: IdentitySecurityAndEncryption
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=784879
--->

# UserCertificateStore sample

This sample shows how you can use UserCertificateStore class to add and delete a certificate
in the user's User Personal certificate store (also known as the MY store).
The UserCertificateStore parallels the CertificateStore class which represents
the App's certificate store.

This sample demonstrates the following:

* Enumerating all certificates and identifying the app certificates and user certificates.
* Moving a certificate from the app certificate store to the user certificate store
  by adding it to the user certificate store
  and deleting it from the app certificate store.
* Moving a certificate from the user certificate store to the app certificate store
  by adding it to the app certificate store
  and deleting it from the user certificate store.

In order to use the UserCertificateStore class,
the app must have the sharedUserCertificate capability.

## Related topics

### Reference

[UserCertificateStore class](https://msdn.microsoft.com/library/windows/apps/windows.security.cryptography.certificates.usercertificatestore.aspx)

[CertificateStores class](https://msdn.microsoft.com/library/windows/apps/windows.security.cryptography.certificates.certificatestores.aspx)

[CertificateStore class](https://msdn.microsoft.com/library/windows/apps/windows.security.cryptography.certificates.certificatestore.aspx)

## System requirements

**Client:** Windows 10 build 14295

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10 build 14295

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

## Deploying the sample

1.  Select **Build** \> **Deploy Solution**.

## Deploying and running the sample

1.  To debug the sample and then run it, press F5 or select **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or select**Debug** \> **Start Without Debugging**.
