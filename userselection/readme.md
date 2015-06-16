Unselectable content areas with -ms-user-select CSS attribute sample
====================================================================

This sample demonstrates how your app can make content areas in your Windows Store app using JavaScript unselectable using the [**-ms-user-select**](http://msdn.microsoft.com/library/windows/apps/hh779846) CSS attribute.

By default, all content in the UI of a Windows Store app using JavaScript can be selected by a user and copied to the clipboard. However, access to UI elements (such as text, images, and other proprietary content) can be limited by excluding them from this default behavior with [**-ms-user-select**](http://msdn.microsoft.com/library/windows/apps/hh779846).

[**-ms-user-select**](http://msdn.microsoft.com/library/windows/apps/hh779846) supports the following values:

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

1.  Start Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
2.  Go to the directory in which you unzipped the sample. Go to the directory named for the sample, and double-click the Visual Studio 2015 Solution (.sln) file.
3.  Press F7 or use **Build** \> **Build Solution** to build the sample.

Run the sample
--------------

To debug the app and then run it, press F5 or use **Debug** \> **Start Debugging**. To run the app without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.

