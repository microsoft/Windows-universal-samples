<!---
  category: CustomUserInteractions Touch
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620566
--->

# Low latency input sample

Shows how to handle input on a dedicated input thread to achieve lowest input latency. 

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample shows:

1. Setting up two rectangles, one rendered using DirectX and one using XAML
2. Using a SwapChainPanel embedded in a XAML app to render and draw DirectX content
3. Achieving lowest input latency by setting up a dedicated input thread using swapChainPanel->CreateCoreIndependentInputSource(CoreInputDeviceTypes::Mouse | CoreInputDeviceTypes::Touch | CoreInputDeviceTypes::Pen);
4. Achieving lowest output latency using Waitable Back Buffers
5. Artificially blocking the UI thread to show the XAML rect rendering choppy and the DirectX rect rendering smooth and unaffected

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Reference

[Reduce latency with DXGI 1.3 swap chains](https://msdn.microsoft.com/library/windows/apps/mt210781)  
[DirectX and XAML](https://msdn.microsoft.com/library/windows/apps/hh825871.aspx)  
[Creating an dedicated input thread](https://msdn.microsoft.com/library/windows/apps/windows.ui.xaml.controls.swapchainpanel.createcoreindependentinputsource.aspx)  

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:**  Windows 10

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
