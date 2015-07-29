//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario3.html", {
        ready: function (element, options) {
            document.getElementById("clip_selection").addEventListener("change", onClipSelection, false);
            document.getElementById("setsrc_button").addEventListener("click", setSource, false);
        }
    });

    var AdaptiveStreaming = Windows.Media.Streaming.Adaptive;
    var url = null;
    var mediaSource = null;
    var event_id = 1;

    function onDownloadRequested(event)
    {
        try
        {
            // rewrite key URIs to replace http:// with https://
            if (event.resourceType === AdaptiveStreaming.AdaptiveMediaSourceResourceType.key)
            {
                var originalUri = args.resourceUri;
                var secureUri = originalUri.replace("http:", "https:");

                //override the URI by setting property on the result sub object
                args.result.resourceUri = new Windows.Foundation.Uri(secureUri);
            }
        }
        catch (e)
        {
            logEvent("EXCEPTION: " + e.toString());
        }
    }

    function onDownloadCompleted(event)
    {
        if (event.resourceByteRangeOffset == null)
        {
            logEvent("Download completed for " + mapResourceType(event.resourceType)
                + " URI: " + event.resourceUri);
        }
        else
        {
            logEvent("Byte range completed for " + mapResourceType(event.resourceType)
                + " URI: " + event.resourceUri + " offset: " + event.resourceByteRangeOffset
                + " length: " + event.resourceByteRangeLength);
        }
    }

    function onDownloadFailed(event)
    {
        if (event.resourceByteRangeOffset == null) {
            logEvent("Download failed for " + mapResourceType(event.resourceType)
                + " URI: " + event.resourceUri);
        }
        else {
            logEvent("Byte range failed for " + mapResourceType(event.resourceType)
                + " URI: " + event.resourceUri + " offset: " + event.resourceByteRangeOffset
                + " length: " + event.resourceByteRangeLength);
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

    function mapResourceType(resourceType) 
    {
        var strings = ["Manifest",
                       "INIT segment",
                       "MEDIA segment",
                       "Key",
                       "IV"];

        if (resourceType < 0 || resourceType > strings.length) 
        {
            return "Unknown Resource Type";
        }

        return strings[resourceType];
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
                var vid = document.getElementById("video_player3");

                vid.src = URL.createObjectURL(mediaSource, { oneTimeOnly: true });
                WinJS.log && WinJS.log("Set media element src to the AdaptiveMediaSource for url: " + url, "sample", "status");
            }
        }
        catch (e)
        {
            WinJS.log && WinJS.log("EXCEPTION: " + e.toString(), "sample", "error");
        }
    }

    function onMediaSourceCreated(result)
    {
        if (result.status === AdaptiveStreaming.AdaptiveMediaSourceCreationStatus.success)
        {

            WinJS.log && WinJS.log("AdaptiveMediaSource.createFromUriAsync completed with status: " + result.status + " - " + mapAdaptiveMediaSourceCreationStatus(result.status), "sample", "status");
            mediaSource = result.mediaSource;
            attachMediaSource();

            //Register for download requested, success, and failure events
            mediaSource.addEventListener("downloadrequested", onDownloadRequested, false);
            mediaSource.addEventListener("downloadcompleted", onDownloadCompleted, false);
            mediaSource.addEventListener("downloadfailed", onDownloadFailed, false);
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
            WinJS.log && WinJS.log("Failed to create adaptive media source: " + mapAdaptiveMediaSourceCreationStatus(result.status) + errorString, "sample", "error");
        }
    }

    function loadMediaSourceFromUri()
    {
        try
        {
            var src_textbox = document.getElementById("src_textbox");
            var vid = document.getElementById("video_player3");

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
