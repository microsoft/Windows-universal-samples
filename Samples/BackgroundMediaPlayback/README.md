<!---
  category: AudioVideoAndCamera
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=800141
--->

# Background media playback sample

This sample demonstrates how to use `MediaPlayer` and `MediaPlaybackList`
to create a collection of songs or videos
that can continue to play even when the app is no longer in the foreground.

Specifically, this sample covers:

-   Enabling background media playback through a manifest capability
-   Playing audio and video in the background with the `MediaPlayer` API
-   Gapless playback with `MediaPlaybackList`
-   Automatic `SystemMediaTransportControl` integration
-   Update of `MediaPlaybackItem` DisplayProperties
-   MVVM for media player apps
-   JSON playlist serialization

There are approaches for performing background audio playback
for earlier versions of Windows,
such as using `BackgroundMediaPlayer` in a dual-process application
or `AudioCategory.BackgroundCapableMedia`
in a single-process desktop application,
but this sample demonstrates the recommended
single-process technique for background playback
on all supported UWP platforms.

Enabling background media playback
----------------------------------

To enable background media playback,
add the capability to the `Package.appxmanifest`.
Note the `uap3` namespace.

```
<Package
  xmlns="http://schemas.microsoft.com/appx/manifest/foundation/windows10"
  xmlns:mp="http://schemas.microsoft.com/appx/2014/phone/manifest"
  xmlns:uap="http://schemas.microsoft.com/appx/manifest/uap/windows10"
  xmlns:uap3="http://schemas.microsoft.com/appx/manifest/uap/windows10/3"
  IgnorableNamespaces="uap uap3 mp">
   ...
  <Capabilities>
    <uap3:Capability Name="backgroundMediaPlayback"/>
  </Capabilities>
</Package>
``` 

In a future update it will be possible to enable this
through the form-based manifest editor in Visual Studio.

Adding this capability enables the following:

* This changes lifecycle behavior to keep the app process from suspending
as long as it plays audio.
All streams become background capable so they don't mute.
 
* All audio APIs become background enabled.
That means you can use any platform audio APIs,
such as MediaPlayer, AudioGraph, XAudio2, and the HTML Audio tag.

Background logic
----------------
To perform background logic when not playing audio,
use Extended Execution or `ApplicationTrigger`.
Handling triggers and running background tasks
inside a single process
can be done through the `OnBackgroundActivated` handler.
See the
[BackgroundActivation sample](/Samples/BackgroundActivation)
for details.

Lifecycle
---------
* Over limit policy:
Instead of terminating the app immediately,
apps continue to run despite being over target provided that
the system doesn't need the memory.
Apps can improve their priority by keeping usage low when backgrounded.

* Apps can use CoreApplication lifecycle events
`EnteredBackground` and `LeavingBackground`
as a trigger to free memory if needed.

* Apps can use the `MemoryManager` class
to monitor their memory usage.
In particular, apps can reduce their memory usage when the
`AppMemoryUsageLevel` reaches `OverLimit`.
If needed, apps can unload UI by setting Window.Current.Content to
null and unregistering event handlers.

* When the app enters the background
or receives an over limit notification,
platform frameworks will take steps to release unused resources,
such as cached textures.

MediaPlayer API
---------------
* MediaPlayer works in foreground and background apps,
with UI or without.
All key media features from MediaElement are available to MediaPlayer.
Now you can "new MediaPlayer()" anywhere.

* A lightweight MediaPlayerElement allows binding and unbinding
to a player for displaying video.
This design enables support for background video
since rendering to an element is decoupled from the player.
Additionally, background audio apps can leverage
its platform `MediaTransportControls` if desired.

* `MediaPlayer` has a `MediaPlayerSurface`
that can be used to render video to a Windows.UI.Composition surface.
This enables media playback in "framework-less apps".
Furthermore,
since all XAML elements are backed by these surfaces,
apps can render video to any XAML element.

* `MediaPlayer` connects to `SystemMediaTransportControls`
through a `CommandManager`.
These controls are important
because that's how the app responds to hardware button presses and Bluetooth,
and they are required for background audio.

* The app can popupate the DisplayProperties of the MediaPlaybackItem
with artwork, track title, etc.,
and the platform will update system controls automatically
when the item is playing.

Related topics
--------------
* [BackgroundActivation sample](/Samples/BackgroundActivation)
* Old [BackgroundAudio sample](http://go.microsoft.com/fwlink/p/?LinkId=619997),
  no longer recommended but available for reference.

System requirements
-----------------------------

**Client:** Windows 10 version 14332

**Phone:** Windows 10 version 14332

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
