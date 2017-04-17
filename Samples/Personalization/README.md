<!--
  category: IdentitySecurityAndEncryption
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620585
-->

# Lock screen personalization sample

Shows how a developer can use the [LockScreen](http://msdn.microsoft.com/library/windows/apps/br241847) API to set the current user's lock screen image. 

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample uses classes from the [Windows.System.UserProfile](http://msdn.microsoft.com/library/windows/apps/br241881) namespace. 
In particular, it uses the **LockScreen** class to set the user's lock screen image.

The lock screen is the screen shown when you lock your computer, as well as when you reboot the device or wake it from a sleep state. 
It is a user-customizable surface that both conveys information and protects the computer against unauthorized use.

The sample lets you use the item picker to select an image your Pictures library to use as the lock screen image. 
If the sample successfully uses the selected image for the lock screen, the image appears in the output area of the sample's main page.

To learn more about the lock screen, see [Lock screen overview](http://msdn.microsoft.com/library/windows/apps/hh779720).

## Related topics

### Reference

[LockScreen](http://msdn.microsoft.com/library/windows/apps/br241847)  
[Windows.System.UserProfile](http://msdn.microsoft.com/library/windows/apps/br241881)  

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Not supported

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or select Debug > Start Without Debugging. 
