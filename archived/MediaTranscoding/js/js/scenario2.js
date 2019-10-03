//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var g_useMp4 = true,
        g_outputType = "MP4",
        g_outputFileExtension= ".mp4",
        g_outputFileName = "TranscodeSampleOutput";

    var g_inputFile = null,
        g_outputFile = null;

    var g_profile,
        g_transcodeOp;

    var page = WinJS.UI.Pages.define("/html/scenario2.html", {
        ready: function (element, options) {
            id("pickFileButton").addEventListener("click", pickFile, false);
            id("pickDestinationButton").addEventListener("click", pickSave, false);
            id("targetFormat").addEventListener("change", onTargetFormatChanged, false);
            id("transcode").addEventListener("click", onTranscode, false);
            id("cancel").addEventListener("click", transcodeCancel, false);

            var profile = Windows.Media.MediaProperties.MediaEncodingProfile.createMp4(Windows.Media.MediaProperties.VideoEncodingQuality.wvga);
            id("VideoW").value = profile.video.width;
            id("VideoH").value = profile.video.height;
            id("VideoBR").value = profile.video.bitrate;
            id("VideoFR").value = ((profile.video.frameRate.numerator / profile.video.frameRate.denominator).toString().substr(0, 5));
            id("AudioBPS").value = profile.audio.bitsPerSample;
            id("AudioCC").value = profile.audio.channelCount;
            id("AudioBR").value = profile.audio.bitrate;
            id("AudioSR").value = profile.audio.sampleRate;
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
        getCustomProfile();

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


    function getCustomProfile() {
        if (g_useMp4) {
            g_profile = Windows.Media.MediaProperties.MediaEncodingProfile.createMp4(Windows.Media.MediaProperties.VideoEncodingQuality.wvga);
        } else {
            g_profile = Windows.Media.MediaProperties.MediaEncodingProfile.createWmv(Windows.Media.MediaProperties.VideoEncodingQuality.wvga);
        }

        g_profile.audio.bitsPerSample = id("AudioBPS").value;
        g_profile.audio.channelCount = id("AudioCC").value;
        g_profile.audio.bitrate = id("AudioBR").value;
        g_profile.audio.sampleRate = id("AudioSR").value;
        g_profile.video.width = id("VideoW").value;
        g_profile.video.height = id("VideoH").value;
        g_profile.video.bitrate = id("VideoBR").value;
        g_profile.video.frameRate.numerator = id("VideoFR").value;
        g_profile.video.frameRate.denominator = 1;
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
        id("pickFileButton").disabled = false;
        id("pickDestinationButton").disabled = false;
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
            try {
                if (file) {
                    g_outputFile = file;

                    id("transcode").disabled = false;

                }
            } catch (e) {
                WinJS.log && WinJS.log(e.message, "sample", "error");
            }
        });
    }

    function onTargetFormatChanged() {
        if (id("targetFormat").selectedIndex === 0) {
            g_outputFileExtension = ".mp4";
            g_outputType = "MP4";
            g_useMp4 = true;
        } else {
            g_outputFileExtension = ".wmv";
            g_outputType = "WMV";
            g_useMp4 = false;
        }
    }
})();
