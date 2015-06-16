# Lock screen personalization sample

This sample demonstrates how a developer can use the [**LockScreen**](http://msdn.microsoft.com/library/windows/apps/br241847) API to set the current user's lock screen image. This sample uses classes from the [**Windows.System.UserProfile**](http://msdn.microsoft.com/library/windows/apps/br241881) namespace. In particular, it uses the **LockScreen** class to set the user's lock screen image. It also demonstrates how to register an RSS feed that can be used as a source for a lock screen slideshow.

The lock screen is the screen shown when you lock your computer, as well as when you reboot the device or wake it from a sleep state. It is a user-customizable surface that both conveys information and protects the computer against unauthorized use.

The sample lets you use the item picker to select an image your Pictures library to use as the lock screen image. If the sample successfully uses the selected image for the lock screen, the image appears in the output area of the sample's main page. You can also select the location of an RSS feed that can supply images for a lock screen slideshow.

To learn more about the lock screen, see [Lock screen overview](http://msdn.microsoft.com/library/windows/apps/hh779720).

## Related topics

### Reference

[**LockScreen**](http://msdn.microsoft.com/library/windows/apps/br241847)

[**Windows.System.UserProfile**](http://msdn.microsoft.com/library/windows/apps/br241881)

## System requirements

**Client:** Windows 10 Technical Preview

**Server:** Windows 10 Technical Preview

**Phone:** Not supported

## Build the sample

1. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2. Go to the directory to which you unzipped the sample. Then go to the subdirectory containing the sample in the language you desire - either C++, C#, or JavaScript. Double-click the Visual Studio 2015 Solution (.sln) file. 
3. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**. 

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or select Debug > Start Without Debugging. 
