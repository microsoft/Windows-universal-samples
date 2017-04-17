<!---
  category: ControlsLayoutAndText
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=722922
--->

# WebView control (XAML) sample

Shows how to use the XAML [WebView](https://msdn.microsoft.com/library/windows/apps/windows.ui.xaml.controls.webview.aspx)
control.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample shows how to

* Load different types of content into the WebView
  * Navigate the Webview to a URI with the
    [Navigate](https://msdn.microsoft.com/library/windows/apps/windows.ui.xaml.controls.webview.navigate.aspx)
    method.
  * Load string content into the WebView with the
    [NavigateToString](https://msdn.microsoft.com/library/windows/apps/windows.ui.xaml.controls.webview.navigatetostring.aspx)
    method.
  * Load a file from the app package into the WebView using the
    [ms-appx-web](https://technet.microsoft.com/scriptcenter/jj655406#ms_appx_web) protocol.
  * Load a file from the app's file system storage into the WebView using the
    [ms-appdata](https://technet.microsoft.com/scriptcenter/jj655406#ms_appdata) protocol.
  * Custom loading of HTML and support files (such as CSS, script, and images) with the
    [NavigateToLocalStreamUri](https://msdn.microsoft.com/library/windows/apps/windows.ui.xaml.controls.webview.navigatetolocalstreamuri.aspx)
    method
    and an implementation of the
    [IUriToStreamResolver](https://msdn.microsoft.com/library/windows/apps/windows.web.iuritostreamresolver.aspx)
    interface.

* Obtain information about the WebView
  * Show a progress ring and Stop button during loading.
  * Update the Backward and Forward buttons based on the WebView navigation history.
  * Obtain a screenshot of the current WebView content with the
    [CapturePreviewToStreamAsync](https://msdn.microsoft.com/library/windows/apps/windows.ui.xaml.controls.webview.capturepreviewtostreamasync.aspx)
    method.

* Interact with the WebView
  * Call a JavaScript function in the WebView and obtain the result
    with the
    [InvokeScriptAsync](https://msdn.microsoft.com/library/windows/apps/windows.ui.xaml.controls.webview.invokescriptasync.aspx)
    method.
  * Receive information from WebView-hosted script by handling the
    [ScriptNotify](https://msdn.microsoft.com/library/windows/apps/windows.ui.xaml.controls.webview.scriptnotify.aspx)
    event.
  * Respond to requests from the WebView to enter full screen.

* Support the Share contract by using the
  [CaptureSelectedContentToDataPackageAsync](https://msdn.microsoft.com/library/windows/apps/windows.ui.xaml.controls.webview.captureselectedcontenttodatapackageasync.aspx)
  method to obtain the current selection of the WebView.

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.

To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples

[FullScreenMode](../FullScreenMode)  
[ShareSource](../ShareSource)  
[RSS reader sample](https://github.com/Microsoft/Windows-appsample-rssreader)  

### Reference

[WebView control](https://msdn.microsoft.com/library/windows/apps/windows.ui.xaml.controls.webview.aspx)  
[IUriToStreamResolver interface](https://msdn.microsoft.com/library/windows/apps/windows.web.iuritostreamresolver.aspx)  

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

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 
