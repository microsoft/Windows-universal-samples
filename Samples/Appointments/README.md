<!---
  category: ContactsAndCalendar 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620488
--->

# Appointment calendar sample

Shows how to use the [Windows.ApplicationModel.Appointments](http://msdn.microsoft.com/library/windows/apps/dn263359) namespace. 

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

An appointment ID is a **String** object that represents the appointment. Use the 
[ShowAddAppointmentAsync](http://msdn.microsoft.com/library/windows/apps/dn297256) API to obtain an appointment ID of the appointment to add. 
Use this appointment ID with the [ShowReplaceAppointmentAsync](http://msdn.microsoft.com/library/windows/apps/dn297283) or 
[ShowRemoveAppointmentAsync](http://msdn.microsoft.com/library/windows/apps/dn297269) API to update or remove the appointment. 

For more info about how to manage appointments, see [Managing appointments](https://msdn.microsoft.com/library/windows/apps/mt269389).

To obtain an insider copy of Windows 10, go to [Windows 10](http://insider.windows.com). 

**Note**  For Windows 10 app samples, go to  [Windows 10 Samples](https://github.com/Microsoft/Windows-universal-samples). The samples for Windows 10 can be built and run using Windows developer [tools](https://developer.windows.com).

## Related topics

### Reference
For more info about the concepts and APIs demonstrated in this sample, see these topics:

- [Windows.ApplicationModel.Appointments](http://msdn.microsoft.com/library/windows/apps/dn263359)
- [Windows.ApplicationModel.Appointments.AppointmentsProvider](http://msdn.microsoft.com/library/windows/apps/dn297284)
- [AppointmentManager.ShowAddAppointmentAsync](http://msdn.microsoft.com/library/windows/apps/dn297256)
- [AppointmentManager.ShowReplaceAppointmentAsync](http://msdn.microsoft.com/library/windows/apps/dn297283)
- [AppointmentManager.ShowRemoveAppointmentAsync](http://msdn.microsoft.com/library/windows/apps/dn297269)

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or select Debug > Start Without Debugging. 
