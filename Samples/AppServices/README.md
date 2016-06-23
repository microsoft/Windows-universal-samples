<!---
  category: DeepLinksAndAppToAppCommunication
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620489
--->

# App services sample

This sample demonstrates how an app can provide a service that other apps can consume. [**Windows.ApplicationModel.AppServices**](https://msdn.microsoft.com/en-us/library/windows.applicationmodel.appservice.aspx) namespace. Some of the classes you might want to review in more detail are the [**AppServiceConnection**](https://msdn.microsoft.com/en-us/library/windows.applicationmodel.appservice.appserviceconnection.aspx) class, which you use to open a connection to an app service and send messages to it, and the [**AppServiceTriggerDetails**](https://msdn.microsoft.com/en-us/library/windows.applicationmodel.appservice.appservicetriggerdetails.aspx) class, which an app service can use to receive and respond to messages. 

Each app service scenario usually involves two apps—the client app that opens the connection and sends messages and a provider app that contains the app service and responds to messages. The sample contains a client project called AppServicesClient and a provider project called AppServicesProvider. The AppServicesProvider provides an app service that can generate random numbers. The AppServicesClient demonstrates
- a scenario where the client connects to the app service, gets a random number and closes a connection (OpenCloseConnectionScenario.xaml) 
- a scenario where the client opens a connection to the app service and keeps the connection open unless explicitly closed by the user (KeepConnectionOpenScenario.xaml)

To obtain an insider copy of Windows 10, go to [Windows 10](http://insider.windows.com). 

**Note**  For Windows 10 app samples, go to  [Windows 10 Samples](https://github.com/Microsoft/Windows-universal-samples). The samples for Windows 10 can be built and run using Windows developer [tools](https://developer.windows.com).


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
