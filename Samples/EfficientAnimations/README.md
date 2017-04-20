<!---
  category: GraphicsAndAnimation
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620541
--->

# Efficient animations (HTML) sample

Shows how to use the [requestAnimationFrame](http://msdn.microsoft.com/library/windows/apps/hh920765) method in an HTML5 Canvas to build smooth 
and power efficient animations in your Windows Store app using JavaScript.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

The [requestAnimationFrame](http://msdn.microsoft.com/library/windows/apps/hh920765) API is the best way to do non-declarative animations in a power efficient and smooth way. This API is similar to the [setTimeout](http://msdn.microsoft.com/library/windows/apps/hh453490) and [setInterval](http://msdn.microsoft.com/library/windows/apps/hh453487) APIs that developers use today, except that it notifies the application when it needs to update the screen, and only when it needs to update the screen. It keeps Windows Store applications using JavaScript perfectly aligned with the painting of the window, and uses only the necessary amount of graphics resources (which is ideal for low power settings and Connected Standby devices). This API will take page visibility and the display refresh rate into account to determine how many frames per second to allocate to the animations.

Until now, HTML and JavaScript did not provide an efficient means for web developers to schedule graphics timers for animations. Animations are commonly overdrawing – causing choppier animations and increased power consumption. Further efficiency is lost as animations are drawn without knowledge of whether the page is visible to the user. For example, most animations use a timer period of less than 16.7ms to draw animations, even though most monitors can only display at 60Hz frequency or 16.7ms periods.

For example, using [setTimeout](http://msdn.microsoft.com/library/windows/apps/hh453490) with a 10ms period results in every third callback not being painted, as another callback occurs prior to the display refresh. This overdrawing results in not only choppy animations, as every third frame being lost, but an overall increased resource consumption.

In this example, the clock is animated using HTML5 Canvas and the [requestAnimationFrame](http://msdn.microsoft.com/library/windows/apps/hh920765) API.

Related topics
--------------

[requestAnimationFrame](http://msdn.microsoft.com/library/windows/apps/hh920765)  

Operating system requirements
-----------------------------

Client

Windows 10

Build the sample
----------------

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

Run the sample
--------------

To debug the app and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the app without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.

