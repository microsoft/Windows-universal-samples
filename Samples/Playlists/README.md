<!--
  category: AudioVideoAndCamera
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=624039
-->

# Playlists sample

This sample demonstrates how to create, save, display, and edit a playlist of audio files.

This sample uses classes that are in the [**Windows.Media.Playlists**](http://msdn.microsoft.com/library/windows/apps/br206938) namespace. It provides five scenarios:

1.  **Create and save a playlist from a set of audio files.**

    This creates a new [**Playlist**](http://msdn.microsoft.com/library/windows/apps/br206904) object and saves it using [**SaveAsAsync**](http://msdn.microsoft.com/library/windows/apps/hh768260).

2.  **Display the contents of an existing playlist (WPL, ZPL, M3U).**

    This loads a [**Playlist**](http://msdn.microsoft.com/library/windows/apps/br206904) object using [**LoadAsync**](http://msdn.microsoft.com/library/windows/apps/br206934), and obtains the properties of each file in [**Playlist.Files**](http://msdn.microsoft.com/library/windows/apps/br206933) using [**Windows.Storage.FileProperties.StorageItemContentProperties.GetMusicPropertiesAsync**](http://msdn.microsoft.com/library/windows/apps/hh770649).

3.  **Insert an item at the end of an existing playlist (WPL, ZPL, M3U).**

    This loads a [**Playlist**](http://msdn.microsoft.com/library/windows/apps/br206904) object using [**LoadAsync**](http://msdn.microsoft.com/library/windows/apps/br206934), and appends a file to [**Playlist.Files**](http://msdn.microsoft.com/library/windows/apps/br206933).

4.  **Remove an item from the end of an existing playlist (WPL, ZPL, M3U).**

    This loads a [**Playlist**](http://msdn.microsoft.com/library/windows/apps/br206904) object using [**LoadAsync**](http://msdn.microsoft.com/library/windows/apps/br206934), and removes the file at the end of [**Playlist.Files**](http://msdn.microsoft.com/library/windows/apps/br206933).

5.  **Remove all items from an existing playlist (WPL, ZPL, M3U).**

    This loads a [**Playlist**](http://msdn.microsoft.com/library/windows/apps/br206904) object using [**LoadAsync**](http://msdn.microsoft.com/library/windows/apps/br206934), and clears the files in [**Playlist.Files**](http://msdn.microsoft.com/library/windows/apps/br206933).

**Note** The Universal Windows app samples require Visual Studio 2015 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Reference

[Playlists namespace](http://msdn.microsoft.com/library/windows/apps/br206938)

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Not supported

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2015 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio 2015 Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or select Debug > Start Without Debugging. 
