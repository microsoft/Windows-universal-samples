<!---
  category: AudioVideoAndCamera
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620020
--->

# Video playback sample

Shows how to create apps that take advantage of many media platform features.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample covers:

- Playing local video
- Using in-band closed captions
- Using out-of-band closed captions
- Dealing with multiple video tracks
- Dealing with multiple audio tracks
- Creating video playlists
- Playing on a composition surface (not available to JS apps)
- Transferring a MediaPlayer from one MediaPlayerElement to another (XAML-only)

## Related topics

### Samples

[VideoPlaybackSynchronization sample](/Samples/VideoPlaybackSynchronization)  
[XamlCustomMediaTransportControls sample](/Samples/XamlCustomMediaTransportControls)  

### Reference

[Windows.Media.Playback namespace](https://msdn.microsoft.com/library/windows/apps/windows.media.playback.aspx)  

## System requirements

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

