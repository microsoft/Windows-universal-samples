<!---
  category: Holographic
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=798591
--->

# Holographic spatial mapping sample

Shows how to acquire spatial mapping data from Windows Perception in
real-time, and provides a simple example for consuming the spatial mapping data.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample requests spatial mapping updates for a space with rectangular extents
20x20x5 meters from the device, and renders the data as a mesh in holographic space. You can choose
one of the following two scenarios:

- Render mesh data in wireframe, visualizing the spatial map in direct comparison to the real world
- Render mesh data as a surface, visualizing the spatial map as an overlay on top of surfaces

This sample uses the spatialPerception app capability, which allows the app to use spatial mapping
data. To do this, the app registers the capability in the app manifest (which uses a schema
extension), and then performs an in-app permissions request, which is automatically granted as a
core feature on the Microsoft HoloLens.

### Render mesh data in wireframe

This is the default operational mode for the sample. Build the sample and deploy it to the
Microsoft HoloLens emulator, or to a Microsoft HoloLens with developer mode enabled. Look around the
room to map your space using the Windows.Perception.Spatial namespace.

### Render mesh data as a surface

This is an optional operational mode for the sample. Starting from wireframe mode, perform an air tap
gesture. The visualization mode will change to render the spatial map as a set of 3D surfaces, using a
simplified lighting algorithm.

### Additional remarks

**Note** The Windows universal samples for Windows 10 Holographic require Visual Studio 2015 Update 2
to build, and a Windows Holographic device to execute. Windows Holographic devices include the
Microsoft HoloLens and the Microsoft HoloLens Emulator.

To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421).

To obtain information about the tools used for Windows Holographic development, including
Microsoft Visual Studio 2015 Update 2 and the Microsoft HoloLens Emulator, go to
[Install the tools](https://developer.microsoft.com/windows/holographic/install_the_tools).

## Reference

The following are used to demonstrate spatial mapping in this code sample:

* [Windows.Perception namespace](https://msdn.microsoft.com/library/windows/apps/windows.perception.aspx)
* [Windows.Perception.Spatial namespace](https://msdn.microsoft.com/library/windows/apps/windows.perception.spatial.aspx)
  * [SpatialPerceptionAccessStatus enumeration](https://msdn.microsoft.com/library/windows/apps/windows.perception.spatial.spatialperceptionaccessstatus.aspx)
* [Windows.Perception.Spatial.Surfaces](https://msdn.microsoft.com/library/windows/apps/windows.perception.spatial.surfaces.aspx)
  * [SpatialSurfaceObserver class](https://msdn.microsoft.com/library/windows/apps/windows.perception.spatial.surfaces.spatialsurfaceobserver.aspx)
  * [SpatialSurfaceInfo class](https://msdn.microsoft.com/library/windows/apps/windows.perception.spatial.surfaces.spatialsurfaceinfo.aspx)
  * [SpatialSurfaceMeshOptions class](https://msdn.microsoft.com/library/windows/apps/windows.perception.spatial.surfaces.spatialsurfacemeshoptions.aspx)
  * [SpatialSurfaceInfo.TryComputeLatestMeshAsync](https://msdn.microsoft.com/library/windows/apps/windows.perception.spatial.surfaces.spatialsurfaceinfo.trycomputelatestmeshasync.aspx)
  * [SpatialSurfaceMesh class](https://msdn.microsoft.com/library/windows/apps/windows.perception.spatial.surfaces.spatialsurfacemesh.aspx)
  * [SpatialSurfaceMeshBuffer class](https://msdn.microsoft.com/library/windows/apps/windows.perception.spatial.surfaces.spatialsurfacemeshbuffer.aspx)


## System requirements

**Client:** Windows 10 Holographic

**Phone:** Not supported

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with
   the sample you want to build.
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the
   subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or
   JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and
run it.

### Deploying the sample to the Microsoft HoloLens emulator

- Click the debug target drop-down, and select **Microsoft HoloLens Emulator**.
- Select **Build** \> **Deploy** Solution.

### Deploying the sample to a Microsoft HoloLens

- Developer unlock your Microsoft HoloLens. For instructions, go to [Enable your device for development]
  (https://msdn.microsoft.com/windows/uwp/get-started/enable-your-device-for-development#enable-your-windows-10-devices).
- Find the IP address of your Microsoft HoloLens. The IP address can be found in **Settings**
  \> **Network & Internet** \> **Wi-Fi** \> **Advanced options**. Or, you can ask Cortana for this
  information by saying: "Hey Cortana, what's my IP address?"
- Right-click on your project in Visual Studio, and then select **Properties**.
- In the Debugging pane, click the drop-down and select **Remote Machine**.
- Enter the IP address of your Microsoft HoloLens into the field labelled **Machine Name**.
- Click **OK**.
- Select **Build** \> **Deploy** Solution.

### Pairing your developer-unlocked Microsoft HoloLens with Visual Studio

The first time you deploy from your development PC to your developer-unlocked Microsoft HoloLens,
you will need to use a PIN to pair your PC with the Microsoft HoloLens.
- When you select **Build** \> **Deploy Solution**, a dialog box will appear for Visual Studio to
  accept the PIN.
- On your Microsoft HoloLens, go to **Settings** \> **Update** \> **For developers**, and click on
  **Pair**.
- Type the PIN displayed by your Microsoft HoloLens into the Visual Studio dialog box and click
  **OK**.
- On your Microsoft HoloLens, select **Done** to accept the pairing.
- The solution will then start to deploy.

### Deploying and running the sample

- To debug the sample and then run it, follow the steps listed above to connect your
  developer-unlocked Microsoft HoloLens, then press F5 or select **Debug** \> **Start Debugging**.
  To run the sample without debugging, press Ctrl+F5 or select **Debug** \> **Start Without Debugging**.
