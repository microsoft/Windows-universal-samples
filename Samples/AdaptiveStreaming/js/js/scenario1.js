//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario1.html", {
        ready: function (element, options) {
            document.getElementById("clip_selection").addEventListener("change", onClipSelection, false);
            document.getElementById("setsrc_button").addEventListener("click", setSource, false);
        }
    });

    var AdaptiveStreaming = Windows.Media.Streaming.Adaptive;
    var url = null;
    var mediaSource = null;

    function onClipSelection() {
        var src_textbox = document.getElementById("src_textbox");
        var clip_selection = document.getElementById("clip_selection");

        for (var i = 0; i < clip_selection.options.length; ++i) {
            if (clip_selection.options[i].selected) {
                src_textbox.value = clip_selection.options[i].value;
            }
        }
    }

    function mapAdaptiveMediaSourceCreationStatus(status) {
        var strings = ["Success",
                       "ManifestDownloadFailure",
                       "ManifestParseFailure",
                       "UnsupportedManifestContentType",
                       "UnsupportedManifestVersion",
                       "UnsupportedManifestProfile",
                       "UnknownFailure"];

        if (status < 0 || status > strings.length) {
            return "Unknown AdaptiveMediaSourceCreationStatus";
        }

        return strings[status];
    }

    function attachMediaSource() {
        try {
            if (mediaSource != null) {
                var vid = document.getElementById("video_player1");

                vid.src = URL.createObjectURL(mediaSource, { oneTimeOnly: true });
                WinJS.log && WinJS.log("Set media element src to the AdaptiveMediaSource for url: " + url, "sample", "status");
            }
        } catch (e) {
            WinJS.log && WinJS.log("EXCEPTION: " + e.toString(), "sample", "error");
        }
    }

    function onMediaSourceCreated(result) {
        if (result.status === AdaptiveStreaming.AdaptiveMediaSourceCreationStatus.success) {

            WinJS.log && WinJS.log("AdaptiveMediaSource.createFromUriAsync completed with status: " + result.status + " - " + mapAdaptiveMediaSourceCreationStatus(result.status), "sample", "status");
            mediaSource = result.mediaSource;
            attachMediaSource();

        } else {
            var errorString = "";
            var httpResponseMessage = result.httpResponseMessage;

            if (httpResponseMessage != null) {
                errorString = " (HTTP response: " + httpResponseMessage.statusCode + " - " + httpResponseMessage.reasonPhrase;

                if (httpResponseMessage.isSuccessStatusCode &&
                    result.status == AdaptiveStreaming.AdaptiveMediaSourceCreationStatus.unsupportedManifestContentType &&
                    httpResponseMessage.content != null) {
                    errorString += "; Content-Type: " + httpResponseMessage.content.headers.contentType;
                }

                errorString += ")";

            }
            WinJS.log && WinJS.log("Failed to create adaptive media source: " + mapAdaptiveMediaSourceCreationStatus(result.status) + errorString, "sample", "error");
        }
    }

    function loadMediaSourceFromUri() {
        try {
            var src_textbox = document.getElementById("src_textbox");
            var vid = document.getElementById("video_player1");

            url = new Windows.Foundation.Uri(src_textbox.value);

            mediaSource = null;
            vid.removeAttribute("src");

            WinJS.log && WinJS.log("Creating AdaptiveMediaSource for url: " + url, "sample", "status");
            AdaptiveStreaming.AdaptiveMediaSource.createFromUriAsync
            AdaptiveStreaming.AdaptiveMediaSource.createFromUriAsync(url).done(
                function completed(result) {
                    onMediaSourceCreated(result);
                });

        } catch (e) {
            WinJS.log && WinJS.log("EXCEPTION: " + e.toString(), "sample", "error");
        }
    }

    function setSource() {
        loadMediaSourceFromUri();
    }

    
})();


