<!---
  category: ContactsAndCalendar
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=624040
--->

# Contact cards sample

Shows how to display contact cards using methods of the
[ContactManager](https://msdn.microsoft.com/library/windows/apps/windows.applicationmodel.contacts.contactmanager.aspx)
class.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample has three scenarios.

- **Show mini contact card**

  Mini contact cards appear as a pop-up window inside the app.
  Apps provide an anchor rectangle around which the mini card will be displayed
  and can provide a suggested position of the mini contact card relative to the anchor rectangle.
  The [ShowContactCard](https://msdn.microsoft.com/library/windows/apps/windows.applicationmodel.contacts.contactmanager.showcontactcard.aspx)
  method displays a mini contact card.

- **Show mini contact card with delay loaded-data**

  Display the mini contact card immediately
  while the app obtains additional information about the contact in the background.
  The card is updated when the additional information is available.
  The [ShowDelayLoadedContactCard](https://msdn.microsoft.com/library/windows/apps/windows.applicationmodel.contacts.contactmanager.showdelayloadedcontactcard.aspx)
  method displays a delay loaded mini contact card.

- **Show full contact card**

  Full contact cards appear as a separate window.
  The [ShowFullContactCard](https://msdn.microsoft.com/library/windows/apps/windows.applicationmodel.contacts.contactmanager.showfullcontactcard.aspx)
  method displays a full contact card.

Not all devices support mini contact cards.
The samples also demonstrate how to detect support for mini contact cards.

**Note** The Universal Windows app samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples

[ContactPicker](../ContactPicker)  

### Reference

[Contact](http://msdn.microsoft.com/library/windows/apps/br224849)  
[ContactCardDelayedDataLoader](http://msdn.microsoft.com/library/windows/apps/dn297400)  
[Windows.ApplicationModel.Contacts](http://msdn.microsoft.com/library/windows/apps/br225002)  

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10 (full contact cards only)

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
