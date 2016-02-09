<!---
  category: GraphicsAndAnimation
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620532
--->

# Direct2D gradient mesh sample

This sample demonstrates how to instantiate and render a gradient mesh in [Direct2D](http://msdn.microsoft.com/en-us/library/windows/desktop/dd370990). This samples uses the helper method provided by Direct2D to create a mesh consisting of two tensor patches that share a side.

Some important APIs used in this sample are:

- The [**D2D1::GradientMeshPatch**](http://msdn.microsoft.com/en-us/library/windows/desktop/dn890770) function, which creates the [**D2D1\_GRADIENT\_MESH\_PATCH**](http://msdn.microsoft.com/en-us/library/windows/desktop/dn890726) structures that contain the tensor patch data.
- The [**ID2D1DeviceContext2::CreateGradientMesh**](http://msdn.microsoft.com/en-us/library/windows/desktop/dn890790) method, which creates an [**ID2D1GradientMesh**](http://msdn.microsoft.com/en-us/library/windows/desktop/dn900410) object on the Direct2D device.
- The [**ID2D1DeviceContext2::DrawGradientMesh**](http://msdn.microsoft.com/en-us/library/windows/desktop/dn900378) method, which renders the ID2D1GradientMesh object to the device context.

## Related topics

[**D2D1\_GRADIENT\_MESH\_PATCH** structure](http://msdn.microsoft.com/en-us/library/windows/desktop/dn890726)

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

