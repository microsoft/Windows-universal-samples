//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var audtag = null;
    var systemMediaControls = null;

    var page = WinJS.UI.Pages.define("/html/speech.html", {
        ready: function (element, options) {
            document.getElementById("button1").addEventListener("click", doSomething5, false);
        },

        unload: function () {

            // Remove the audio tag and then null it.
            // Then unload event listeners so you don't press play on another media element you switched from.

            if (audtag) {
                document.getElementById("MediaElement").removeChild(audtag);
                audtag.removeEventListener("playing", audioPlaying, false);
                audtag.removeEventListener("pause", audioPaused, false);
                audtag = null;
            }
            if (systemMediaControls) {
                systemMediaControls.removeEventListener("buttonpressed", mediaButtonPressed, false);
                systemMediaControls.removeEventListener("propertychanged", mediaPropertyChanged, false);
                systemMediaControls.isPlayEnabled = false;
                systemMediaControls.isPauseEnabled = false;
                systemMediaControls.playbackStatus = Windows.Media.MediaPlaybackStatus.closed;
                systemMediaControls = null;
            }
        }
    });

    function doSomething5() {

        var openPicker = new Windows.Storage.Pickers.FileOpenPicker();
        openPicker.viewMode = Windows.Storage.Pickers.PickerViewMode.list;
        openPicker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.musicLibrary;
        openPicker.fileTypeFilter.replaceAll([".mp3", ".mp4", ".m4a", ".wma", ".wav"]);
        openPicker.pickSingleFileAsync().done(function (file) {
            if (file) {
                // Create the media control.

                systemMediaControls = Windows.Media.SystemMediaTransportControls.getForCurrentView();

                // Add event listeners for PBM notifications to illustrate app is
                // getting a new SoundLevel and pass the audio tag along to the function

                systemMediaControls.addEventListener("propertychanged", mediaPropertyChanged, false);

                // Add event listener for the mandatory media commands and enable them.
                // These are necessary to play streams of type 'backgroundCapableMedia'
                systemMediaControls.addEventListener("buttonpressed", mediaButtonPressed, false);
                systemMediaControls.isPlayEnabled = true;
                systemMediaControls.isPauseEnabled = true;

                var fileLocation = window.URL.createObjectURL(file, { oneTimeOnly: true });

                if (!audtag) {
                    audtag = document.createElement('audio');
                    audtag.setAttribute("id", "audtag");
                    audtag.setAttribute("controls", "true");
                    audtag.setAttribute("msAudioCategory", "Speech");
                    audtag.setAttribute("src", fileLocation);
                    audtag.addEventListener("playing", audioPlaying, false);
                    audtag.addEventListener("pause", audioPaused, false);
                    document.getElementById("MediaElement").appendChild(audtag);
                    audtag.load();
                    WinJS.log && WinJS.log("Audio Tag Loaded", "sample", "status");
                    log(getTimeStampedMessage("test"));
                }

            } else {
                WinJS.log && WinJS.log("Audio Tag Did Not Load Properly", "sample", "error");
            }

        });
    }

    function mediaButtonPressed(e) {
        switch (e.button) {
            case Windows.Media.SystemMediaTransportControlsButton.play:
                // Handle the Play event and print status to screen..
                WinJS.log && WinJS.log("Play Received", "sample", "status");
                audtag.play();
                break;

            case Windows.Media.SystemMediaTransportControlsButton.pause:
                // Handle the Pause event and print status to screen.
                WinJS.log && WinJS.log("Pause Received", "sample", "status");
                audtag.pause();
                break;

            default:
                break;
        }
    }

    function mediaPropertyChanged(e) {
        switch (e.property) {
            case Windows.Media.SystemMediaTransportControlsProperty.soundLevel:
                //Catch SoundLevel notifications and determine SoundLevel state.  If it's muted, we'll pause the player.
                var soundLevel = e.target.soundLevel;

                switch (soundLevel) {

                    case Windows.Media.SoundLevel.muted:
                        log(getTimeStampedMessage("App sound level is: Muted"));
                        break;
                    case Windows.Media.SoundLevel.low:
                        log(getTimeStampedMessage("App sound level is: Low"));
                        break;
                    case Windows.Media.SoundLevel.full:
                        log(getTimeStampedMessage("App sound level is: Full"));
                        break;
                }

                appMuted();
                break;

            default:
                break;
        }
    }

    function audioPlaying() {
        systemMediaControls.playbackStatus = Windows.Media.MediaPlaybackStatus.playing;
    }

    function audioPaused() {
        systemMediaControls.playbackStatus = Windows.Media.MediaPlaybackStatus.paused;
    }

    function appMuted() {

        if (audtag) {
            if (!audtag.paused) {
                audtag.pause();
                WinJS.log && WinJS.log("Audio Paused", "sample", "status");
            }
        }
    }

    function log(msg) {
        var pTag = document.createElement("p");
        pTag.innerHTML = msg;
        document.getElementById("StatusOutput").appendChild(pTag);
    }

    function getTimeStampedMessage(eventCalled) {
        var timeformat = new Windows.Globalization.DateTimeFormatting.DateTimeFormatter("longtime");
        var time = timeformat.format(new Date());

        var message = eventCalled + "\t\t" + time;
        return message;
    }
})();
