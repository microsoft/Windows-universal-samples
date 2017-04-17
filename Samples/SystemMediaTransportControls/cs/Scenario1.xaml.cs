//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Collections.Generic;
using System.Threading.Tasks;

using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.Media;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.Storage.Streams;
using System.Diagnostics;
using Windows.Media.Playback;
using Windows.Media.Core;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    /// <summary>
    /// Page for scenario 1 of our sample.
    /// </summary>
    public sealed partial class Scenario1
    {
        private MainPage rootPage = null;
        private bool isInitialized = false;

        /// <summary>
        /// Indicates whether this scenario page is still active. Changes value during navigation 
        /// to or away from the page.
        /// </summary>
        private bool isThisPageActive = false;

        // same type as returned from Windows.Storage.Pickers.FileOpenPicker.PickMultipleFilesAsync()
        private IReadOnlyList<StorageFile> playlist = null;

        /// <summary>
        /// index to current media item in playlist
        /// </summary>
        private int currentItemIndex = 0;

        /// <summary>
        /// Indicates whetehr to stat the playlist again upon reaching the end. 
        /// </summary>
        private bool repeatPlaylist = false;

        private SystemMediaTransportControls systemMediaControls = null;

        private DispatcherTimer smtcPositionUpdateTimer = null;

        private bool pausedDueToMute = false;

        private MediaPlayer mediaPlayer;

        private MediaPlaybackItem mediaPlaybackItem;

        public Scenario1()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
            // as NotifyUser()
            rootPage = MainPage.Current;
            isThisPageActive = true;

            // retrieve the SystemMediaTransportControls object, register for its events, and configure it 
            // to a disabled state consistent with how this scenario starts off (ie. no media loaded)
            SetupSystemMediaTransportControls();

            if (!isInitialized)
            {
                isInitialized = true;

                // Create a new MediaPlayer and assign it to the element for video rendering
                mediaPlayer = new MediaPlayer();
                mediaPlayerElement.SetMediaPlayer(mediaPlayer);

                // MediaPlayer has automatic integration with SMTC. This sample demonstrates manual
                // integration with SMTC, and so we disable the MediaPlayer's CommandManager.
                // Unless you have a need to manually integrate with SMTC, it is recommended that
                // you use the built-in integration through CommandManager instead of the
                // procedures demonstrated throughout this sample.
                mediaPlayer.CommandManager.IsEnabled = false;

                mediaPlayer.PlaybackSession.PlaybackStateChanged += mediaPlayer_PlaybackStateChanged;
                mediaPlayer.MediaOpened += mediaPlayer_MediaOpened;
                mediaPlayer.MediaEnded += mediaPlayer_MediaEnded;
                mediaPlayer.MediaFailed += mediaPlayer_MediaFailed;

                smtcPositionUpdateTimer = new DispatcherTimer();
                smtcPositionUpdateTimer.Interval = TimeSpan.FromSeconds(5);
                smtcPositionUpdateTimer.Tick += positionUpdate_TimerTick;
            }
        }

        private void positionUpdate_TimerTick(object sender, object e)
        {
            UpdateSmtcPosition();
        }

        /// <summary>
        /// Invoked when we are about to leave this page.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            isThisPageActive = false;
            rootPage = null;

            // Because the system media transport control object is associated with the current app view
            // (ie. window), all scenario pages in this sample will be using the same instance. Therefore 
            // we need to remove the event handlers specific to this scenario page before 
            // user navigates away to another scenario in the sample.
            systemMediaControls.ButtonPressed -= systemMediaControls_ButtonPressed;
            systemMediaControls.PlaybackRateChangeRequested -= systemMediaControls_PlaybackRateChangeRequested;
            systemMediaControls.PlaybackPositionChangeRequested -= systemMediaControls_PlaybackPositionChangeRequested;
            systemMediaControls.AutoRepeatModeChangeRequested -= systemMediaControls_AutoRepeatModeChangeRequested;

            // Perform other cleanup for this scenario page.
            mediaPlayer.Source = null;
            playlist = null;
            currentItemIndex = 0;
        }

        /// <summary>
        /// Invoked from this scenario page's OnNavigatedTo event handler.  Retrieve and initialize the
        /// SystemMediaTransportControls object.
        /// </summary>
        private void SetupSystemMediaTransportControls()
        {
            // Retrieve the SystemMediaTransportControls object associated with the current app view
            // (ie. window).  There is exactly one instance of the object per view, instantiated by
            // the system the first time GetForCurrentView() is called for the view.  All subsequent 
            // calls to GetForCurrentView() from the same view (eg. from different scenario pages in 
            // this sample) will return the same instance of the object.
            systemMediaControls = SystemMediaTransportControls.GetForCurrentView();

            // This scenario will always start off with no media loaded, so we will start off disabling the 
            // system media transport controls.  Doing so will hide the system UI for media transport controls
            // from being displayed, and will prevent the app from receiving any events such as ButtonPressed 
            // from it, regardless of the current state of event registrations and button enable/disable states.
            // This makes IsEnabled a handy way to turn system media transport controls off and back on, as you 
            // may want to do when the user navigates to and away from certain parts of your app.
            systemMediaControls.IsEnabled = false;

            // To receive notifications for the user pressing media keys (eg. "Stop") on the keyboard, or 
            // clicking/tapping on the equivalent software buttons in the system media transport controls UI, 
            // all of the following needs to be true:
            //     1. Register for ButtonPressed event on the SystemMediaTransportControls object.
            //     2. IsEnabled property must be true to enable SystemMediaTransportControls itself.
            //        [Note: IsEnabled is initialized to true when the system instantiates the
            //         SystemMediaTransportControls object for the current app view.]
            //     3. For each button you want notifications from, set the corresponding property to true to
            //        enable the button.  For example, set IsPlayEnabled to true to enable the "Play" button 
            //        and media key.
            //        [Note: the individual button-enabled properties are initialized to false when the
            //         system instantiates the SystemMediaTransportControls object for the current app view.]
            //
            // Here we'll perform 1, and 3 for the buttons that will always be enabled for this scenario (Play,
            // Pause, Stop).  For 2, we purposely set IsEnabled to false to be consistent with the scenario's 
            // initial state of no media loaded.  Later in the code where we handle the loading of media
            // selected by the user, we will enable SystemMediaTransportControls.
            systemMediaControls.ButtonPressed += systemMediaControls_ButtonPressed;

            // Add event handlers to support requests from the system to change our playback state. 
            systemMediaControls.PlaybackRateChangeRequested += systemMediaControls_PlaybackRateChangeRequested;
            systemMediaControls.PlaybackPositionChangeRequested += systemMediaControls_PlaybackPositionChangeRequested;
            systemMediaControls.AutoRepeatModeChangeRequested += systemMediaControls_AutoRepeatModeChangeRequested;

            // Subscribe to property changed events to get SoundLevel changes. 
            systemMediaControls.PropertyChanged += systemMediaControls_PropertyChanged;

            // Note: one of the prerequisites for an app to be allowed to play audio while in background, 
            // is to enable handling Play and Pause ButtonPressed events from SystemMediaTransportControls.
            systemMediaControls.IsPlayEnabled = true;
            systemMediaControls.IsPauseEnabled = true;
            systemMediaControls.IsStopEnabled = true;
            systemMediaControls.PlaybackStatus = MediaPlaybackStatus.Closed;
        }

        private async void systemMediaControls_PropertyChanged(SystemMediaTransportControls sender, SystemMediaTransportControlsPropertyChangedEventArgs args)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                if (args.Property == SystemMediaTransportControlsProperty.SoundLevel)
                {
                    // Check for the new Sound level
                    switch (systemMediaControls.SoundLevel)
                    {
                        case SoundLevel.Full:
                        case SoundLevel.Low:
                            // If we had paused due to system mute, then resume on unmute.
                            if (pausedDueToMute)
                            {
                                mediaPlayer.Play();
                                pausedDueToMute = false;
                            }
                            break;
                        case SoundLevel.Muted:
                            // We've been muted by the system. Pause playback to release resources.
                            if (mediaPlayer != null && mediaPlayer.PlaybackSession.PlaybackState == MediaPlaybackState.Playing)
                            {
                                mediaPlayer.Pause();
                                pausedDueToMute = true;
                            }
                            break;
                    }
                }
            });
        }

        private async void systemMediaControls_AutoRepeatModeChangeRequested(SystemMediaTransportControls sender, AutoRepeatModeChangeRequestedEventArgs args)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                // MediaPlayer only supports repeating a track, if we want to repeat a list we need to handle it ourselves (or use a MediaPlaybackList)
                switch (args.RequestedAutoRepeatMode)
                {
                    case MediaPlaybackAutoRepeatMode.None:
                        mediaPlayer.IsLoopingEnabled = false;
                        repeatPlaylist = false;
                        break;
                    case MediaPlaybackAutoRepeatMode.List:
                        mediaPlayer.IsLoopingEnabled = false;
                        repeatPlaylist = true;
                        break;
                    case MediaPlaybackAutoRepeatMode.Track:
                        mediaPlayer.IsLoopingEnabled = true;
                        repeatPlaylist = false;
                        break;
                }

                // Report back our new state as whatever was requested, seeing as we always honor it. 
                systemMediaControls.AutoRepeatMode = args.RequestedAutoRepeatMode;
                rootPage.NotifyUser("Auto repeat mode change requested", NotifyType.StatusMessage);
            });
        }


        /// <summary>
        /// Used to update all position/duration related information to the SystemMediaTransport controls.
        /// Can be used when some state changes (new track, position changed) or periodically (every 5 seconds or so) 
        /// to keep the system in sync with this apps playback state. 
        /// </summary>
        private void UpdateSmtcPosition()
        {
            var timelineProperties = new SystemMediaTransportControlsTimelineProperties();

            // This is a simple scenario that supports seeking, therefore start time and min seek are both 0 and 
            // end time and max seek are both the duration. This allows the system to suggest seeking anywhere in the track.
            // Position is the current player position.

            // Note: More complex scenarios may alter more of these values, such as only allowing seeking in a section of the content,
            // by setting min and max seek differently to start and end. For other scenarios such as live playback, end time may be 
            // updated frequently too. 
            timelineProperties.StartTime = TimeSpan.FromSeconds(0);
            timelineProperties.MinSeekTime = TimeSpan.FromSeconds(0);
            timelineProperties.Position = mediaPlayer.PlaybackSession.Position;
            timelineProperties.MaxSeekTime = mediaPlayer.PlaybackSession.NaturalDuration;
            timelineProperties.EndTime = mediaPlayer.PlaybackSession.NaturalDuration;

            systemMediaControls.UpdateTimelineProperties(timelineProperties);
        }

        /// <summary>
        /// Handles a request from the System Media Transport Controls to change our current playback position. 
        /// </summary>
        private async void systemMediaControls_PlaybackPositionChangeRequested(SystemMediaTransportControls sender, PlaybackPositionChangeRequestedEventArgs args)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                // First we validate that the requested position falls within the range of the current piece of content we are playing,
                // this should usually be the case but the content may have changed whilst the request was coming in.
                if (args.RequestedPlaybackPosition.Duration() <= mediaPlayer.PlaybackSession.NaturalDuration.Duration() &&
                args.RequestedPlaybackPosition.Duration().TotalSeconds >= 0)
                {
                    // Next we verify that our player is in a state that we think is valid to change the position
                    if (mediaPlayer.PlaybackSession.PlaybackState == MediaPlaybackState.Paused || mediaPlayer.PlaybackSession.PlaybackState == MediaPlaybackState.Playing)
                    {
                        // Finally if the above conditions are met we update the position and report the new position back to SMTC. 
                        mediaPlayer.PlaybackSession.Position = args.RequestedPlaybackPosition.Duration();
                        UpdateSmtcPosition();
                    }
                }

                rootPage.NotifyUser("Playback position change requested", NotifyType.StatusMessage);
            });
        }

        /// <summary>
        /// Handles a request from the System Media Transport Controls to change our current playback rate
        /// </summary>
        private async void systemMediaControls_PlaybackRateChangeRequested(SystemMediaTransportControls sender, PlaybackRateChangeRequestedEventArgs args)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                // Check to make sure the requested value is in a range we deem appropriate, if so set it on the MediaPlayer. 
                // We then need to turn around and update SMTC so that the system knows the new value.
                if (args.RequestedPlaybackRate >= 0 && args.RequestedPlaybackRate <= 2)
                {
                    mediaPlayer.PlaybackSession.PlaybackRate = args.RequestedPlaybackRate;
                    systemMediaControls.PlaybackRate = mediaPlayer.PlaybackSession.PlaybackRate;
                }

                rootPage.NotifyUser("Playback rate change requested", NotifyType.StatusMessage);
            });
        }




        // For supported audio and video formats for Windows Store apps, see:
        //     http://msdn.microsoft.com/en-us/library/windows/apps/hh986969.aspx
        private static string[] supportedAudioFormats = new string[]
        {
            ".3g2", ".3gp2", ".3gp", ".3gpp", ".m4a", ".mp4", ".asf", ".wma", ".aac", ".adt", ".adts", ".mp3", ".wav", ".ac3", ".ec3",
        };

        private static string[] supportedVideoFormats = new string[]
        {
            ".3g2", ".3gp2", ".3gp", ".3gpp", ".m4v", ".mp4v", ".mp4", ".mov", ".m2ts", ".asf", ".wmv", ".avi",
        };

        /// <summary>
        /// Invoked when user invokes the "Select Files" XAML button in the app.
        /// Launches the Windows File Picker to let user select a list of audio or video files 
        //  to play in the app.
        /// </summary>
        private async void SelectFilesButton_Click(object sender, RoutedEventArgs e)
        {
            FileOpenPicker filePicker = new FileOpenPicker();
            filePicker.ViewMode = PickerViewMode.List;
            filePicker.SuggestedStartLocation = PickerLocationId.MusicLibrary;
            filePicker.CommitButtonText = "Play";
            foreach (string fileExtension in supportedAudioFormats)
            {
                filePicker.FileTypeFilter.Add(fileExtension);
            }
            foreach (string fileExtension in supportedVideoFormats)
            {
                filePicker.FileTypeFilter.Add(fileExtension);
            }

            IReadOnlyList<StorageFile> selectedFiles = await filePicker.PickMultipleFilesAsync();
            if (selectedFiles.Count > 0)
            {
                rootPage.NotifyUser(String.Format("{0} file(s) selected", selectedFiles.Count), NotifyType.StatusMessage);
                playlist = selectedFiles;

                // Now that we have a playlist ready, allow the user to control media playback via system media 
                // transport controls.  We enable it now even if one or more files may turn out unable to load
                // into the MediaPlayer, to allow the user to invoke Next/Previous to go to another
                // item in the playlist to try playing.
                systemMediaControls.IsEnabled = true;

                // For the design of this sample scenario, we will always automatically start playback after
                // the user has selected new files to play.  Set AutoPlay to true so the MediaPlayer
                // will automatically start playing after we set Source on it in SetNewMediaItem().
                mediaPlayer.AutoPlay = true;
                await SetNewMediaItem(0);   // Start with first file in the list of picked files.
            }
            else
            {
                // user canceled the file picker
                rootPage.NotifyUser("no files selected", NotifyType.StatusMessage);
            }
        }

        private async void mediaPlayer_MediaOpened(MediaPlayer sender, object e)
        {
            // UI updates must happen on dispatcher thread
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                if (isThisPageActive)
                {
                    if (mediaPlaybackItem.AudioTracks.Count == 0)
                    {
                        // Ensure the XAML MediaPlayerElement control has enough height and some reasonable width
                        // to display the build-in transport controls for audio-only playback.
                        mediaPlayerElement.MinWidth = 456;
                        mediaPlayerElement.MinHeight = 120;
                        mediaPlayerElement.MaxHeight = 120;

                        // Force the MediaPlayerElement out of fullscreen mode in case the user was using that
                        // for a video playback but we are now loading audio-only content.  The build-in 
                        // transport controls for audio-only playback do not have the button to 
                        // enter/exit fullscreen mode.
                        mediaPlayerElement.IsFullWindow = false;
                    }
                    else
                    {
                        // For video playback, let XAML resize MediaPlayerElement control based on the dimensions 
                        // of the video, but also have XAML scale it down as necessary (preserving aspect ratio)
                        // to a reasonable maximum height like 300.
                        mediaPlayerElement.MinWidth = 0;
                        mediaPlayerElement.MinHeight = 0;
                        mediaPlayerElement.MaxHeight = 300;
                    }
                }
            });
        }

        private async void mediaPlayer_MediaEnded(MediaPlayer sender, object e)
        {
            if (isThisPageActive)
            {
                if (currentItemIndex < playlist.Count - 1)
                {
                    // Current media must've been playing if we received an event about it ending.
                    // The design of this sample scenario is to automatically start playing the next
                    // item in the playlist.  So we'll set the AutoPlay property to true, then in 
                    // SetNewMediaItem() when we eventually set Source on the MediaPlayer,
                    // it will automatically playing the new media item.
                    mediaPlayer.AutoPlay = true;
                    await SetNewMediaItem(currentItemIndex + 1);
                }
                else
                {
                    // End of playlist reached. 
                    if (repeatPlaylist)
                    {
                        // Playlist repeat was selected so start again. 
                        mediaPlayer.AutoPlay = true;
                        await SetNewMediaItem(0);
                        await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => // UI updates must happen on dispatcher thread
                        {
                            rootPage.NotifyUser("end of playlist, starting playback again at beginning of playlist", NotifyType.StatusMessage);
                        });
                    }
                    else
                    {
                        // Repeat wasn't selected so just stop playback.
                        mediaPlayer.AutoPlay = false;
                        mediaPlayer.Pause();
                        mediaPlayer.PlaybackSession.Position = TimeSpan.Zero;
                        await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                        {
                            rootPage.NotifyUser("end of playlist, stopping playback", NotifyType.StatusMessage);
                        });
                    }
                }
            }
        }

        private async void mediaPlayer_MediaFailed(MediaPlayer sender, MediaPlayerFailedEventArgs e)
        {
            // UI updates must happen on dispatcher thread
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                if (isThisPageActive)
                {
                    string errorMessage = String.Format(@"Cannot play {0} [""{1}""]." +
                        "\nPress Next or Previous to continue, or select new files to play.",
                        playlist[currentItemIndex].Name,
                        e.ErrorMessage.Trim());

                    rootPage.NotifyUser(errorMessage, NotifyType.ErrorMessage);
                }
            });
        }

        /// <summary>
        /// Updates the SystemMediaTransportControls' PlaybackStatus property based on the PlaybackState property of the
        /// MediaPlayer.
        /// </summary>
        /// <remarks>Invoked mainly from the MediaPlayer's PlaybackStateChanged event handler.</remarks>
        private void SyncPlaybackStatusToMediaPlayerState()
        {
            // Updating PlaybackStatus with accurate information is important; for example, it determines whether the system media
            // transport controls UI will show a play vs pause software button, and whether hitting the play/pause toggle key on 
            // the keyboard will translate to a Play vs a Pause ButtonPressed event.
            //
            // Even if your app uses its own custom transport controls in place of the built-in ones from XAML, it is still a good
            // idea to update PlaybackStatus in response to the MediaPlayer's PlaybackStateChanged event.  Windows supports scenarios 
            // such as streaming media from a MediaPlayer to a networked device (eg. TV) selected by the user from Devices charm 
            // (ie. "Play To"), in which case the user may pause and resume media streaming using a TV remote or similar means.  
            // The CurrentStateChanged event may be the only way to get notified of playback status changes in those cases.
            switch (mediaPlayer.PlaybackSession.PlaybackState)
            {
                case MediaPlaybackState.None:
                    systemMediaControls.PlaybackStatus = MediaPlaybackStatus.Closed;
                    break;

                case MediaPlaybackState.Opening:
                    // This state is when new media is being loaded to the MediaPlayer [ie.
                    // Source].  For this sample the design is to maintain the previous playing/pause 
                    // state before the new media is being loaded.  So we'll leave the PlaybackStatus alone
                    // during loading.  This keeps the system UI from flickering between displaying a "Play" 
                    // vs "Pause" software button during the transition to a new media item.
                    break;

                case MediaPlaybackState.Buffering:
                    // No updates in MediaPlaybackStatus necessary--buffering is just
                    // a transitional state where the system is still working to get
                    // media to start or to continue playing.
                    break;

                case MediaPlaybackState.Paused:
                    if (mediaPlayer.PlaybackSession.Position == TimeSpan.Zero)
                        systemMediaControls.PlaybackStatus = MediaPlaybackStatus.Stopped;
                    else
                        systemMediaControls.PlaybackStatus = MediaPlaybackStatus.Paused;
                    break;

                case MediaPlaybackState.Playing:
                    systemMediaControls.PlaybackStatus = MediaPlaybackStatus.Playing;
                    break;

            }

            // If we started playing start our timer else make sure it's stopped.
            // On state changed always send an update to keep the system in sync. 
            if (mediaPlayer.PlaybackSession.PlaybackState == MediaPlaybackState.Playing)
            {
                smtcPositionUpdateTimer.Start();
            }
            else
            {
                smtcPositionUpdateTimer.Stop();
            }

            UpdateSmtcPosition();

            UpdateCustomTransportControls();
        }

        private async void mediaPlayer_PlaybackStateChanged(MediaPlaybackSession sender, object e)
        {
            // UI updates must happen on dispatcher thread
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                if (!isThisPageActive)
                {
                    return;
                }

                // For the design of this sample, the general idea is that if user is playing
                // media when Next/Previous is invoked, we will automatically start playing
                // the new item once it has loaded.  Whereas if the user has paused or stopped
                // media playback when Next/Previous is invoked, we won't automatically start
                // playing the new item.  In other words, we maintain the user's intent to
                // play or not play across changing the media.  This is most easily handled
                // using the AutoPlay property of the MediaPlayer.
                if (mediaPlayer.PlaybackSession.PlaybackState == MediaPlaybackState.Playing)
                {
                    mediaPlayer.AutoPlay = true;
                }
                else if (mediaPlayer.PlaybackSession.PlaybackState == MediaPlaybackState.None ||
                         mediaPlayer.PlaybackSession.PlaybackState == MediaPlaybackState.Paused)
                {
                    mediaPlayer.AutoPlay = false;
                }

                // Update the playback status tracked by SystemMediaTransportControls.  See comments
                // in SyncPlaybackStatusToMediaPlayerState()'s body for why this is important.
                SyncPlaybackStatusToMediaPlayerState();
            });
        }

        private async void systemMediaControls_ButtonPressed(SystemMediaTransportControls sender, SystemMediaTransportControlsButtonPressedEventArgs args)
        {
            // The system media transport control's ButtonPressed event may not fire on the app's UI thread.  XAML controls 
            // (including the MediaPlayerElement control in our page as well as the scenario page itself) typically can only be 
            // safely accessed and manipulated on the UI thread, so here for simplicity, we dispatch our entire event handling 
            // code to execute on the UI thread, as our code here primarily deals with updating the UI and the MediaPlayerElement.
            // 
            // Depending on how exactly you are handling the different button presses (which for your app may include buttons 
            // not used in this sample scenario), you may instead choose to only dispatch certain parts of your app's 
            // event handling code (such as those that interact with XAML) to run on UI thread.
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, async () =>
            {
                // Because the handling code is dispatched asynchronously, it is possible the user may have
                // navigated away from this scenario page to another scenario page by the time we are executing here.
                // Check to ensure the page is still active before proceeding.
                if (isThisPageActive)
                {
                    switch (args.Button)
                    {
                        case SystemMediaTransportControlsButton.Play:
                            rootPage.NotifyUser("Play pressed", NotifyType.StatusMessage);
                            mediaPlayer.Play();
                            break;

                        case SystemMediaTransportControlsButton.Pause:
                            rootPage.NotifyUser("Pause pressed", NotifyType.StatusMessage);
                            mediaPlayer.Pause();
                            break;

                        case SystemMediaTransportControlsButton.Stop:
                            rootPage.NotifyUser("Stop pressed", NotifyType.StatusMessage);
                            mediaPlayer.Pause();
                            mediaPlayer.PlaybackSession.Position = TimeSpan.Zero;
                            break;

                        case SystemMediaTransportControlsButton.Next:
                            rootPage.NotifyUser("Next pressed", NotifyType.StatusMessage);
                            // range-checking will be performed in SetNewMediaItem()
                            await SetNewMediaItem(currentItemIndex + 1);
                            break;

                        case SystemMediaTransportControlsButton.Previous:
                            rootPage.NotifyUser("Previous pressed", NotifyType.StatusMessage);
                            // range-checking will be performed in SetNewMediaItem()
                            await SetNewMediaItem(currentItemIndex - 1);
                            break;

                            // Insert additional case statements for other buttons you want to handle in your app.
                            // Remember that you also need to first enable those buttons via the corresponding
                            // Is****Enabled property on the SystemMediaTransportControls object.
                    }
                }
            });
        }

        /// <summary>
        /// Returns an appropriate MediaPlaybackType value based on the given StorageFile's ContentType (MIME type).
        /// </summary>
        /// <returns>
        /// One of the three valid MediaPlaybackType enum values, or MediaPlaybackType.Unknown if the ContentType 
        /// is not a media type (audio, video, image) or cannot be determined.
        //  </returns>
        /// <remarks>
        /// For use with SystemMediaTransportControlsDisplayUpdater.CopyFromFileAsync() in UpdateSystemMediaControlsDisplayAsync().
        /// </remarks>
        private MediaPlaybackType GetMediaTypeFromFileContentType(StorageFile file)
        {
            // Determine the appropriate MediaPlaybackType of the media file based on its ContentType (ie. MIME type).
            // The MediaPlaybackType determines the information shown in the system UI for the system media transport
            // controls.  For example, the CopyFromFileAsync() API method will look for different metadata properties 
            // from the file to be extracted for eventual display in the system UI, depending on the MediaPlaybackType 
            // passed to the method.

            // NOTE: MediaPlaybackType.Unknown is *not* a valid value to use in SystemMediaTransportControls APIs.  
            // This method will return MediaPlaybackType.Unknown to indicate no valid MediaPlaybackType exists/can be 
            // determined for the given StorageFile.
            MediaPlaybackType mediaPlaybackType = MediaPlaybackType.Unknown;
            string fileMimeType = file.ContentType.ToLowerInvariant();

            if (fileMimeType.StartsWith("audio/"))
            {
                mediaPlaybackType = MediaPlaybackType.Music;
            }
            else if (fileMimeType.StartsWith("video/"))
            {
                mediaPlaybackType = MediaPlaybackType.Video;
            }
            else if (fileMimeType.StartsWith("image/"))
            {
                mediaPlaybackType = MediaPlaybackType.Image;
            }

            return mediaPlaybackType;
        }

        /// <summary>
        /// Updates the system UI for media transport controls to display media metadata from the given StorageFile.
        /// </summary>
        /// <param name="mediaFile">
        /// The media file being loaded.  This method will try to extract media metadata from the file for use in
        /// the system UI for media transport controls.
        /// </param>
        private async Task UpdateSystemMediaControlsDisplayAsync(StorageFile mediaFile)
        {
            MediaPlaybackType mediaType = GetMediaTypeFromFileContentType(mediaFile);

            bool copyFromFileAsyncSuccessful = false;
            if (MediaPlaybackType.Unknown != mediaType)
            {
                // Use the SystemMediaTransportControlsDisplayUpdater's CopyFromFileAsync() API method to try extracting
                // from the StorageFile the relevant media metadata information (eg. track title, artist and album art 
                // for MediaPlaybackType.Music), copying them into properties of DisplayUpdater and related classes.
                //
                // In place of using CopyFromFileAsync() [or, you can't use that method because your app's media playback
                // scenario doesn't involve StorageFiles], you can also use other properties in DisplayUpdater and 
                // related classes to explicitly set the MediaPlaybackType and media metadata properties to values of 
                // your choosing.
                //
                // Usage notes:
                //     - the first argument cannot be MediaPlaybackType.Unknown
                //     - API method may throw an Exception on certain file errors from the passed in StorageFile, such as 
                //       file-not-found error (eg. file was deleted after user had picked it from file picker earlier).
                try
                {
                    copyFromFileAsyncSuccessful = await systemMediaControls.DisplayUpdater.CopyFromFileAsync(mediaType, mediaFile);
                }
                catch (Exception)

                {
                    // For this sample, we will handle this case same as CopyFromFileAsync returning false, 
                    // though we could provide our own metadata here.
                }
            }
            else
            {
                // If we are here, it means we are unable to determine a MediaPlaybackType based on the StorageFile's
                // ContentType (MIME type).  CopyFromFileAsync() requires a valid MediaPlaybackType (ie. cannot be
                // MediaPlaybackType.Unknown) for the first argument.  One way to handle this is to just pick a valid 
                // MediaPlaybackType value to call CopyFromFileAsync() with, based on what your app does (eg. Music
                // if your app is a music player).  The MediaPlaybackType directs the API to look for particular sets
                // of media metadata information from the StorageFile, and extraction is best-effort so in worst case
                // simply no metadata information are extracted.  
                //
                // For this sample, we will handle this case the same way as CopyFromFileAsync() returning false
            }

            if (!isThisPageActive)
            {
                // User may have navigated away from this scenario page to another scenario page 
                // before the async operation completed.
                return;
            }

            if (!copyFromFileAsyncSuccessful)
            {
                // For this sample, if CopyFromFileAsync() didn't work for us for whatever reasons, we will just 
                // clear DisplayUpdater of all previously set metadata, if any.  This makes sure we don't end up 
                // displaying in the system UI for media transport controls any stale metadata from the media item 
                // we were previously playing.
                systemMediaControls.DisplayUpdater.ClearAll();
            }

            // Finally update the system UI display for media transport controls with the new values currently
            // set in the DisplayUpdater, be it via CopyFrmoFileAsync(), ClearAll(), etc.
            systemMediaControls.DisplayUpdater.Update();
        }

        /// <summary>
        /// Performs all necessary actions (including SystemMediaTransportControls related) of loading a new media item 
        /// in the app for playback.
        /// </summary>
        /// <param name="newItemIndex">index in playlist of new item to load for playback, can be out of range.</param>
        /// <remarks>
        /// If the newItemIndex argument is out of range, it will be adjusted accordingly to stay in range.
        /// </remarks>
        private async Task SetNewMediaItem(int newItemIndex)
        {
            // enable Next button unless we're on last item of the playlist
            if (newItemIndex >= playlist.Count - 1)
            {
                systemMediaControls.IsNextEnabled = false;
                newItemIndex = playlist.Count - 1;
            }
            else
            {
                systemMediaControls.IsNextEnabled = true;
            }

            // enable Previous button unless we're on first item of the playlist
            if (newItemIndex <= 0)
            {
                systemMediaControls.IsPreviousEnabled = false;
                newItemIndex = 0;
            }
            else
            {
                systemMediaControls.IsPreviousEnabled = true;
            }

            // note that the Play, Pause and Stop buttons were already enabled via SetupSystemMediaTransportControls() 
            // invoked during this scenario page's OnNavigateToHandler()


            currentItemIndex = newItemIndex;
            StorageFile mediaFile = playlist[newItemIndex];
            IRandomAccessStream stream = null;
            try
            {
                stream = await mediaFile.OpenAsync(FileAccessMode.Read);
            }
            catch (Exception e)
            {
                // User may have navigated away from this scenario page to another scenario page 
                // before the async operation completed.
                if (isThisPageActive)
                {
                    // If the file can't be opened, for this sample we will behave similar to the case of
                    // setting a corrupted/invalid media file stream on the MediaPlayer (which triggers a 
                    // MediaFailed event).  We abort any ongoing playback by nulling the MediaPlayer's 
                    // source.  The user must press Next or Previous to move to a different media item, 
                    // or use the file picker to load a new set of files to play.
                    mediaPlayer.Source = null;

                    string errorMessage = String.Format(@"Cannot open {0} [""{1}""]." +
                        "\nPress Next or Previous to continue, or select new files to play.",
                        mediaFile.Name,
                        e.Message.Trim());
                    rootPage.NotifyUser(errorMessage, NotifyType.ErrorMessage);
                }
            }

            // User may have navigated away from this scenario page to another scenario page 
            // before the async operation completed. Check to make sure page is still active.
            if (!isThisPageActive)
            {
                return;
            }

            if (stream != null)
            {
                // We're about to change the MediaPlayer's source media, so put ourselves into a 
                // "changing media" state.  We stay in that state until the new media is playing,
                // loaded (if user has currently paused or stopped playback), or failed to load.
                // At those points we will call OnChangingMediaEnded().
                //
                // Note that the SMTC visual state may not update until assigning a source or
                // beginning playback. If using a different API than MediaPlayer, such as AudioGraph,
                // you will need to begin playing a stream to see SMTC update.
                mediaPlaybackItem = new MediaPlaybackItem(MediaSource.CreateFromStream(stream, mediaFile.ContentType));
                mediaPlayer.Source = mediaPlaybackItem;
            }

            try
            {
                // Updates the system UI for media transport controls to display metadata information
                // reflecting the file we are playing (eg. track title, album art/video thumbnail, etc.)
                // We call this even if the mediaFile can't be opened; in that case the method can still 
                // update the system UI to remove any metadata information previously displayed.
                await UpdateSystemMediaControlsDisplayAsync(mediaFile);
            }
            catch (Exception e)
            {
                // Check isThisPageActive as user may have navigated away from this scenario page to 
                // another scenario page before the async operations completed.
                if (isThisPageActive)
                {
                    rootPage.NotifyUser(e.Message, NotifyType.ErrorMessage);
                }
            }
        }

        /// <summary>
        /// Updates the custom transport controls (XAML).
        /// 
        /// For the sake of clarity in this Sample, we'll handle updating our custom transport controls
        /// seperately from updating the SystemMediaTransportControls.
        /// 
        /// This should be called on the UI thread.
        /// </summary>
        private void UpdateCustomTransportControls()
        {
            // Update the custom controls based upon the state of our playlist.
            // We want to match the same state that was previously set on the SystemMediaTransportControls
            nextButton.IsEnabled = systemMediaControls.IsNextEnabled;
            previousButton.IsEnabled = systemMediaControls.IsPreviousEnabled;

            // Update the Play / Pause Toggle button based upon the MediaPlayer's current state
            switch (mediaPlayer.PlaybackSession.PlaybackState)
            {
                case MediaPlaybackState.None:
                    // Revert back to a known play / pause toggle state
                    playPauseButton.Content = new SymbolIcon(Symbol.Play);
                    playPauseButton.IsEnabled = false;
                    break;

                case MediaPlaybackState.Paused:
                    playPauseButton.Content = new SymbolIcon(Symbol.Play);
                    playPauseButton.IsEnabled = true;
                    break;

                case MediaPlaybackState.Playing:
                    playPauseButton.Content = new SymbolIcon(Symbol.Pause);
                    playPauseButton.IsEnabled = true;
                    break;
            }
        }

        /// <summary>
        /// Handle the Custom Transport Controls Play / Pause button being clicked.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void playPauseButton_Click(object sender, RoutedEventArgs e)
        {
            if (mediaPlayer.PlaybackSession.PlaybackState == MediaPlaybackState.Paused)
            {
                mediaPlayer.Play();
                rootPage.NotifyUser("Custom Transport Controls Play pressed", NotifyType.StatusMessage);
            }
            else
            {
                mediaPlayer.Pause();
                rootPage.NotifyUser("Custom Transport Controls Pause pressed", NotifyType.StatusMessage);
            }
        }

        /// <summary>
        /// Handle the Custom Transport Controls SkipNext button click.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void nextButton_Click(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("Custom Transport Controls Next pressed", NotifyType.StatusMessage);
            // range-checking will be performed in SetNewMediaItem()
            await SetNewMediaItem(currentItemIndex + 1);
        }

        /// <summary>
        /// Handle the Custom Transport Controls SkipPrevious button click.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void previousButton_Click(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("Custom Transport Controls Previous pressed", NotifyType.StatusMessage);
            // range-checking will be performed in SetNewMediaItem()
            await SetNewMediaItem(currentItemIndex - 1);
        }
    }
}
