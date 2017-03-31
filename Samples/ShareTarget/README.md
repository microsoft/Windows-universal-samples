<!---
  category: DeepLinksAndAppToAppCommunication 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620597
--->

# Sharing content target app sample

Shows how an app receives content shared from another app. This sample uses classes from the 
[Windows.ApplicationModel.DataTransfer](http://msdn.microsoft.com/library/windows/apps/br205967) and 
[Windows.ApplicationModel.DataTransfer.Share](http://msdn.microsoft.com/library/windows/apps/br205989) namespaces. 

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Some of the classes you might want to review in more detail are the [ShareOperation](http://msdn.microsoft.com/library/windows/apps/br205977) 
class, which you use to manage a share operation, and the [DataPackageView](http://msdn.microsoft.com/library/windows/apps/hh738408) class, 
which you use to get the content being shared. Because each share scenario usually involves two apps—the source app that provides the content 
and a target app that receives the content—we recommend you install and deploy the 
[Sharing content source app sample](http://go.microsoft.com/fwlink/p/?linkid=231511) when you install and run this one. 
That way, you can see how sharing works from end to end. 

This sample covers how to receive shared content in a variety of formats, including:

- Text 
- Web link 
- Application link 
- Images 
- Files 
- Delay-rendered files 
- HTML content 
- Custom data 

Also, this sample shows how to display the source app's 30x30 logo and the source app's package family name.

We also recommend you take a look at the  [Sharing and exchanging data](http://msdn.microsoft.com/library/windows/apps/hh464923) section of our documentation, which describes how sharing works and contains several how-to topics that cover how to share [text](http://msdn.microsoft.com/library/windows/apps/hh758313), an  [image](http://msdn.microsoft.com/library/windows/apps/hh758305), files, and other formats. Our  [Guidelines and checklist for sharing content](http://msdn.microsoft.com/library/windows/apps/hh465251) can also help you create a great user experience with the share feature. 

For more info about the concepts and APIs demonstrated in this sample, see these topics:

- [Sharing content source app sample](http://go.microsoft.com/fwlink/p/?linkid=231511)
- [Sharing and exchanging data](http://msdn.microsoft.com/library/windows/apps/hh464923)
- [How to receive files (HTML)](http://msdn.microsoft.com/library/windows/apps/hh758302)
- [How to receive HTML (HTML)](http://msdn.microsoft.com/library/windows/apps/hh758303)
- [How to receive HTML (XAML)](http://msdn.microsoft.com/library/windows/apps/hh973053)
- [How to receive text (HTML)](http://msdn.microsoft.com/library/windows/apps/hh758304)
- [How to receive text (XAML)](http://msdn.microsoft.com/library/windows/apps/hh973054)
- [Quickstart: Receiving shared content (HTML)](http://msdn.microsoft.com/library/windows/apps/hh465255)
- [Receive data](https://msdn.microsoft.com/library/windows/apps/mt243292)
- [DataPackageView](http://msdn.microsoft.com/library/windows/apps/hh738408)
- [ShareOperation](http://msdn.microsoft.com/library/windows/apps/br205977)
- [Windows.ApplicationModel.DataTransfer](http://msdn.microsoft.com/library/windows/apps/br205967)
- [Windows.ApplicationModel.DataTransfer.Share](http://msdn.microsoft.com/library/windows/apps/br205989)



To obtain an insider copy of Windows 10, go to [Windows 10](http://insider.windows.com). 

**Note**  For Windows 10 app samples, go to  [Windows 10 Samples](https://github.com/Microsoft/Windows-universal-samples). The samples for Windows 10 can be built and run using Windows developer [tools](https://developer.windows.com).


## Related topics

- [Windows 8 app samples](http://go.microsoft.com/fwlink/p/?LinkID=227694)

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

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
