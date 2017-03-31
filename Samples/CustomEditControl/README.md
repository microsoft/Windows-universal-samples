<!---
  category: ControlsLayoutAndText
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=830671
-->

# Custom Edit Control sample

Shows how to use the CoreTextEditContext class in the Windows.UI.Text.Core namespace
to create a rudimentary text control. Note that this text control is not complete;
it glosses over many details that would be necessary for a complete text edit control.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

The focus of the sample is in CustomEditControl.cs to show how to implement and manage a text control.
There is much more functionality built in to the CoreTextEditContext that is not covered in this sample.

This sample demonstrates the following:

* Managing the text and current selection of a custom edit control.
  This sample uses a simple string to record the text.
* Rendering the text and current selection in the custom edit control.
  To illustrate that the app is completely responsible for the visual
  presentation of the control, this sample takes the unusual decision
  to use a globe to represent the caret.
* Manually setting focus to and removing focus from the control
  and the CoreTextEditContext based on application-defined criteria.
  The CoreTextEditContext processes text input when it has focus.
* Setting the CoreTextEditContext.InputPaneDisplayPolicy to Manual
  and manually showing the software keyboard when the custom edit control
  gains focus and hiding it when the custom edit control loses focus.
* Responding to system events that request information about the
  custom edit control or request changes to the text and selection of
  the custom edit control.
* Responding to layout information requests so that the IME candidate window
  can be positioned properly.
* Changing the selection and/or moving the caret when the user presses
  an arrow key, and deleting text when the user presses the Backspace key.

**Instructions on using this sample**

* Click or tap on the custom edit control to give it focus,
  and click or tap outside the custom edit control to remove focus.
* Observe that the Input Pane appears (if applicable)
  when the custom edit control gains focus,
  and it disappears when the custom edit control loses focus.
* Use the arrow keys to move the caret (shown as a globe).
* Hold the shift key when pressing the arrow keys to adjust
  the selection.
* Use the Backspace key to delete text.
* To demonstrate support for IME candidates:
  * Install an IME by going to Settings, Time and Language,
    Region and language. Click "Add a language" and select
    Chinese (Simplified) "中文（中华人民共和国）".
  * Set your input language to Chinese by using the language
    selector in the bottom right corner of the taskbar (on Desktop)
    or by swiping the space bar on the software keyboard (on Mobile).
  * Put focus on the custom edit control and start typing.
    IME candidate suggestions will appear as you type.

**Features missing from this sample**

* This sample does not properly handle surrogate pairs
  or grapheme clusters.
* This sample does not support common keyboard shortcuts
  such as Home and End, nor does it support shortcuts such
  as Ctrl+V to paste.
* This sample does not show a context menu if the user right-clicks
  or performs a press-and-hold gesture.
* This sample does not support using the mouse or touch to
  move the caret or adjust the selection.

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.

To obtain information about Windows 10 development, go to the [Windows Dev Center](https://dev.windows.com)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples

[Unicode](/Samples/Unicode)  

### Reference

[CoreTextEditContext](https://msdn.microsoft.com/library/windows/apps/windows.ui.text.core.coretexteditcontext.aspx)  

### Conceptual

[Custom text input](https://msdn.microsoft.com/windows/uwp/input-and-devices/custom-text-input)  
[Unicode Standard Annex #29 "Unicode Text Segmentation"](http://www.unicode.org/reports/tr29/)  

## System requirements

**Client:** Windows 10 

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10 

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio?2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 
