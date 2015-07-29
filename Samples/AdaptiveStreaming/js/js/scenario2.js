//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario2.html", {
        ready: function (element, options) {
            document.getElementById("clip_selection").addEventListener("change", onClipSelection, false);
            document.getElementById("setsrc_button").addEventListener("click", setSource, false);
            document.getElementById("clear_button").addEventListener("click", clearEvents, false);
            document.getElementById("bitrate_selection").addEventListener("change", onInitialBitrateSelection, false);
            document.getElementById("min_bitrate_button").addEventListener("click", setDesiredMinBitrate, false);
            document.getElementById("max_bitrate_button").addEventListener("click", setDesiredMaxBitrate, false);
            document.getElementById("bw_measurement_button").addEventListener("click", setBwMeasurmentWindow, false);
        }
    });

    var AdaptiveStreaming = Windows.Media.Streaming.Adaptive;
    var url = null;
    var mediaSource = null;

    var event_id = 1;

    var availableBitrates = null;

    var isInitialBitrateSet = false;
    var initialBitrate;

    var minBitrate = null;
    var maxBitrate = null;
    var bwMeasurementWindow = null;

    function secondsToMiliseconds(seconds)
    {
        return seconds * 1000;
    }

    function setBwMeasurmentWindow() {
        try {
            if (mediaSource != null) {
                var windowSize = document.getElementById("bw_measurement_textbox").value;
                if (windowSize != "")
                {
                    mediaSource.inboundBitsPerSecondWindow = secondsToMiliseconds(windowSize);
                }
                else
                {
                    //default is 30 seconds
                    mediaSource.inboundBitsPerSecondWindow = secondsToMiliseconds(30);
                }

                bwMeasurementWindow = windowSize;
                logEvent("Inbound bits per second window set to " + mediaSource.inboundBitsPerSecondWindow + " miliseconds");
            }
            else
            {
                WinJS.log && WinJS.log("Adaptive Media Source is not set.", "sample", "error");
            }
        } catch (e)
        {
            logEvent("EXCEPTION: " + e.toString());
        }
    }

    function setDesiredMinBitrate()
    {
        try {
            if (mediaSource != null)
            {
                var bitrate = document.getElementById("min_bitrate_textbox").value;
                if (bitrate != "")
                {
                    mediaSource.desiredMinBitrate = bitrate;
                }
                else
                {
                    mediaSource.desiredMinBitrate = null;
                }

                minBitrate = bitrate;
                logEvent("Min bitrate set to " + mediaSource.desiredMinBitrate);
            }
            else
            {
                WinJS.log && WinJS.log("Adaptive Media Source is not set.", "sample", "error");
            }
        }
        catch (e)
        {
            logEvent("EXCEPTION: " + e.toString());
        }
    }

    function setDesiredMaxBitrate()
    {
        try {
            if (mediaSource != null)
            {
                var bitrate = document.getElementById("max_bitrate_textbox").value;
                if (bitrate != "")
                {
                    mediaSource.desiredMaxBitrate = bitrate;
                }
                else
                {
                    mediaSource.desiredMaxBitrate = null;
                }
                maxBitrate = bitrate;
                logEvent("Max bitrate set to " + mediaSource.desiredMaxBitrate);
            }
            else
            {
                WinJS.log && WinJS.log("Adaptive Media Source is not set.", "sample", "error");
            }
        }
        catch (e)
        {
            logEvent("EXCEPTION: " + e.toString());
        }
    }

    function onInitialBitrateSelection()
    {
        isInitialBitrateSet = true;

        var initialBitrateSelection;

        var initial_bitrate_selection = document.getElementById("bitrate_selection");

        for (var i = 0; i < initial_bitrate_selection.options.length; ++i)
        {
            if (initial_bitrate_selection.options[i].selected)
            {
                initialBitrate = initial_bitrate_selection.options[i].value;
            }
        }
        loadMediaSourceFromUri();
        
    }

    function populateInitialBitrate()
    {

        var bitrate_selection = document.getElementById("bitrate_selection");
        if (bitrate_selection == null)
            return;

        bitrate_selection.options.length = 1;
        
        if (mediaSource == null)
        {
            WinJS.log && WinJS.log("Adaptive Media Source is not set.", "sample", "error");
        }
        else
        {
            availableBitrates.forEach(function (bitrate) {
                var option = document.createElement("option");
                option.text = bitrate;
                option.value = bitrate;

                bitrate_selection.add(option);
            });
            
        }
    }

    function clearEvents()
    {
        var events_textarea = document.getElementById("events_textarea");
        events_textarea.value = "";
        event_id = 1;
    }

    function logEvent(str)
    {
        var events_textarea = document.getElementById("events_textarea");
        if (events_textarea != null) {
            events_textarea.value += "[" + event_id + "] " + str + "\n";
            event_id++;
        }
    }

    function onClipSelection()
    {
        var src_textbox = document.getElementById("src_textbox");
        var clip_selection = document.getElementById("clip_selection");

        for (var i = 0; i < clip_selection.options.length; ++i)
        {
            if (clip_selection.options[i].selected)
            {
                src_textbox.value = clip_selection.options[i].value;
            }
        }
    }

    function mapAdaptiveMediaSourceCreationStatus(status)
    {
        var strings = ["Success",
                       "ManifestDownloadFailure",
                       "ManifestParseFailure",
                       "UnsupportedManifestContentType",
                       "UnsupportedManifestVersion",
                       "UnsupportedManifestProfile",
                       "UnknownFailure"];

        if (status < 0 || status > strings.length)
        {
            return "Unknown AdaptiveMediaSourceCreationStatus";
        }

        return strings[status];
    }

    function attachMediaSource()
    {
        try
        {
            if (mediaSource != null)
            {
                var vid = document.getElementById("video_player2");

                vid.src = URL.createObjectURL(mediaSource, { oneTimeOnly: true });
                logEvent("Set media element src to the AdaptiveMediaSource for url: " + url);
            }
        }
        catch (e)
        {
            logEvent("EXCEPTION: " + e.toString());
        }
    }

    function onMediaSourceCreated(result)
    {
        if (result.status === AdaptiveStreaming.AdaptiveMediaSourceCreationStatus.success)
        {

            logEvent("AdaptiveMediaSource.createFromUriAsync completed with status: " + result.status + " - " + mapAdaptiveMediaSourceCreationStatus(result.status));
            mediaSource = result.mediaSource;
            if (isInitialBitrateSet)
            {
                mediaSource.initialBitrate = initialBitrate;
                isInitialBitrateSet = false;
            }

            logEvent("Selecting bitrate: " + mediaSource.initialBitrate);

            var bitrates = "";
            availableBitrates = mediaSource.availableBitrates;
            availableBitrates.forEach(function (bitrate)
            {
                if (bitrates == "")
                {
                    bitrates = bitrate;
                } else
                {
                    bitrates += ", " + bitrate;
                }

            });

            logEvent("Available bitrates: " + bitrates);

            populateInitialBitrate();

            mediaSource.desiredMinBitrate = minBitrate;

            mediaSource.desiredMaxBitrate = maxBitrate;

            if (bwMeasurementWindow != null)
            {
                mediaSource.inboundBitsPerSecondWindow = bwMeasurementWindow;
            }

            attachMediaSource();

        }
        else
        {
            var errorString = "";
            var httpResponseMessage = result.httpResponseMessage;

            if (httpResponseMessage != null)
            {
                errorString = " (HTTP response: " + httpResponseMessage.statusCode + " - " + httpResponseMessage.reasonPhrase;

                if (httpResponseMessage.isSuccessStatusCode &&
                    result.status == AdaptiveStreaming.AdaptiveMediaSourceCreationStatus.unsupportedManifestContentType &&
                    httpResponseMessage.content != null)
                {
                    errorString += "; Content-Type: " + httpResponseMessage.content.headers.contentType;
                }

                errorString += ")";

            }
            logEvent("Failed to create adaptive media source: " + mapAdaptiveMediaSourceCreationStatus(result.status) + errorString);
        }
    }

    function loadMediaSourceFromUri()
    {
        try
        {
            var src_textbox = document.getElementById("src_textbox");
            var vid = document.getElementById("video_player2");

            url = new Windows.Foundation.Uri(src_textbox.value);

            mediaSource = null;
            vid.removeAttribute("src");

            WinJS.log && WinJS.log("Creating AdaptiveMediaSource for url: " + url, "sample", "status");
            AdaptiveStreaming.AdaptiveMediaSource.createFromUriAsync
            AdaptiveStreaming.AdaptiveMediaSource.createFromUriAsync(url).done(
                function completed(result)
                {
                    onMediaSourceCreated(result);
                });

        }
        catch (e)
        {
            WinJS.log && WinJS.log("EXCEPTION: " + e.toString(), "sample", "error");
        }
    }

    function setSource()
    {
        loadMediaSourceFromUri();
    }


})();
