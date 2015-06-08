# Account picture name sample

This sample demonstrates different ways of getting the name of the user that is currently logged in. It also demonstrates how to get and set the image used for the user's tile.

Specifically, this sample demonstrates the following scenarios:

-   How to get the [**DisplayName**](http://msdn.microsoft.com/library/windows/apps/hh921595) for the current logged on user.
-   How to get the first and last name for the current logged on user. (This is available only for Microsoft accounts. An empty string is returned if a Microsoft account is not available.)
-   How to obtain the logged on user's account picture as a bitmap. You can get request three different types: small, large, and video. If the size that is requested is not available an empty file is returned.
-   How to set the account picture for the currently logged on user. You can set three different types: small, large, and video. (More than one type can be set in the same call, but a small image must be accompanied by a large image and/or video.)
-   How to register for a change event for account picture updates.

**Note** The Universal Windows app samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10, go to [Windows 10](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Reference

[**Windows.System.UserProfile** namespace](http://msdn.microsoft.com/library/windows/apps/br241881)

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

