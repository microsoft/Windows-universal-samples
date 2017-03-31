<!---
  category: AudioVideoAndCamera
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=800141
--->

# Background media playback sample

Shows how to use MediaPlayer and MediaPlaybackList
to create a collection of songs or videos that can continue to play even
when the app is no longer in the foreground.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

Specifically, this sample covers:

- Enabling background media playback through a manifest capability
- Playing audio and video in the background with the MediaPlayer API
- Gapless playback with MediaPlaybackList
- Automatic SystemMediaTransportControl integration
- Update of MediaPlaybackItem DisplayProperties
- MVVM for media player apps
- JSON playlist serialization

Other relevant application model considerations include:

- Background applications have a memory target

- There are lifecycle events to inform apps if over target

- Apps can respond to being over target by unloading resources, including views
  or view state to get under target

- If an app needs to continue executing in the background when not playing media
  it must use Extended Execution, background tasks, or other supported mechanism
  to sponsor execution

- If an app needs to make networking calls in the background when not
  downloading or streaming media using platform media APIs these must be wrapped
  in either a foreground initiated Extended Execution session or a background
  task like ApplicationTrigger, MaintenanceTrigger, or TimerTrigger.
  Otherwise the network may be unavailable in [standby](https://msdn.microsoft.com/library/windows/hardware/mt282515.aspx).

See the [Background Activity With the Single Process Model](https://blogs.windows.com/buildingapps/2016/06/07/background-activity-with-the-single-process-model/)
blog post for more information.

There are approaches for performing background audio playback for earlier
versions of Windows, such as using BackgroundMediaPlayer in a dual-process
application or AudioCategory.BackgroundCapableMedia in a single-process
desktop application, but this sample demonstrates the recommended single-process
technique for background playback on all supported UWP platforms.

Enabling background media playback
----------------------------------

To enable background media playback, add the capability to the
Package.appxmanifest. Note the uap3 namespace.

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

In a future update it will be possible to enable this through the form-based
manifest editor in Visual Studio.

Adding this capability enables the following:

* This changes lifecycle behavior to keep the app process from suspending as
long as it plays audio. All streams become background capable so they don't
mute.

* All media playback APIs become background enabled. That means you can use any
platform audio APIs, such as MediaPlayer, AudioGraph, XAudio2, and the HTML
Audio tag.

MediaPlayer provides a default SystemMediaTransportControls implementation.
If using another media API or if the MediaPlayer.CommandManager is disabled
then an application must also minimally:

1. Enable SystemMediaTransportControls by setting IsEnabled to true
2. Set IsPlayEnabled and IsPauseEnabled to true
3. Handle the corresponding ButtonPressed events

Background logic
----------------
To perform background logic when not playing audio, use a foreground initiated
Extended Execution session or a background or foreground initiated
ApplicationTrigger. Handling triggers and running background tasks inside a
single process can be done through the OnBackgroundActivated handler. See the
[BackgroundActivation sample](/Samples/BackgroundActivation) for details.

Lifecycle
---------
* Over limit policy: Apps must handle OnAppMemoryUsageLimitChanging to reduce
memory usage if over the new limit about to be enforced. Instead of terminating
the app immediately, apps continue to run on some platforms despite being over
target provided that the system doesn't need the memory. Apps can improve their
priority by keeping usage low when backgrounded and can maximize the range of
devices supported by ensuring the application stays under the limit.

* Apps can use CoreApplication lifecycle events EnteredBackground and
LeavingBackground as a trigger to free memory used by a GUI if needed since
the GUI is not displayed in the background. If needed, apps can release GUI
resources by setting Window.Current.Content to null and unregistering event
handlers.

* Apps can use the MemoryManager class to monitor their memory usage after a
new limit has been applied. In particular, apps can reduce their memory usage
when the AppMemoryUsageLevel reaches High or OverLimit.

* When the app enters the background or receives an over limit notification,
platform frameworks will take steps to release unused resources, such as cached
textures.

MediaPlayer API
---------------
* MediaPlayer works in foreground and background apps, with UI or without. All
key media features from MediaElement are available to MediaPlayer. Now you can
"new MediaPlayer()" anywhere.

* A lightweight MediaPlayerElement allows binding and unbinding to a player for
displaying video. This design enables support for background video since
rendering to an element is decoupled from the player. Additionally, background
audio apps can leverage its platform MediaTransportControls if desired.

* MediaPlayer has a MediaPlayerSurface that can be used to render video to a
Windows.UI.Composition surface. This enables media playback in "framework-less
apps". Furthermore, since all XAML elements are backed by these surfaces, apps
can render video to any XAML element.

* MediaPlayer connects to SystemMediaTransportControls through a
CommandManager. These controls are important because that's how the app
responds to hardware button presses and Bluetooth, and they are required for
background audio.

* The app can popupate the DisplayProperties of the MediaPlaybackItem with
artwork, track title, etc., and the platform will update system controls
automatically when the item is playing.

Memory management guidance for background audio apps
----------------------------------------------------

### Background information

There are three events for controlling your memory usage (but only one is mandatory):

 * **MemoryManager::AppMemoryLimitChanging** – Raised just before the limit of how
   much total memory the app can use is changed. For example, when the app moves
   into the background and on Xbox the memory limit changes from 1024MB to
   128MB. This is the most important event to handle to keep the platform from
   suspending or terminating the app.
 
 * **MemoryManager::AppMemoryUsageIncreased** – Raised when the app's memory
   consumption has increased to a higher value in the AppMemoryUsageLevel
   enumeration. For example, from Low to Medium. Handling this event is optional
   but recommended since the application is still responsible for staying under
   limit.

 * **MemoryManager::AppMemoryUsageDecreased** – Raised when the app's memory
   consumption has decreased to a higher lower value in the
   AppMemoryUsageLevel enumeration. For example, from High to Low. Handling
   this event is optional but indicates the application may be able to allocate
   additional memory if needed.

There are two events to take into account when your app is entering and
leaving the background:

 * **WebUIApplication/Application::EnteredBackground** – Raised when the app has
   moved from the foreground to the background.

 * **WebUIApplication/Application::LeavingBackground** – Raised when the is
   about to move from the background to the foreground.
 
### Managing memory using the AppMemoryLimitChanging event 

The AppMemoryLimitChanging event notifies an application that the memory limit
is about to change. The NewLimit property on the
AppMemoryUsageLimitChangingEventArgs can be used to determine whether the
current app memory usage (MemoryManager::AppMemoryUsage) is over the new limit
or not. If app memory usage is over the new limit, the app must free resources
or risk being suspended or terminated when the new memory limit comes into
effect.

Note: 
 - Some device configurations will allow an application to continue running over
   the new memory limit until the system is under resource pressure.

 - On Xbox in particular, apps will be suspended or terminated if they do not
   reduce memory to under the new limits within 2 seconds.

 - This means that apps can deliver the best experience across the broadest
   range of devices by using this event to reduce resource usage below the limit
   within 2 seconds of the event being raised.

### Managing memory using the AppMemoryUsageIncreased and AppMemoryUsageDecreased events

When running in the foreground or the background the AppMemoryUsageIncreased
and AppMemoryUsageDecreased events can be used control the amount of memory an
app uses. For example:

 - if an app gets an AppMemoryUsageIncreased event and the
   MemoryManager::AppMemoryUsageLevel is AppMemoryUsageLevel::High then it
   should consider freeing resources to stay within the memory constraints of
   the system.

 - if an app gets an AppMemoryUsageIncreased event and the
   MemoryManager.AppMemoryUsageLevel is AppMemoryUsageLevel.OverLimit then
   it must free resources to stay within the memory constraints of the system.
   If it does not the app may be suspended or terminated (see Notes above for
   more information).  On Xbox if the app does not reduce memory within 2
   seconds of receiving an OverLimit event it will be suspended or terminated.

 - The MemoryManager::AppMemoryUsage and MemoryManager::AppMemoryUsageLimit
   properties can be used to determine actual memory usage and the current
   limit. Note: These events are not a replacement handling the
   AppMemoryLimitChanging event, but rather allow the application to continue
   to monitor its memory usage after limits have been applied.

### Freeing resources

An app may have resources such as cached data that can be released at any point
in time and recreated easily. These are ideal to release in
AppMemoryLimitChanging or as needed in AppMemoryUsageIncreased.

Some resources cannot be released while the UI is present, such as visible
images, 3D models, or the view itself and its backing data. These can be
released directly in EnteredBackground but this could result in releasing
views unnecessarily. Alternatively, use EnteredBackground/LeavingBackground
to track background status and then release UI resources from
AppMemoryLimitChanging only if needed.

Be especially careful of references that could prevent resources from being
garbage collected, such as strong references or subscribed event handlers.

If your application clears window content on background transitions, be aware
of the following.

When the window content is set to be collected, each Frame will begin its
disconnection process. If there are Pages in the visual object tree under the
window content, these will raise their Unloaded event. Pages cannot be
completely cleared from memory unless all references to them are removed. In the
Unloaded callback, make sure to do these things to ensure memory is quickly
removed:

 ✓ **Do** clear and set any large data structures in your Page to null.

 ✓ **Do** unregister for all event handlers that have callback methods within
   the Page. Make sure to Register for those callbacks again during the Loaded
   event handler for the Page. The Loaded event will be raised when the UI has
   been reconstituted later and the Page has been added to the visual object
   tree.

 ✓ **Do** call GC.Collect at the end of the Unloaded callback to quickly garbage
   collect any of the large data structures you have just set to null.

### Moving from the foreground to the background

When an app moves from the foreground to the background, the system does work on
behalf of the app to free up resources that are not needed in the background.
For example, the UI frameworks flush cached textures and the video subsystem
frees memory allocated on behalf of the app. However, an app will still need to
carefully monitor its memory usage to avoid being suspended or terminated by the
system.

When an app moves from the foreground to the background it will first get an
EnteredBackground event and then a AppMemoryLimitChanging event.

 ✓ **Do** use the EnteredBackground event to free up UI resources that you know
 your app does not need when running in the background – for example the cover
 art image for a song.

 ✘ **Do not** perform long running operations in the EnteredBackground event
 as you can cause the system to appear to be slow to transition between
 applications as a result.

 ✓ **Do** use the AppMemoryLimitChanging event to ensure that your app is
 using less memory than the new background limit. Make sure that you free up
 resources if this is not the case. If you do not your app may be suspended or
 terminated according to device specific policy.

 ✓ **Do** manually invoke the garbage collector if your app is over the new
 memory limit when the AppMemoryLimitChanging event is raised.

 ✓ **Consider** as a performance optimization, freeing UI resources in the
 AppMemoryLimitChanging event handler instead of in the EnteredBackground
 handler. Use a boolean value set in the EnteredBackground/LeavingBackground
 event handlers to track whether the app is in the background or foreground.
 Then in the AppMemoryLimitChanging event handler, if AppMemoryUsage is over
 the limit and the app is in the background (based on the Boolean value) you can
 free UI resources.

 ✓ **Do** use the AppMemoryUsageIncreased event to continue to monitor your
 app’s memory usage while running in the background if you expect it to change.
 If the AppMemoryUsageLevel is High or OverLimit make sure that you free
 up resources.

### Moving from the background to the foreground

When an app moves from the background to the foreground, the app will first get
an AppMemoryLimitChanging event and then a LeavingBackground event.

 ✓ **Do** use the LeavingBackground event to recreate UI resources that your app
 discarded when moving into the background.

Related topics
--------------
* [Background Activity With the Single Process Model](https://blogs.windows.com/buildingapps/2016/06/07/background-activity-with-the-single-process-model/)
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
