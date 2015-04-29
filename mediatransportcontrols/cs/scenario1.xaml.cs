//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
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

        private SystemMediaTransportControls systemMediaControls = null;

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
                MyMediaElement.CurrentStateChanged += MyMediaElement_CurrentStateChanged;
                MyMediaElement.MediaOpened += MyMediaElement_MediaOpened;
                MyMediaElement.MediaEnded += MyMediaElement_MediaEnded;
                MyMediaElement.MediaFailed += MyMediaElement_MediaFailed;
            }
        }

        /// <summary>
        /// Invoked when we are about to leave this page.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            isThisPageActive = false;

            // Because the system media transport control object is associated with the current app view
            // (ie. window), all scenario pages in this sample will be using the same instance. Therefore 
            // we need to remove the ButtonPressed event handler specific to this scenario page before 
            // user navigates away to another scenario in the sample.
            systemMediaControls.ButtonPressed -= systemMediaControls_ButtonPressed;

            // Perform other cleanup for this scenario page.
            MyMediaElement.Source = null;
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

            // Note: one of the prerequisites for an app to be allowed to play audio while in background, 
            // is to enable handling Play and Pause ButtonPressed events from SystemMediaTransportControls.
            systemMediaControls.IsPlayEnabled = true;
            systemMediaControls.IsPauseEnabled = true;
            systemMediaControls.IsStopEnabled = true;
            systemMediaControls.PlaybackStatus = MediaPlaybackStatus.Closed;
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
                // into the MediaElement control, to allow the user to invoke Next/Previous to go to another
                // item in the playlist to try playing.
                systemMediaControls.IsEnabled = true;

                // For the design of this sample scenario, we will always automatically start playback after
                // the user has selected new files to play.  Set AutoPlay to true so the XAML MediaElement
                // control will automatically start playing after we do a SetSource() on it in SetNewMediaItem().
                MyMediaElement.AutoPlay = true;
                await SetNewMediaItem(0);   // Start with first file in the list of picked files.
            }
            else
            {
                // user canceled the file picker
                rootPage.NotifyUser("no files selected", NotifyType.StatusMessage);
            }
        }

        private void MyMediaElement_MediaOpened(object sender, RoutedEventArgs e)
        {
            if (isThisPageActive)
            {
                if (MyMediaElement.IsAudioOnly)
                {
                    // Ensure the XAML MediaElement control has enough height and some reasonable width
                    // to display the build-in transport controls for audio-only playback.
                    MyMediaElement.MinWidth = 456;
                    MyMediaElement.MinHeight = 42;
                    MyMediaElement.MaxHeight = 42;

                    // Force the MediaElement out of fullscreen mode in case the user was using that
                    // for a video playback but we are now loading audio-only content.  The build-in 
                    // transport controls for audio-only playback do not have the button to 
                    // enter/exit fullscreen mode.
                    MyMediaElement.IsFullWindow = false;
                }
                else
                {
                    // For video playback, let XAML resize MediaElement control based on the dimensions 
                    // of the video, but also have XAML scale it down as necessary (preserving aspect ratio)
                    // to a reasonable maximum height like 300.
                    MyMediaElement.MinWidth = 0;
                    MyMediaElement.MinHeight = 0;
                    MyMediaElement.MaxHeight = 300;
                }
            }
        }

        private async void MyMediaElement_MediaEnded(object sender, RoutedEventArgs e)
        {
            if (isThisPageActive)
            {
                if (currentItemIndex < playlist.Count - 1)
                {
                    // Current media must've been playing if we received an event about it ending.
                    // The design of this sample scenario is to automatically start playing the next
                    // item in the playlist.  So we'll set the AutoPlay property to true, then in 
                    // SetNewMediaItem() when we eventually call SetSource() on the XAML MediaElement 
                    // control, it will automatically playing the new media item.
                    MyMediaElement.AutoPlay = true;
                    await SetNewMediaItem(currentItemIndex + 1);
                }
                else
                {
                    // End of playlist reached.  We'll just stop media playback.
                    MyMediaElement.AutoPlay = false;
                    MyMediaElement.Stop();
                    rootPage.NotifyUser("end of playlist, stopping playback", NotifyType.StatusMessage);
                }
            }
        }

        private void MyMediaElement_MediaFailed(object sender, ExceptionRoutedEventArgs e)
        {
            if (isThisPageActive)
            {
                string errorMessage = String.Format(@"Cannot play {0} [""{1}""]." +
                    "\nPress Next or Previous to continue, or select new files to play.",
                    playlist[currentItemIndex].Name,
                    e.ErrorMessage.Trim());
                rootPage.NotifyUser(errorMessage, NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// Updates the SystemMediaTransportControls' PlaybackStatus property based on the CurrentState property of the
        /// MediaElement control.
        /// </summary>
        /// <remarks>Invoked mainly from the MediaElement control's CurrentStateChanged event handler.</remarks>
        private void SyncPlaybackStatusToMediaElementState()
        {
            // Updating PlaybackStatus with accurate information is important; for example, it determines whether the system media
            // transport controls UI will show a play vs pause software button, and whether hitting the play/pause toggle key on 
            // the keyboard will translate to a Play vs a Pause ButtonPressed event.
            //
            // Even if your app uses its own custom transport controls in place of the built-in ones from XAML, it is still a good
            // idea to update PlaybackStatus in response to the MediaElement's CurrentStateChanged event.  Windows supports scenarios 
            // such as streaming media from a MediaElement to a networked device (eg. TV) selected by the user from Devices charm 
            // (ie. "Play To"), in which case the user may pause and resume media streaming using a TV remote or similar means.  
            // The CurrentStateChanged event may be the only way to get notified of playback status changes in those cases.
            switch (MyMediaElement.CurrentState)
            {
            case MediaElementState.Closed:
                systemMediaControls.PlaybackStatus = MediaPlaybackStatus.Closed;
                break;

            case MediaElementState.Opening:
                // This state is when new media is being loaded to the XAML MediaElement control [ie.
                // SetSource()].  For this sample the design is to maintain the previous playing/pause 
                // state before the new media is being loaded.  So we'll leave the PlaybackStatus alone
                // during loading.  This keeps the system UI from flickering between displaying a "Play" 
                // vs "Pause" software button during the transition to a new media item.
                break;

            case MediaElementState.Buffering:
                // No updates in MediaPlaybackStatus necessary--buffering is just
                // a transitional state where the system is still working to get
                // media to start or to continue playing.
                break;

            case MediaElementState.Paused:
                systemMediaControls.PlaybackStatus = MediaPlaybackStatus.Paused;
                break;

            case MediaElementState.Playing:
                systemMediaControls.PlaybackStatus = MediaPlaybackStatus.Playing;
                break;

            case MediaElementState.Stopped:
                systemMediaControls.PlaybackStatus = MediaPlaybackStatus.Stopped;
                break;
            }
        }

        private void MyMediaElement_CurrentStateChanged(object sender, RoutedEventArgs e)
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
            // using the AutoPlay property of the XAML MediaElement control.
            if (MyMediaElement.CurrentState == MediaElementState.Playing)
            {
                MyMediaElement.AutoPlay = true;
            }
            else if (MyMediaElement.CurrentState == MediaElementState.Stopped ||
                     MyMediaElement.CurrentState == MediaElementState.Paused)
            {
                MyMediaElement.AutoPlay = false;
            }

            // Update the playback status tracked by SystemMediaTransportControls.  See comments
            // in SyncPlaybackStatusToMediaElementState()'s body for why this is important.
            SyncPlaybackStatusToMediaElementState();
        }

        private async void systemMediaControls_ButtonPressed(SystemMediaTransportControls sender, SystemMediaTransportControlsButtonPressedEventArgs args)
        {   
            // The system media transport control's ButtonPressed event may not fire on the app's UI thread.  XAML controls 
            // (including the MediaElement control in our page as well as the scenario page itself) typically can only be 
            // safely accessed and manipulated on the UI thread, so here for simplicity, we dispatch our entire event handling 
            // code to execute on the UI thread, as our code here primarily deals with updating the UI and the MediaElement.
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
                            MyMediaElement.Play();
                            break;

                        case SystemMediaTransportControlsButton.Pause:
                            rootPage.NotifyUser("Pause pressed", NotifyType.StatusMessage);
                            MyMediaElement.Pause();
                            break;

                        case SystemMediaTransportControlsButton.Stop:
                            rootPage.NotifyUser("Stop pressed", NotifyType.StatusMessage);
                            MyMediaElement.Stop();
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
                    // For this sample, we will handle this case same as CopyFromFileAsync returning false
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
                    // setting a corrupted/invalid media file stream on the MediaElement (which triggers a 
                    // MediaFailed event).  We abort any ongoing playback by nulling the MediaElement's 
                    // source.  The user must press Next or Previous to move to a different media item, 
                    // or use the file picker to load a new set of files to play.
                    MyMediaElement.Source = null;

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
                // We're about to change the MediaElement's source media, so put ourselves into a 
                // "changing media" state.  We stay in that state until the new media is playing,
                // loaded (if user has currently paused or stopped playback), or failed to load.
                // At those points we will call OnChangingMediaEnded().
                MyMediaElement.SetSource(stream, mediaFile.ContentType);
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
    }
}
