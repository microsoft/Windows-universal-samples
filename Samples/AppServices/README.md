<!---
  category: DeepLinksAndAppToAppCommunication
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620489
--->

# App services sample

Shows how an app can provide a service that other apps or other computers can consume.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Each app service scenario usually involves two apps—the client app that opens the connection and sends messages and a provider app that contains the app service and responds to messages. The sample contains a client project called AppServicesClient and a provider project called AppServicesProvider. The AppServicesProvider provides an app service that can generate random numbers. The AppServicesClient demonstrates
- a scenario where the client connects to the app service, gets a random number and closes a connection (OpenCloseConnectionScenario.xaml) 
- a scenario where the client opens a connection to the app service and keeps the connection open unless explicitly closed by the user (KeepConnectionOpenScenario.xaml)

The [RemoteSystem](/Samples/RemoteSystem) sample includes a scenario where
the client connects to this app service from another computer.
This sample declares
SupportsRemoteSystems="true" in its manifest to allow connections from other computers.

To obtain an insider copy of Windows 10, go to [Windows 10](http://insider.windows.com). 

**Note**
- For Windows 10 app samples, go to  [Windows 10 Samples](https://github.com/Microsoft/Windows-universal-samples). The samples for Windows 10 can be built and run using Windows developer [tools](https://developer.windows.com).

# Related topics

## Samples

[RemoteSystems](/Samples/RemoteSystems)

## Reference

[Windows.ApplicationModel.AppServices namespace](https://msdn.microsoft.com/library/windows.applicationmodel.appservice.aspx)

[AppServiceConnection class](https://msdn.microsoft.com/library/windows.applicationmodel.appservice.appserviceconnection.aspx),
which an app uses to open a connection to an app service and send messages to it.

[AppServiceTriggerDetails class](https://msdn.microsoft.com/library/windows.applicationmodel.appservice.appservicetriggerdetails.aspx)
which an app service uses to receive and respond to messages.

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

- Ensure that the AppServiceProvider project is deployed 
- To debug the sample and then run it, press F5 or select Debug >  Start Debugging with the AppServiceClient project set as the startup project. 
- To run the sample without debugging, press Ctrl+F5 or select Debug > Start Without Debugging with the AppServiceClient project set as the startup project. 
