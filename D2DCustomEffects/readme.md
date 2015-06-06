# Direct2D custom image effects sample

This sample demonstrates how to implement [custom Direct2D effects](http://msdn.microsoft.com/library/windows/desktop/jj710194) using HLSL pixel, vertex, and compute shaders.

Specifically, this sample shows how to:

- Define the effect's properties, metadata and implement required interfaces
- Author HLSL that conforms to Direct2D requirements, including opting into effect shader linking
- Register and use a custom effect in an app

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10, go to [Windows 10](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Feature areas

- [**Direct2D effects**](http://msdn.microsoft.com/library/windows/desktop/hh706327): provides built-in effects and the ability to create custom effects.
- [**Direct2D**](http://msdn.microsoft.com/library/windows/desktop/dd370990): used to render images, primitives, and text.
- [**Windows Imaging Component (WIC)**](http://msdn.microsoft.com/library/windows/desktop/ee719655): used to load, scale, and convert the images.

### Reference

Custom effect interfaces:

- [ID2D1EffectContext](http://msdn.microsoft.com/library/windows/desktop/hh404459)
- [ID2D1EffectImpl](http://msdn.microsoft.com/library/windows/desktop/hh404568)
- [ID2D1Transform](http://msdn.microsoft.com/library/windows/desktop/hh446919)

Related DirectX app APIs:

- [ID2D1DeviceContext](http://msdn.microsoft.com/library/windows/desktop/hh404479)
- [IDXGIFactory2::CreateSwapChainForCoreWindow](http://msdn.microsoft.com/library/windows/desktop/hh404559)
- [CoreWindow](http://msdn.microsoft.com/library/windows/apps/br208225)

## System requirements

**Client:** Windows 10 Technical Preview

**Server:** Windows 10 Technical Preview

**Phone:**  Windows 10 Technical Preview

## Build the sample

1. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2. Go to the directory to which you unzipped the sample. Then go to the subdirectory containing the sample in the language you desire - either C++, C#, or JavaScript. Double-click the Visual Studio 2015 Solution (.sln) file. 
3. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**. 

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 