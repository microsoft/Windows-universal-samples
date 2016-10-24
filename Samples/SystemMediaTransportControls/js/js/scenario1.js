//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    // System Media Transport Controls
    var systemMediaControls;

    // HTML5 <video> element in this scenario's HTML markup.
    // (This script would also work unmodified with <audio> element;
    // see scenario1.html for details.)
    var player;

    // contains list of media files (as Windows.Storage.StorageFile
    // objects) selected by the user via 
    // Windows.Storage.Pickers.FileOpenPicker.pickMultipleFilesAsync()
    var playlist = [];

    // keeps track of current item playing from the playlist.
    var currentItemIndex = 0;

    var isScenario1Active = false;

    var repeatPlaylist = false;

    var positionUpdateTimerId = 0;

    var pausedDueToMute = false;

    var page = WinJS.UI.Pages.define("/html/scenario1.html", {
        ready: function (element, options) {
            isScenario1Active = true;

            // retrieve the SystemMediaTransportControls object, register for its events, and configure it 
            // to a disabled state consistent with how this scenario starts off (ie. no media loaded)
            setupSystemMediaTransportControls();

            document.getElementById("sampleHeader").innerHTML = "System media transport controls"

            document.getElementById("pickerButton").addEventListener("click", getFilesWithPicker, false);
            player = document.getElementById("player");
            player.addEventListener("ended", playerEnded, false);
            player.addEventListener("playing", playerPlaying, false);
            player.addEventListener("pause", playerPaused, false);
            player.addEventListener("error", playerError, false);
        },

        unload: function (element, options) {
            isScenario1Active = false;

            // Because the system media transport control object is associated with the current app view
            // (ie. window), all scenario pages in this sample will be using the same instance. Therefore 
            // we need to remove the ButtonPressed event handler specific to this scenario page before 
            // user navigates away to another scenario in the sample.
            systemMediaControls.removeEventListener("buttonpressed", systemMediaControlsButtonPressed, false);
            systemMediaControls.removeEventListener("propertychanged", systemMediaControlsPropertyChanged, false);

            systemMediaControls.removeEventListener("playbackratechangerequested", systemMediaControlsPlaybackRateChangeRequested, false);
            systemMediaControls.removeEventListener("playbackpositionchangerequested", systemMediaControlsPlaybackPositionChangeRequested, false);
            systemMediaControls.removeEventListener("autorepeatmodechangerequested", systemMediaControlsAutoRepeatModeChangeRequested, false);

            // Perform other cleanup for this scenario page.
            player.src = null;
            playlist = [];
            currentItemIndex = 0;

            clearInterval(positionUpdateTimerId);
        }
    });

    // Invoked from this Page's "ready" method.  Retrieve and initialize the 
    // SystemMediaTransportControls object.
    function setupSystemMediaTransportControls() {
        // Retrieve the SystemMediaTransportControls object associated with the current app view
        // (ie. window).  There is exactly one instance of the object per view, instantiated by
        // the system the first time getForCurrentView() is called for the view.  All subsequent 
        // calls to getForCurrentView() from the same view (eg. from different scenario pages in 
        // this sample) will return the same instance of the object.
        systemMediaControls = Windows.Media.SystemMediaTransportControls.getForCurrentView();

        // This scenario will always start off with no media loaded, so we will start off disabling the 
        // system media transport controls.  Doing so will hide the system UI for media transport controls
        // from being displayed, and will prevent the app from receiving any events such as buttonpressed 
        // from it, regardless of the current state of event registrations and button enable/disable states.
        // This makes isEnabled a handy way to turn system media transport controls off and back on, as you 
        // may want to do when the user navigates to and away from certain parts of your app.
        systemMediaControls.isEnabled = false;

        // To receive notifications for the user pressing media keys (eg. "Stop") on the keyboard, or 
        // clicking/tapping on the equivalent software buttons in the system media transport controls UI, 
        // all of the following needs to be true:
        //     1. Register for buttonpressed event on the SystemMediaTransportControls object.
        //     2. isEnabled property must be true to enable SystemMediaTransportControls itself.
        //        [Note: isEnabled is initialized to true when the system instantiates the
        //         SystemMediaTransportControls object for the current app view.]
        //     3. For each button you want notifications from, set the corresponding property to true to
        //        enable the button.  For example, set isPlayEnabled to true to enable the "Play" button 
        //        and media key.
        //        [Note: the individual button-enabled properties are initialized to false when the
        //         system instantiates the SystemMediaTransportControls object for the current app view.]
        //
        // Here we'll perform 1, and 3 for the buttons that will always be enabled for this scenario (Play,
        // Pause, Stop).  For 2, we purposely set isEnabled to false to be consistent with the scenario's 
        // initial state of no media loaded.  Later in the code where we handle the loading of media
        // selected by the user, we will enable SystemMediaTransportControls.
        systemMediaControls.addEventListener("buttonpressed", systemMediaControlsButtonPressed, false);
        systemMediaControls.addEventListener("propertychanged", systemMediaControlsPropertyChanged, false);

        systemMediaControls.addEventListener("playbackratechangerequested", systemMediaControlsPlaybackRateChangeRequested, false);
        systemMediaControls.addEventListener("playbackpositionchangerequested", systemMediaControlsPlaybackPositionChangeRequested, false);
        systemMediaControls.addEventListener("autorepeatmodechangerequested", systemMediaControlsAutoRepeatModeChangeRequested, false);  

        // Note: one of the prerequisites for an app to be allowed to play audio while in background, 
        // is to enable handling Play and Pause buttonpressed events from SystemMediaTransportControls.
        systemMediaControls.isPlayEnabled = true;
        systemMediaControls.isPauseEnabled = true;
        systemMediaControls.isStopEnabled = true;
        systemMediaControls.playbackStatus = Windows.Media.MediaPlaybackStatus.closed;
    }

    // For supported audio and video formats for Windows Store apps, see:
    //     http://msdn.microsoft.com/en-us/library/windows/apps/hh986969.aspx
    var supportedAudioFormats = [
        ".3g2", ".3gp2", ".3gp", ".3gpp", ".m4a", ".mp4", ".asf", ".wma", ".aac", ".adt", ".adts", ".mp3", ".wav", ".ac3", ".ec3",
    ];

    var supportedVideoFormats = [
        ".3g2", ".3gp2", ".3gp", ".3gpp", ".m4v", ".mp4v", ".mp4", ".mov", ".m2ts", ".asf", ".wmv", ".avi",
    ];

    // Opens the Windows File Picker to let user select a list of audio or video files
    // to play in the app.
    function getFilesWithPicker() {

        var filePicker = new Windows.Storage.Pickers.FileOpenPicker();
        filePicker.viewMode = Windows.Storage.Pickers.PickerViewMode.list;
        filePicker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.musicLibrary;
        filePicker.commitButtonText = "Play";
       
        for (var i = 0; i < supportedAudioFormats.length; i++) {
            filePicker.fileTypeFilter.append(supportedAudioFormats[i]);
        }

        for (var j = 0; j < supportedVideoFormats.length; j++) {
            filePicker.fileTypeFilter.append(supportedVideoFormats[j]);
        }

        filePicker.pickMultipleFilesAsync().then(function (selectedFiles) {
            if (selectedFiles.length > 0) {
                WinJS.log && WinJS.log(selectedFiles.length + " file(s) selected", "sample", "status");
                playlist = selectedFiles;

                // Now that we have a playlist ready, allow the user to control media playback via 
                // system media transport controls.  We enable it now even if one or more files may 
                // turn out unable to load and play, that way the user can still invoke Next/Previous
                // to go to another item in the playlist to try playing.
                systemMediaControls.isEnabled = true;

                // For the design of this sample scenario, we will always automatically start playback after
                // the user has selected new files to play.  Set the HTML5 audio/video element's autoplay
                // attribute to true, so that playback will automatically start after we set a new source on
                // the element.
                player.autoplay = true;
                setNewMediaItem(0);               // Start with first file in the list of picked files.
            } else {
                // user canceled the picker
                WinJS.log && WinJS.log("no files selected", "sample", "status");
            }
        });
    }

    /// <summary>
    /// Used to update all position/duration related information to the SystemMediaTransport controls.
    /// Can be used when some state changes (new track, position changed) or periodically (every 5 seconds or so) 
    /// to keep the system in sync with this apps playback state. 
    /// </summary>
    function updateSmtcPosition() {
        var timelineProperties = new Windows.Media.SystemMediaTransportControlsTimelineProperties();

        // This is a simple scenario that supports seeking, therefore start time and min seek are both 0 and 
        // end time and max seek are both the duration. This allows the system to suggest seeking anywhere in the track.
        // Position is obviously the current media elements position.

        // Note: More complex scenarios may alter more of these values, such as only allowing seeking in a section of the content,
        // by setting min and max seek differently to start and end. For other scenarios such as live playback, end time may be 
        // updated frequently too.    
        timelineProperties.startTime = 0;
        timelineProperties.minSeekTime = 0;

        if (player != null) {
            // The below properties are in seconds, multiply them to make them milliseconds. 
            timelineProperties.position = player.currentTime * 1000;
            timelineProperties.maxSeekTime = player.duration * 1000;
            timelineProperties.endTime = player.duration * 1000;
        }

        systemMediaControls.updateTimelineProperties(timelineProperties);
    }

    // Updating SystemMediaTransportControls' playbackStatus property with accurate information is important; 
    // for example, it determines whether the system media transport controls UI will show a play vs pause 
    // software button, and whether hitting the play/pause toggle key on the keyboard will translate to a 
    // Play vs a Pause buttonpressed event.
    //
    // To update playbackStatus accurately, listen for events like "playing" and "pause" from the HTML5 
    // audio/video element itself.  Even if you implement your own UI controls for your media player instead
    // of using the default transport controls of the element, listening for events from the element is still
    // recommended.  Windows supports scenarios such as streaming media from the element to a networked device
    // (eg. TV) selected by the user from Devices charm (ie. "Play To"), in which case the user may pause and
    // resume media streaming using a TV remote or similar external means.  Thus media-related events from
    // the element may be the only way to get notified of playback status changes in those cases.

    // Sidenote: for the design of this sample, the general idea is that if user is playing media when
    // Next/Previous is invoked, we will automatically start playing the new item once it has loaded.
    // Whereas if the user has paused or stopped media playback when Next/Previous is invoked, we won't 
    // automatically start playing the new item.  In other words, we maintain the user's intent to
    // play or not play across changing the media.  This is most easily handled using the HTML5 audio/
    // video element's "autoplay" attribute.

    // handler for "playing" event from the HTML5 audio/video element
    function playerPlaying() {
        if (isScenario1Active) {
            player.autoplay = true;
            systemMediaControls.playbackStatus = Windows.Media.MediaPlaybackStatus.playing;
            updateSmtcPosition();
            positionUpdateTimerId = setInterval(updateSmtcPosition, 5000);
        }
    }

    // handler for "pause" event from the HTML5 audio/video element
    function playerPaused() {
        if (isScenario1Active) {
            player.autoplay = false;
            systemMediaControls.playbackStatus = Windows.Media.MediaPlaybackStatus.paused;
            updateSmtcPosition();
            clearInterval(positionUpdateTimerId);
        }
    }

    // handler for the "ended" event from the HTML5 audio/video element
    function playerEnded() {
        if (isScenario1Active) {
            if (currentItemIndex < playlist.length - 1) {
                // Current media must've been playing if we received an event about it ending.
                // The design of this sample scenario is to automatically start playing the next
                // item in the playlist.  So we'll set the element's autoplay attribute to true, 
                // then in setNewMediaItem() when we eventually set a new source on the player, 
                // it will automatically playing the new media item.
                player.autoplay = true;
                setNewMediaItem(currentItemIndex + 1);
            } else {
                if (repeatPlaylist) {
                    // Playlist repeat was selected so start again. 
                    player.autoplay = true;
                    setNewMediaItem(0);
                    WinJS.log && WinJS.log("end of playlist, starting playback again at start of playlist", "sample", "status");
                } else {
                    // End of playlist reached.  We'll just stop media playback.
                    player.autoplay = false;
                    stopPlayer();
                    WinJS.log && WinJS.log("end of playlist, stopping playback", "sample", "status");
                    updateSmtcPosition();
                    clearInterval(positionUpdateTimerId);
                }
            }
        }
    }

    // handler for the "error" event from the HTML5 audio/video element
    function playerError() {
        if (isScenario1Active) {
            var errorMessage = "Error playing " + playlist[currentItemIndex].name +
                ".\nPress Next or Previous to continue, or select new files to play.";
            WinJS.log && WinJS.log(errorMessage, "sample", "error");
            systemMediaControls.playbackStatus = Windows.Media.MediaPlaybackStatus.closed;
        }
    }

    // this scenario's event handler for SystemMediaTransportControls' buttonpressed event
    function systemMediaControlsButtonPressed(eventIn) {
        // Check if the user is still on scenario 1, or has started going into 
        // another scenario in the sample.
        if (!isScenario1Active) {
            return;
        }

        var mediaButton = Windows.Media.SystemMediaTransportControlsButton;
        switch (eventIn.button) {
            case mediaButton.play:
                WinJS.log && WinJS.log("Play pressed", "sample", "status");
                player.play();
                break;

            case mediaButton.pause:
                WinJS.log && WinJS.log("Pause pressed", "sample", "status");
                player.pause();
                break;

            case mediaButton.stop:
                WinJS.log && WinJS.log("Stop pressed", "sample", "status");
                stopPlayer();
                break;

            case mediaButton.next:
                WinJS.log && WinJS.log("Next pressed", "sample", "status");
                setNewMediaItem(currentItemIndex + 1);
                break;

            case mediaButton.previous:
                WinJS.log && WinJS.log("Previous pressed", "sample", "status");
                setNewMediaItem(currentItemIndex - 1);
                break;

                // Insert additional case statements for other buttons you want to handle in your app.
                // Remember that you also need to first enable those buttons via the corresponding
                // is****Enabled property on the SystemMediaTransportControls object.
        }
    }

    // this scenario's event handler for SystemMediaTransportControls' buttonpressed event
    function systemMediaControlsPropertyChanged(eventIn) {
        // Check if the user is still on scenario 1, or has started going into 
        // another scenario in the sample.
        if (!isScenario1Active) {
            return;
        }

        if (eventIn.property == Windows.Media.SystemMediaTransportControlsProperty.soundLevel) {
            switch (systemMediaControls.soundLevel) {
                case Windows.Media.SoundLevel.muted:
                    // We've been muted by the system. Pause playback to release resources.
                    if (player != null && !player.paused && !player.ended) {
                        WinJS.log && WinJS.log("Paused due to system mute", "sample", "status");
                        player.pause();
                        pausedDueToMute = true;
                    }
                    break;

                case Windows.Media.SoundLevel.full:
                case Windows.Media.SoundLevel.low:
                    // If we had paused due to system mute, then resume on unmute.
                    if (pausedDueToMute) {
                        WinJS.log && WinJS.log("Unpause due to system unmute", "sample", "status");
                        player.play();
                        pausedDueToMute = false;
                    }
                    break;
            }
        }
    }

    // this scenario's event handler for SystemMediaTransportControls' playbackratechangerequsted event
    function systemMediaControlsPlaybackRateChangeRequested(eventIn) {
        // Check if the user is still on scenario 1, or has started going into 
        // another scenario in the sample.
        if (!isScenario1Active) {
            return;
        }

        // Check to make sure the requested value is in a range we deem appropriate.
        // We then need to turn around and update SMTC so that the system knows the new value.
        if (eventIn.requestedPlaybackRate >= 0 && eventIn.requestedPlaybackRate <= 2) {
            player.playbackRate = eventIn.requestedPlaybackRate;
            systemMediaControls.playbackRate = player.playbackRate;
            WinJS.log && WinJS.log("Playback rate updated", "sample", "status");
        }
    }

    // this scenario's event handler for SystemMediaTransportControls' playbackpositionchangerequsted event
    function systemMediaControlsPlaybackPositionChangeRequested(eventIn) {
        // Check if the user is still on scenario 1, or has started going into 
        // another scenario in the sample.
        if (!isScenario1Active) {
            return;
        }

        // Check to make sure the requested value is in a range we deem appropriate
        //This is a simplified version assumign 0 is the start time, more rgid checks may be needed for offset starts. 
        // We then need to turn around and update SMTC so that the system knows the new value.
        if (eventIn.requestedPlaybackPosition >= 0 && eventIn.requestedPlaybackPosition <= player.duration) {
            if (player.playerState == playerState.started) {
                player.currentTime = eventIn.requestedPlaybackPosition;
                updateSmtcPosition();
                WinJS.log && WinJS.log("Playback Position updated", "sample", "status");
            }
        }
    }

    function systemMediaControlsAutoRepeatModeChangeRequested(eventIn) {
        // Check if the user is still on scenario 1, or has started going into 
        // another scenario in the sample.
        if (!isScenario1Active) {
            return;
        }

        var autoRepeatMode = Windows.Media.MediaPlaybackAutoRepeatMode;
        switch (eventIn.requestedAutoRepeatMode) {
            // Player only supports repeating a track, if we want to repeat a list we need to handle it ourselves

            case autoRepeatMode.None:
                player.loop = false;
                repeatPlaylist = false;
                break;
            case autoRepeatMode.List:
                player.loop = false;
                repeatPlaylist = true;
                break;
            case autoRepeatMode.Track:
                player.loop = true;
                repeatPlaylist = false;
                break;
        }

        WinJS.log && WinJS.log("Auto Repeat Mode updated", "sample", "status");

        // Report back our new state as whatever was requested, seeing as we always honor it. 
        systemMediaControls.autoRepeatMode = eventIn.requestedAutoRepeatMode;
    }

    // For playback of local files, "stop" can be simulated as a pause followed by
    // seeking to beginning.  For other scenarios (eg. live streaming) stop may need
    // to be simulated differently, to be treated same as "pause", or to not support 
    // at all.
    function stopPlayer() {
        player.pause();
        player.currentTime = 0;
        systemMediaControls.playbackStatus = Windows.Media.MediaPlaybackStatus.stopped;
    }

    function getMediaTypeFromFileContentType(storageFile) {
        // Determine the appropriate MediaPlaybackType of the media file based on its 
        // ContentType (ie. MIME type).  The MediaPlaybackType determines the information
        // shown in the system UI for the system media transport controls.  For example, 
        // the copyFromFileAsync() API method will look for different metadata properties 
        // from the file to be extracted for eventual display in the system UI, depending 
        // on the MediaPlaybackType passed to the API.

        // NOTE: MediaPlaybackType.unknown is *not* a valid value to use in 
        // SystemMediaTransportControls APIs.  This method will return 
        // MediaPlaybackType.unknown to indicate no valid MediaPlaybackType exists/can be 
        // determined for the given storageFile.

        var mediaPlaybackType = Windows.Media.MediaPlaybackType.unknown;
        var fileMimeType = storageFile.contentType.toLowerCase();

        if (fileMimeType.indexOf("audio/") === 0) {
            mediaPlaybackType = Windows.Media.MediaPlaybackType.music;
        }
        else if (fileMimeType.indexOf("video/") === 0) {
            mediaPlaybackType = Windows.Media.MediaPlaybackType.video;
        }
        else if (fileMimeType.indexOf("image/") === 0) {
            mediaPlaybackType = Windows.Media.MediaPlaybackType.image;
        }

        return mediaPlaybackType;
    }

    // Updates the system UI for media transport controls to display media metadata
    // (eg. title, album art, etc.) from the given StorageFile object.
    // This is an asynchronous operation and will return a WinJS Promise object capturing
    // the operation.
    function updateSystemMediaControlsDisplayAsync(mediaFile) {
        var updatePromise;
        var mediaType = getMediaTypeFromFileContentType(mediaFile);

        if (mediaType !== Windows.Media.MediaPlaybackType.unknown) {
            // Use the SystemMediaTransportControlsDisplayUpdater's copyFromFileAsync() 
            // API method to try extracting from the StorageFile the relevant media metadata
            // information (eg. track title, artist and album art for MediaPlaybackType.music), 
            // copying them into properties of the displayUpdater and related classes.
            //
            // In place of using copyFromFileAsync() [or, you can't use that method because 
            // your app's media playback scenario doesn't involve StorageFiles], you can also 
            // use other properties in displayUpdater and related classes to explicitly set 
            // the MediaPlaybackType and media metadata properties to values of your choosing.
            //
            // Usage notes:
            //     - the first argument cannot be MediaPlaybackType.unknown
            //     - API method may produce an error on certain file errors from the passed
            //       in StorageFile, such as file-not-found error (eg. file was deleted after 
            //       user had picked it from file picker earlier).
            updatePromise = systemMediaControls.displayUpdater.copyFromFileAsync(mediaType, mediaFile);
        } else {
            // If we are here, it means we are unable to determine a MediaPlaybackType value based on the 
            // StorageFile's contentType (MIME type).  copyFromFileAsync() requires a valid MediaPlaybackType
            // (ie. cannot be MediaPlaybackType.unknown) for the first argument.  One way to handle this is to
            // just pick a valid MediaPlaybackType value to call copyFromFileAsync() with, based on what your
            // app does (eg. MediaPlaybackType.music if your app is a music player).  The MediaPlaybackType
            // directs the API to look for particular sets of media metadata information from the StorageFile,
            // and extraction is best-effort so in worst case simply no metadata information are extracted.  
            //
            // For this sample, we will handle this case the same way as copyFromFileAsync() operation
            // completing with a result of false (operation unsuccessful, handled later here in the function),
            // so we create a dummy Promise that completes with a result of false.
            updatePromise = WinJS.Promise.as(false);
        }

        return updatePromise.then(function (isUpdateSuccessful) {
            return isUpdateSuccessful;   // simply forward the result to the next "then"
        }, function (error) {
            // we're in here if the updatePromise is fulfilled with an error [eg. file
            // not found error in copyFromFileAsync()].  Convert this case to a non-error 
            // fulfillment with a value of false.
            return false;
        }).then(function (isUpdateSuccessful) {
            if (!isScenario1Active) {
                // User may have navigated away from this scenario page to another scenario page 
                // before the async operation completed.
                return;
            }

            if (!isUpdateSuccessful) {
                // For this sample, if copyFromFileAsync() didn't work for us for whatever reasons, 
                // we will just clear displayUpdater of all previously set metadata, if any.  This 
                // helps ensure we don't end up displaying in the system UI for media transport 
                // controls any stale metadata from the media item we were previously playing.
                systemMediaControls.displayUpdater.clearAll();
            }

            // Finally update the system UI display for media transport controls with the new values
            // currently set in the DisplayUpdater, be it via CopyFrmoFileAsync(), ClearAll(), etc.
            systemMediaControls.displayUpdater.update();
        });
    }


    // Performs all necessary actions (including SystemMediaTransportControls related) of loading a
    // new media item in the app for playback.  newItemIndex points to new item in playlist to load
    // for playback; if it is out of range, it will be adjusted accordingly to stay in range.
    function setNewMediaItem(newItemIndex) {
        // enable Next button unless we're on last item of the playlist
        if (newItemIndex >= playlist.length - 1) {
            systemMediaControls.isNextEnabled = false;
            newItemIndex = playlist.length - 1;
        } else {
            systemMediaControls.isNextEnabled = true;
        }

        // enable Previous button unless we're on first item of the playlist
        if (newItemIndex <= 0) {
            systemMediaControls.isPreviousEnabled = false;
            newItemIndex = 0;
        } else {
            systemMediaControls.isPreviousEnabled = true;
        }

        // note that the Play, Pause and Stop buttons were already enabled via 
        // setupSystemMediaTransportControls() invoked during this scenario page's 
        // ready() method

        currentItemIndex = newItemIndex;
        var mediaFile = playlist[currentItemIndex];
        player.src = URL.createObjectURL(mediaFile, { oneTimeOnly: true });

        updateSystemMediaControlsDisplayAsync(mediaFile).done(function (done) {
            // no further actions needed
        }, function (error) {
            WinJS.log && WinJS.log(error.message, "sample", "error");
        });
    }
})();
