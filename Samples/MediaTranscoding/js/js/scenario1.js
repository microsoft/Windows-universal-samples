//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var g_outputType = "MP4",
        g_outputFileExtension = ".mp4",
        g_outputFileName = "TranscodeSampleOutput";

    var g_inputFile = null,
        g_outputFile = null;

    var g_profile,
        g_transcodeOp;

    var page = WinJS.UI.Pages.define("/html/scenario1.html", {
        ready: function (element, options) {
            id("pickFileButton").addEventListener("click", pickFile, false);
            id("pickDestinationButton").addEventListener("click", pickSave, false);
            id("targetFormat").addEventListener("change", onTargetFormatChanged, false);
            id("transcode").addEventListener("click", onTranscode, false);
            id("cancel").addEventListener("click", transcodeCancel, false);
        }
    });

    function onTranscode() {
        stopPlayers();
        disableButtons();

        // Create transcode object.
        var transcoder = null;
        transcoder = new Windows.Media.Transcoding.MediaTranscoder();

        // Set mrfcrf444 processing if specified
        if (id("enableMrfCrf444").checked) {
            transcoder.videoProcessingAlgorithm = Windows.Media.Transcoding.MediaVideoProcessingAlgorithm.mrfCrf444;
        }

        // Get transcode profile.
        getPresetProfile(id("profileSelect"));

        // Clear messages.
        WinJS.log && WinJS.log("", "sample", "status");

        // Transcode.
        g_transcodeOp = null;
        var prepareOp = transcoder.prepareFileTranscodeAsync(g_inputFile, g_outputFile, g_profile);
        prepareOp.done(function (result) {
            if (result.canTranscode) {
                g_transcodeOp = result.transcodeAsync();
                g_transcodeOp.done(transcodeComplete, transcoderErrorHandler, transcodeProgress);
            } else {
                transcodeFailure(result.failureReason);
            }
        }); // prepareOp.done
        id("cancel").disabled = false;
    }


    function getPresetProfile(profileSelect) {
        g_profile = null;
        var mediaProperties = Windows.Media.MediaProperties;
        var videoEncodingProfile;
        switch (profileSelect.selectedIndex) {
            case 0:
                videoEncodingProfile = mediaProperties.VideoEncodingQuality.hd1080p;
                break;
            case 1:
                videoEncodingProfile = mediaProperties.VideoEncodingQuality.hd720p;
                break;
            case 2:
                videoEncodingProfile = mediaProperties.VideoEncodingQuality.wvga;
                break;
            case 3:
                videoEncodingProfile = mediaProperties.VideoEncodingQuality.ntsc;
                break;
            case 4:
                videoEncodingProfile = mediaProperties.VideoEncodingQuality.pal;
                break;
            case 5:
                videoEncodingProfile = mediaProperties.VideoEncodingQuality.vga;
                break;
            case 6:
                videoEncodingProfile = mediaProperties.VideoEncodingQuality.qvga;
                break;
        }

        switch (g_outputType) {
            case "AVI":
                g_profile = mediaProperties.MediaEncodingProfile.createAvi(videoEncodingProfile);
                break;
            case "WMV":
                g_profile = mediaProperties.MediaEncodingProfile.createWmv(videoEncodingProfile);
                break;
            default:
                g_profile = mediaProperties.MediaEncodingProfile.createMp4(videoEncodingProfile);
                break;
        }

        /*
        For transcoding to audio profiles, create the encoding profile using one of these APIs:
            mediaProperties.MediaEncodingProfile.createMp3(audioEncodingProfile)
            mediaProperties.MediaEncodingProfile.createM4a(audioEncodingProfile)
            mediaProperties.MediaEncodingProfile.createWma(audioEncodingProfile)
            mediaProperties.MediaEncodingProfile.createWav(audioEncodingProfile)

        where audioEncodingProfile is one of these presets:
            mediaProperties.AudioEncodingQuality.high
            mediaProperties.AudioEncodingQuality.medium
            mediaProperties.AudioEncodingQuality.low
        */
    }

    function id(elementId) {
        return document.getElementById(elementId);
    }

    function stopPlayers() {
        var video = id("inputVideo");
        if (!video.paused) {
            video.pause();
        }
        video = id("outputVideo");
        if (!video.paused) {
            video.pause();
        }
    }

    function transcoderErrorHandler(error) {
        WinJS.log && WinJS.log(error.message, "sample", "error");
        enableButtons();
        id("cancel").disabled = true;
    }

    function transcodeProgress(percent) {
        WinJS.log && WinJS.log("Progress: " + percent.toString().split(".")[0] + "%", "sample", "status");
    }

    function transcodeComplete(result) {
        WinJS.log && WinJS.log("Transcode completed.", "sample", "status");
        enableButtons();
        id("cancel").disabled = true;
        try {
            id("outputPath").innerHTML = "Output (" + g_outputFile.path + ")";
            var video = id("outputVideo");
            video.src = URL.createObjectURL(g_outputFile, { oneTimeOnly: true });
            video.play();
        }
        catch (e) {
            WinJS.log && WinJS.log(e.message, "sample", "error");
        }
    }

    function transcodeFailure(failureReason) {
        if (g_outputFile !== null) {
            g_outputFile.deleteAsync();
        }

        switch (failureReason) {
            case Windows.Media.Transcoding.TranscodeFailureReason.codecNotFound:
                transcoderErrorHandler(new Error("Codec not found."));
                break;
            case Windows.Media.Transcoding.TranscodeFailureReason.invalidProfile:
                transcoderErrorHandler(new Error("Invalid profile."));
                break;
            case Windows.Media.Transcoding.TranscodeFailureReason.unknown:
                transcoderErrorHandler(new Error("Unknown failure."));
                break;
        }
    }

    function transcodeCancel() {
        if (g_transcodeOp) {
            g_transcodeOp.cancel();
            id("cancel").disabled = true;
            if (g_outputFile !== null) {
                g_outputFile.deleteAsync();
            }
        }
    }

    function enableButtons() {
        id("pickDestinationButton").disabled = false;
        id("pickFileButton").disabled = false;
        id("targetFormat").disabled = false;
        id("transcode").disabled = true;
        id("enableMrfCrf444").disabled = false;
    }

    function disableButtons() {
        id("pickFileButton").disabled = true;
        id("pickDestinationButton").disabled = true;
        id("targetFormat").disabled = true;
        id("transcode").disabled = true;
        id("enableMrfCrf444").disabled = true;
    }

    function enableNonSquarePARProfiles() {
        id("profileSelect")[3].disabled = false;
        id("profileSelect")[4].disabled = false;
    }

    function disableNonSquarePARProfiles() {
        id("profileSelect")[3].disabled = true;
        id("profileSelect")[4].disabled = true;

        // Ensure a valid profile is set
        if ((id("profileSelect").selectedIndex === 3) || (id("profileSelect").selectedIndex === 4)) {
            id("profileSelect").selectedIndex = 2;
        }
    }

    function pickFile() {
        var openPicker = new Windows.Storage.Pickers.FileOpenPicker();
        openPicker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.videosLibrary;
        openPicker.fileTypeFilter.replaceAll([".mp4", ".wmv"]);
        openPicker.pickSingleFileAsync().done(function (file) {
            try {
                if (file) {
                    g_inputFile = file;
                    // Load video on video tag.
                    var video = id("inputVideo");
                    video.src = URL.createObjectURL(file, { oneTimeOnly: true });
                    video.play();
                    // Enable buttons.
                    enableButtons();
                }
            } catch (e) {
                WinJS.log && WinJS.log(e.message, "sample", "error");
            }
        });
    }

    function pickSave() {
        var savePicker = new Windows.Storage.Pickers.FileSavePicker();
        savePicker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.videosLibrary;
        savePicker.fileTypeChoices.insert(g_outputType, [g_outputFileExtension]);
        savePicker.suggestedFileName = g_outputFileName;
        
        savePicker.pickSaveFileAsync().done(function (file) {
            if (file) {
                g_outputFile = file;
                id("transcode").disabled = false;
            }
        });
    }

    function onTargetFormatChanged() {
        if (id("targetFormat").selectedIndex === 2) {
            g_outputFileExtension = ".avi";
            g_outputType = "AVI";

            // Disable NTSC and PAL profiles as non-square pixel aspect ratios are not supported by AVI
            disableNonSquarePARProfiles();
        } else if (id("targetFormat").selectedIndex === 1) {
            g_outputFileExtension = ".wmv";
            g_outputType = "WMV";
            enableNonSquarePARProfiles();
        } else {
            g_outputFileExtension = ".mp4";
            g_outputType = "MP4";
            enableNonSquarePARProfiles();
        }
    }
})();
