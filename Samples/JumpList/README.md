<!---
  category: DeepLinksAndAppToAppCommunication
--->

# # Jump list customization sample

Shows how to customize your app's jump list. A jump list is a system-provided menu that appears when the user right-clicks a program in the taskbar or on the
Start menu. It is used to provide quick access to recently or frequently-used documents and offer direct links to app functionality.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

The sample shows the following techniques:

- Disable the jump list.
- Create a jump list based on Recent or Frequent items.
- Adding tasks to the jump list.
- Creating groups of items in a jump list.
- Perform an action when a jump list item is activated by the user.

**Note** Although the sample will compile and run, the level of functionality will depend on which build of Windows you are running and what device you are running it on.
In particular, phone does not support displaying or storing jump list information at this time.

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10, go to [Windows 10](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Reference

[JumpList class](https://msdn.microsoft.com/library/windows/apps/windows.ui.startscreen.jumplist.aspx)  
[JumpListItem class](https://msdn.microsoft.com/library/windows/apps/windows.ui.startscreen.jumplistitem.aspx)  

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10 (but non-functional; see discussion above)

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
