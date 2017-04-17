<!---
  category: ControlsLayoutAndText
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620618
--->

# Disabling selection sample

Shows how your app can make content areas in your Windows Store app using JavaScript unselectable using the 
[-ms-user-select](http://msdn.microsoft.com/library/windows/apps/hh779846) CSS attribute.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

By default, all content in the UI of a Windows Store app using JavaScript can be selected by a user and copied to the clipboard. 
However, access to UI elements (such as text, images, and other proprietary content) can be limited by excluding them from this default 
behavior with [-ms-user-select](http://msdn.microsoft.com/library/windows/apps/hh779846).

[-ms-user-select](http://msdn.microsoft.com/library/windows/apps/hh779846) supports the following values:

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<thead>
<tr class="header">
<th align="left">Term
Description</th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td align="left"><p>none</p>
<p>Blocks selection from starting on that element. It will not block an existing selection from entering the element.</p></td>
<td align="left"><p>element</p>
<p>Enables selection to start within the element; however, the selection is contained by the bounds of that element.</p></td>
</tr>
</tbody>
</table>

Related topics
--------------

[How to disable text and image selection](http://go.microsoft.com/fwlink/p/?linkid=272182)  

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

