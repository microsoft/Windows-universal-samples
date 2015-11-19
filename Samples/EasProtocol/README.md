<!---
  category: NetworkingAndWebServices 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620540
--->

# Exchange Active Sync (EAS) sample

This sample shows how mail clients can retrieve device information and work with supplied Exchange Active Sync (EAS) policies.

Windows Store apps can configure their mail clients to stay compliant with the given EAS policies. To bring the local computer compliant, you must request the user's consent before you proceed through the use of a consent UI. Then you can configure the exchange account.

This sample requires some experience with WMI programming.

**Note**  This sample was created using one of the universal app templates available in Visual Studio. It shows how its solution is structured so it can run on both Windows 10 and Windows Phone 10.

You can choose one of three scenarios: 
- Retrieving device information
- Checking if device policies are compliant with supplied EAS policies
- Applying supplied EAS policies.

## Related Topics

[Windows.Security.ExchangeActiveSyncProvisioning](http://msdn.microsoft.com/library/windows/apps/hh701506)

## Operating system requirements

Windows 10

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
