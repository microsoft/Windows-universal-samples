<!---
  category: AudioVideoAndCamera 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620567
--->

# Media editing sample

Shows how to use the APIs in the **Windows.Media.Editing** namespace to edit and compose media clips. The tasks 
demonstrated in these scenarios include opening, trimming, and saving videos, appending multiple video clips together into a 
single composition, adding background audio, and using overlays to composite videos.  

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

**Scenario 1: Trim and Save Clip:**    
Press the *Choose Video* button to launch a file open picker that allows you to choose a video file to open. Press the 
*Trim Video* button to trim 25% of the videos duration from the beginning and end of the clip . Press play on the video controls 
to view the trimmed video clip. Press the *Save Result* button to launch a file save picker that allows you to choose a 
location and file name with which to save the trimmed video clip.  

In the code behind, when the file open picker is launched and a file is selected, the file is assigned to a **MediaElement** 
defined in the XAML file with a call to using the **SetSource** to make it viewable in the UI.   

When the *Trim Video* button is pressed, a new **MediaClip** object is created from the video file with a call to 
**MediaClip.CreateFromFileAsync**. The **TrimTimeFromStart** and **TrimTimeFromEnd** methods of the **MediaClip** class are called 
to trim the duration of the clip. Next, a new **MediaComposition** object is created and the trimmed clip is appended to the 
composition's **Clips** collection. A **MediaStreamSource** is created using the **MediaComposition** method **GeneratePreviewMediaStreamSource** 
and then the **MediaElement* method **SetMediaStreamSource** to make the composition viewable in the UI.    

When the *Save Result* button is pressed, a **FileSavePicker** is created to allow the user to pick a destination file. The
**MediaComposition** method **RenderToFileAsync** saves the composition to the selected file. 

**Scenario 2: Append Clips:**  
Press the *Choose First Video* button to launch a file open picker that allows you to choose a video file to open. Press the 
*Choose Second Video* button to select a second video. Press the *Append Videos* button to append the videos together into a 
single composition. Press play on the video controls to view the trimmed appended clips.     

In the code behind, when each file picker is launched, the resulting file is assigned to the **MediaElement** defined in the
XAML file with the **SetSource** method, to make the video files viewable in the UI. When the *Append Videos* button is
pressed, a new **MediaClip** object is created for each of the video files. Next, a new **MediaComposition** object is created
and each media clip is added to the composition's **Clips** collection. Finally, a **MediaStreamSource** is created using 
the **MediaComposition** method **GeneratePreviewMediaStreamSource** and then the **MediaElement** method **SetMediaStreamSource** 
is called to make the composition is viewable in the UI.  

**Scenario 3: Add Audio Tracks**  
 Press the *Choose Video* button to launch a file open picker that allows you to choose a video file to open. Press the 
 *Add Background Audio* button to launch a file open picker that allows you to choose a audio file to open. Press play 
 on the video controls to view the video clip with the added background audio.  

In the code behind, when the file open picker is launched and a video file is selected, the file is assigned to a **MediaElement** 
defined in the XAML file with a call to using the **SetSource**. When the *Add Background Audio* button is pressed, a new 
**MediaClip** is created from the selected video file, a new **MediaComposition** is created, and the **MediaClip** is added to 
the composition's **Clips** collection. Next the file picker for the audio file is launched. When the file picker returns, 
a new **BackgroundAudioTrack** is created from the selected audio file by calling **CreateFromFileAsync**. The **BackgroundAudioTrack**
is added to the composition's *BackgroundAudioTracks* collection. Finally, a **MediaStreamSource** is created using the 
**MediaComposition** method **GeneratePreviewMediaStreamSource** and then the **MediaElement** method **SetMediaStreamSource** is 
called to make the composition viewable in the UI.

**Scenario 4: Add Overlays** 
Press the *Choose Base Video* button to launch a file open picker that allows you to choose a video file to open. Press the 
*Choose Overlay Video* button to select a second video. Press play on the video controls to view the overlayed video clip. 

In the code behind, when each file picker is launched, the resulting file is assigned to the **MediaElement** defined in the
XAML file with the **SetSource** method, to make the clips viewable in the UI. To create the overlay, first a new
**MediaClip** is created from the base video clip, a new **MediaComposition** is created, and the base media clip is added to
the composition's **Clips** collection. Next, a new **MediaClip** is created for the overlay video clip. The **GetVideoEncodingProperties**
method is called to get information about the format of the clip. A **Rect** to specify the placement of the overlay is created,
using the encoding properties to make sure the aspect ratio of the clip is preserved. Next, a **MediaOverlay** object is created
from the clip. The **Position** of the overlay is set using the **Rect** structure and the **Opacity** of the overlay is set to 75%.
A new **OverlayLayer** is created, and the **MediaOverlay** is added to its **Overlays** collection. Next the **OverlayLayer** is added
to the composition's **OverlayLayers** collection. Finally, a **MediaStreamSource** is created using the **MediaComposition** method 
**GeneratePreviewMediaStreamSource** and then the **MediaElement** method **SetMediaStreamSource** is called to make the composition
viewable in the UI. 


Related topics
--------------

[Windows.Media.Editing namespace](https://msdn.microsoft.com/library/windows/apps/windows.media.editing.aspx)  

System requirements
-----------------------------

**Client:** Windows 10

**Phone:** Windows 10

Build the sample
----------------

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

Run the sample
--------------

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

**Deploying the sample**
1.  Select **Build** \> **Deploy Solution**.

**Deploying and running the sample**
1.  To debug the sample and then run it, press F5 or select **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or select**Debug** \> **Start Without Debugging**.

