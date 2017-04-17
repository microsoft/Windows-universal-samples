<!---
  category: AudioVideoAndCamera
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620479
-->

# Adaptive streaming sample

Shows various features of the AdaptiveMediaSource object,
used in conjunction with MediaSource, MediaPlaybackItem, MediaPlayer and MediaPlayerElement.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

This sample demonstrates the following:

**Using a simple adaptive streaming URI**

In XAML, a Uri in the Source property of a MediaPlayerElement
goes through a format converter which calls MediaSource.CreateFromUri.
An AdaptiveMediaSource is created implicitly
when a manifest URI (.m3u8, .mpd, or their mime types)
is used with MediaSource.CreateFromUri(uri).

There are no properties on the MediaPlayerElement that will retrieve the implicit media data source.
No properties nor event handlers of the AdaptiveMediaSource can be retrieved when it is created implicitly.

This scenario might be appropriate if you are using adaptive streaming
for a few media assets as part of your application,
such as a help page or an instructional video.
You can still get a reference to the
MediaSource, and create a MediaPlaybackItem from it,
and then use their properties and event handlers.

This scenario is not appropriate for apps whose main purpose
is adaptive streaming video playback.
See the other scenarios for alternatives.

**Registering event handlers**

There are several events which inform the app of the state of adaptive streaming.
Combining them with the MediaPlayer, MediaPlaybackItem, and MediaSource events
gives the app a lot of information about the media session.

In this scenario, we demonstrate the appropriate location to register event handlers,
object lifetime management, as well as setting and retrieving custom properties.

We provide a simple example of updating text on the UI with current download and
playback bitrates with icons from the Segoe MDL2 Assets font.

We also introduce the concept of attaching CustomProperties to a MediaSource
and reading them in event handlers.

**Modify network calls made by the AdaptiveMediaSource**

The AdaptiveMediaSource allows the app to manage some or all aspects of content download.
This can be done by modifying the arguments of the DownloadRequested event handler,
by passing an HttpClient to the constructor, or by handling downloads via app code.

This scenario will use encryption key requests as an example.
In HLS, the default key exchange is "Identity", also known as Clear-Key,
which is done over HTTPS.
Some apps may desire to improve upon this
by adding additional authentication and authorization to the HTTPS GET for the key.
We will use the key services available in Azure Media Services, and show several implementations.

This sample implements both "Url Query Parameter" and "Authorization Header" mechanisms
for passing tokens to the Key Delivery system in Azure Media Services.

However, for simplicity, we are skipping the Azure Access Control System (ACS) token-exchange code
and have directly hardcoded a Bearer token via our AdaptiveContentModel static class.

For more information on acquiring and passing tokens to the Key Delivery system in
Azure Media Services, see the references below.

This scenario also demonstrates HDCP session management.
The underlying OutputProtectionManager will enforce the policy requested by HdcpSession,
and will apply the maximum protection of any session on the platform.
The app should only concern itself with the policy it has applied via its session: it is not possible
to query the actual HDCP state as imposed by another session or the PlayReady protection system.
For this sample, we respond to the protection level
by imposing a lower DesiredMaxBitrate in order to restrict content on unsecured devices.

Not shown in this scenario, but related in concept:

* Passing a Stream to a manifest in the constructor of AdaptiveMediaSource,
to replace the first web request for a Uri.
* Getting a copy of the downloaded bytes after they have been consumed by the platform in DownloadCompleted.

*Tune the AdaptiveMediaSource download and bitrate switching heuristics**

This scenario shows ways in which the app can tune the adaptive media source.

Setting initial, minimum or maximum bitrates is typical and expected.

Setting InboundBitsPerSecondWindow, BitrateDowngradeTriggerRatio,
and DesiredBitrateHeadroomRatio should be done
only after extensive testing under a range of network conditions.

**Register for, create and consume Metadata Cues**

This scenario shows ways in which the app can consume or create timed metadata.

The AdaptiveMediaSource publishes ID3 tags within TS content
and any M3U8 comment tags it finds in manifests
as TimedMetadataTracks.
This sample shows how to register to consume this data in the app.

The captioning system will also publish TimedMetadataTracks for:
WebVTT segments within an HLS presentation,
additional files (TTML, SRT or WebVTT) added to the source,
or 608 captions within SEI NALUs of H.264.
Although it is not demonstrated in this sample,
the app can opt to render the captions itself
by following a process similar to one used in this scenario.

The app can also create a Custom TimedMetadataTrack and add it to a MediaSource.
This provides a uniform event-based mechanism to consume timed-synchronized information.
Here we demonstrate playback progress reporting using a custom class TrackingEventCue.
We re-use this concept in the next sample
to provide reporting on five ads inserted into a main MediaPlaybackItem.

**Create advertisements within an AdaptiveMediaSource MediaPlaybackItem**

This scenario shows simple ad-insertion with an AdaptiveMediaSource.
Unlike browser-based ad-insertion, apps are not affected by ad-blockers.

We add two pre-roll ads,
two mid-roll ads at 10% into the main content,
and a post-roll ad.
For each of these ads, and the main content,
we re-use the Custom TimedMetadataTracks from the Metadata sample
to raise playback progress events.

**Note** The Windows universal samples require Visual Studio 2015 to build and Windows 10 to execute.

To obtain information about Windows 10 development, go to the [Windows Dev Center](https://dev.windows.com)

To obtain information about Microsoft Visual Studio 2015 and the tools for developing Windows apps, go to [Visual Studio 2015](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Samples

[VideoPlayback](../VideoPlayback)
<!---
[MediaBreakManager](../MediaBreakManager)
[MediaTimelineController](../MediaTimelineController)
-->

### Reference

[AdaptiveMediaSource](https://msdn.microsoft.com/library/windows/apps/windows.media.streaming.adaptive.adaptivemediasource.aspx)  
[AdaptiveMediaSourceAdvancedSettings](https://msdn.microsoft.com/library/windows/apps/windows.media.streaming.adaptive.adaptivemediasourceadvancedsettings.aspx)  
[Windows.Media.Streaming.Adaptive namespace](https://msdn.microsoft.com/library/windows/apps/windows.media.streaming.adaptive.aspx)  
[How client pass tokens to Azure Media Services Key delivery services](http://mingfeiy.com/how-client-pass-tokens-to-azure-media-services-key-delivery-services)  
[An End-to-End Prototype of AES Encryption with ACS Authentication and ACS Token Authorization](https://azure.microsoft.com/blog/an-end-to-end-prototype-of-aes-encryption-with-acs-authentication-and-acs-token-authorization/).

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
