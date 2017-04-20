<!---
  category: GraphicsAndAnimation
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620532
--->

# Direct2D gradient mesh sample

Shows how to instantiate and render a gradient mesh in [Direct2D](http://msdn.microsoft.com/library/windows/desktop/dd370990). 
This samples uses the helper method provided by Direct2D to create a mesh consisting of two tensor patches that share a side.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Some important APIs used in this sample are:

- The [D2D1::GradientMeshPatch](http://msdn.microsoft.com/library/windows/desktop/dn890770) function, which creates the [D2D1\_GRADIENT\_MESH\_PATCH](http://msdn.microsoft.com/library/windows/desktop/dn890726) structures that contain the tensor patch data.
- The [ID2D1DeviceContext2::CreateGradientMesh](http://msdn.microsoft.com/library/windows/desktop/dn890790) method, which creates an [ID2D1GradientMesh](http://msdn.microsoft.com/library/windows/desktop/dn900410) object on the Direct2D device.
- The [ID2D1DeviceContext2::DrawGradientMesh](http://msdn.microsoft.com/library/windows/desktop/dn900378) method, which renders the ID2D1GradientMesh object to the device context.

## Sample project files

The sample's project files fall into the following categories.

### Sample-specific files
The following files exercise the gradient mesh APIs and form the main educational content of the sample:

- App.cpp/.h
- D2DGradientMeshMain.cpp/.h
- D2DGradientMeshRenderer.cpp/.h

D2DGradientMeshRenderer contains most of the gradient mesh-specific code.

### DirectX SDK sample common files
The following files provide common functionality needed by DirectX SDK samples:

- **DeviceResources.cpp/.h:** Manages creation and lifetime of the core Direct3D and Direct2D device-dependent resources. Handles cases such as device lost and window size and orientation changes.
- **DirectXHelper.h:** Common inline helper functions, including ThrowIfFailed which converts HRESULT-based APIs into an exception model.
- **SampleOverlay.cpp/.h:** Renders the Windows SDK overlay badge on top of sample content.

All DX SDK samples and the Visual Studio template DX project contain a version of these files. These common files demonstrate important best practices for DX UWP apps, and you are encouraged to use them in your own projects.

### C++ UWP common files
Variants of the following files are found in every UWP app written in C++:

- Package.appxmanifest
- pch.cpp/.h
- D2DGradientMesh.vcxproj
- D2DGradientMesh.vcxproj.filters
- D2DGradientMesh.sln

## Related topics

[D2D1\_GRADIENT\_MESH\_PATCH structure](http://msdn.microsoft.com/library/windows/desktop/dn890726)  

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

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 

