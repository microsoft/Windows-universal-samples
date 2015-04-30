# Transform3D animations sample

This sample shows how Transform3D can be used to create custom 3D animations and transitions in XAML apps.

Specifically, two effects are demonstrated in this sample:

- **3D rotation animation:** In the main page for this app, the columns are animated with a custom 3D rotation animation when content changes, including a 3D shadowing effect. See the `SectionView` class for an implementation of this effect.
- **3D staggered zoom animation:** When the user navigates to a specific item, a staggered zoom effect is used to announce the entrance of content. The `ArticlePage` class demonstrates how to create this effect.

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10, go to [Windows 10](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples

[Transform3D Parallax](https://github.com/Microsoft/Windows-universal-samples/tree/master/xaml_transform3dparallax)

### Reference

[UIElement.Transform3D](https://msdn.microsoft.com/en-us/library/windows/apps/windows.ui.xaml.uielement.transform3d.aspx)

[PerspectiveTransform3D](https://msdn.microsoft.com/en-us/library/windows/apps/windows.ui.xaml.media.media3d.perspectivetransform3d.aspx)

[CompositeTransform3D](https://msdn.microsoft.com/en-us/library/windows/apps/windows.ui.xaml.media.media3d.compositetransform3d.aspx)

## System requirements

**Client:** Windows 10 Insider Preview

**Server:** Windows 10 Insider Preview

**Phone:**  Windows 10 Insider Preview

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
