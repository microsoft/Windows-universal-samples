<!---
  category: ControlsLayoutAndText Printing
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=722936
  language: cs
--->

# 3D Printing from Unity sample

Shows how to load a Unity 3D asset into the Windows 10 3D print dialog.
From there, the user can repair the model, make simple modifications,
and send it to a local 3D printer or an online 3D print service.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

**Note** that this is not a traditional sample
that you can simply load into Visual Studio.
Follow the instructions below to create and run the sample.

# Prerequisites

In addition to Visual Studio and the Windows SDK, you need the following:

* Unity version
  [5.2.1p2](https://unity3d.com/unity/qa/patch-releases "Unity") or later.

* The Unity plugin for Visual Studio.
  This can be installed as part of installing Unity,
  or you can
  [download it](https://visualstudiogallery.msdn.microsoft.com/8d26236e-4a64-4d64-8486-7df95156aba9 "Visual Studio 2015 Tools for Unity")
  manually.

* The 3D Builder app.
  This application comes preinstalled with Windows 10.
  If you have uninstalled it,
  you can
  [reinstall it from the Store](https://www.microsoft.com/store/apps/3d-builder/9wzdncrfj3t6 "3D Builder").

A basic understanding of Unity is assumed.
Learn how to get started with Unity
[here](https://unity3d.com/learn/tutorials "Unity tutorials") 

# Create the sample

We provide only the scripts Print3D.cs and Turn.cs.
You will create the scene and the model (fbx model, material, and texture)
yourself.

1. Start Unity and create a new 3D project.

2. Import an asset from the
   [Unity store](https://www.assetstore.unity3d.com/en/ "Unity Store").
   For this sample, we will use [Cartoon crucian carp](https://www.assetstore.unity3d.com/en/#!/content/46132 "Fish").

3. Create an empty GameObject and drag the FBX asset from the All Models folder
   into the newly-created GameObject.

4. In the Assets\nnj3de_cruscarp folder of the project, click cruscarp.png.
   This will show "cruscarp Import Settings" in the Inspector pane.

5. In the Inspector pane for "cruscarp Import Settings",
   set the Texture Type to **Advanced**,
   and then check the **Read/Write Enabled** checkbox,
   and then set the Format to **RGBA 32 bit**.

6. Click Apply at the bottom of the Inspector pane to apply the changes.

7. Go to the All Scripts folder, right-click an empty space,
   and select **Import New Asset**.
   Select the Print3D.cs and (optionally) Turn.cs files from this sample.
   The Turn.cs script lets the user rotate the object and inspect
   it within the Unity editor.

8. Expand the GameObject you created in step 3
   to reveal the **cruscarp** node.
   Expand the **cruscarp** node to reveal a second
   **cruscarp** node. This is the rendering mesh.

9. Drag the Print3D.cs file (and optionally Turn.cs file)
   from the All Scripts folder onto the rendering mesh node
   you revealed in step 8.

10. From the File menu, select **Build Settings**, and set
    the Platform to **Windows Store**, SDK to **Universal 10**,
    and Build Type to **XAML**.
    Click **Build** and choose a folder that will receive the
    generated C# project.

11. Open the generated csproj file in a text editor to fix an error
    in Unity's code generation: Search for
    the lines

    <TargetPlatformVersion>10.0.N.0.0</TargetPlatformVersion>

   and

    <SDKReference Include="WindowsMobile, Version=10.0.N.0.0">

    (The value of N will depend on the SDK you are using.)
    Delete the extra .0:

    <TargetPlatformVersion>10.0.N.0</TargetPlatformVersion>

    <SDKReference Include="WindowsMobile, Version=10.0.N.0">

# Build and run the sample

You can now open the .sln file in Visual Studio
and build the sample.
When you run the sample,
click the **3D Print** button to launch the Windows 10
3D Print dialog.
From this dialog, you can order your model online or send
it to a local printer.

# Debugging the sample

From the Modules window,
choose Assembly-CSharp.dll.
Find the symbol from your solution folder and load it.
Within your script, set your breakpoints. 

Additionally, you can inspect the [3MF file](http://3mf.io/ "3MF")
that has been generated using the commented function in Print3D.cs.


## Related topics

### Reference

* [3D Printing in Windows](https://www.microsoft.com/3d)

* [Windows.Graphics.Printing3D namespace](https://msdn.microsoft.com/library/windows/apps/windows.graphics.printing3d.aspx)
* [Print3DManager class](https://msdn.microsoft.com/library/windows/apps/windows.graphics.printing3d.print3dmanager.aspx) launches the print dialog
* [Printing3D3MFPackage class](https://msdn.microsoft.com/library/windows/apps/windows.graphics.printing3d.printing3d3mfpackage.aspx).

## System requirements

**Client:** Windows 10 Version 1511

**Phone:** Windows 10 Version 1511
