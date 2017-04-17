<!---
  category: AudioVideoAndCamera 
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620569
--->

# Transcoding media sample
  
Shows how to use the [Windows.Media.Transcoding](http://msdn.microsoft.com/library/windows/apps/br207105) API to transcode a video file in a Windows Store app.. Transcoding is the conversion of a digital media file, such as a video or audio file, from one format to another. For example, you might convert a Windows Media file to MP4 so that it can be played on a portable device that supports MP4 format. Or, you might convert a high-definition video file to a lower resolution. In that case, the re-encoded file might use the same codec as the original file, but it would have a different encoding profile.  
  
> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample covers the following scenarios:  
  
-   Converting a video file from one resolution to another resolution.  
-   Converting a video file using a custom output format.  
-   Creating a clip from a video using the [TrimStart](http://msdn.microsoft.com/library/windows/apps/br207103) and [TrimStop](http://msdn.microsoft.com/library/windows/apps/br207104) API.  
  
Some of the transcode API covered in this sample are:  
  
-   [Windows.Media.Transcoding](http://msdn.microsoft.com/library/windows/apps/br207105)  
-   [MediaTranscoder](http://msdn.microsoft.com/library/windows/apps/br207080)  
-   [MediaTranscoder.PrepareFileTranscodeAsync](http://msdn.microsoft.com/library/windows/apps/hh700936)  
-   [PrepareTranscodeResult](http://msdn.microsoft.com/library/windows/apps/hh700941)  
-   [PrepareTranscodeResult.TranscodeAsync](http://msdn.microsoft.com/library/windows/apps/hh700946)  
-   [MediaProperties.MediaEncodingProfile](http://msdn.microsoft.com/library/windows/apps/hh701026)  
-   [TrimStart](http://msdn.microsoft.com/library/windows/apps/br207103)  
-   [TrimStop](http://msdn.microsoft.com/library/windows/apps/br207104)  
  
For more info about transcoding video files in Windows Store apps, see [Transcode media files](https://msdn.microsoft.com/library/windows/apps/mt282149) and [Transcode media files](https://msdn.microsoft.com/library/windows/apps/mt282149).  

## Related topics  
  
[Windows 8 app samples](http://go.microsoft.com/fwlink/p/?LinkID=227694)  
  
**Roadmaps**  
  
[Roadmap for apps using C\# and Visual Basic](http://msdn.microsoft.com/library/windows/apps/br229583)  
[Roadmap for apps using JavaScript](http://msdn.microsoft.com/library/windows/apps/hh465037)  
[Roadmap for apps using C++](http://msdn.microsoft.com/library/windows/apps/hh700360)  
[Designing UX for apps](http://msdn.microsoft.com/library/windows/apps/hh767284)  
[Audio, video, and camera](https://msdn.microsoft.com/library/windows/apps/mt203788)  
  
**Tasks**  
  
[Transcode media files](https://msdn.microsoft.com/library/windows/apps/mt282149)  

**Reference**  
  
[Windows.Media.Transcoding](http://msdn.microsoft.com/library/windows/apps/br207105)  
[MediaTranscoder](http://msdn.microsoft.com/library/windows/apps/br207080)  
[MediaTranscoder.PrepareFileTranscodeAsync](http://msdn.microsoft.com/library/windows/apps/hh700936)  
[PrepareTranscodeResult](http://msdn.microsoft.com/library/windows/apps/hh700941)  
[PrepareTranscodeResult.TranscodeAsync](http://msdn.microsoft.com/library/windows/apps/hh700946)  
[MediaProperties.MediaEncodingProfile](http://msdn.microsoft.com/library/windows/apps/hh701026)  
[TrimStart](http://msdn.microsoft.com/library/windows/apps/br207103)  
[TrimStop](http://msdn.microsoft.com/library/windows/apps/br207104)  
  
## System requirements

**Client:** Windows 10

**Phone:** Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

**Deploying the sample**
1.  Select **Build** \> **Deploy Solution**.

**Deploying and running the sample**
1.  To debug the sample and then run it, press F5 or select **Debug** \> **Start Debugging**. To run the sample without debugging, press Ctrl+F5 or select**Debug** \> **Start Without Debugging**.