//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario4.html", {
        ready: function (element, options) {
            document.getElementById("clip_selection").addEventListener("change", onClipSelection, false);
            document.getElementById("setsrc_button").addEventListener("click", setSource, false);
        }
    });

    var AdaptiveStreaming = Windows.Media.Streaming.Adaptive;
    var url = null;
    var mediaSource = null;
    var event_id = 1;


    function clearEvents() {
        var events_textarea = document.getElementById("events_textarea");
        events_textarea.value = "";
        event_id = 1;
    }

    function logEvent(str) {
        var events_textarea = document.getElementById("events_textarea");
        if (events_textarea != null) {
            events_textarea.value += "[" + event_id + "] " + str + "\n";
            event_id++;
        }
    }

    function onClipSelection() {
        var src_textbox = document.getElementById("src_textbox");
        var clip_selection = document.getElementById("clip_selection");

        for (var i = 0; i < clip_selection.options.length; ++i) {
            if (clip_selection.options[i].selected) {
                src_textbox.value = clip_selection.options[i].value;
            }
        }
    }

    function mapResourceType(resourceType) {
        var strings = ["Manifest",
                       "INIT segment",
                       "MEDIA segment",
                       "Key",
                       "IV"];

        if (resourceType < 0 || resourceType > strings.length) {
            return "Unknown Resource Type";
        }

        return strings[resourceType];
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
                var vid = document.getElementById("video_player4");
                vid.src = URL.createObjectURL(mediaSource, { oneTimeOnly: true });
                WinJS.log && WinJS.log("Set media element src to the AdaptiveMediaSource for url: " + url, "sample", "status");
            }
        }
        catch (e) {
            WinJS.log && WinJS.log("EXCEPTION: " + e.toString(), "sample", "error");
        }
    }

    function onMediaSourceCreated(result) {
        if (result.status === AdaptiveStreaming.AdaptiveMediaSourceCreationStatus.success) {

            WinJS.log && WinJS.log("AdaptiveMediaSource.createFromUriAsync completed with status: " + result.status + " - " + mapAdaptiveMediaSourceCreationStatus(result.status), "sample", "status");
            mediaSource = result.mediaSource;
            attachMediaSource();

        }
        else {
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
            var vid = document.getElementById("video_player4");
            var src_textbox = document.getElementById("src_textbox");

            url = new Windows.Foundation.Uri(src_textbox.value);

            mediaSource = null;
            vid.removeAttribute("src");

            WinJS.log && WinJS.log("Creating AdaptiveMediaSource for url: " + url, "sample", "status");
            var eme = new PlayReadyManager(vid);
            AdaptiveStreaming.AdaptiveMediaSource.createFromUriAsync
            AdaptiveStreaming.AdaptiveMediaSource.createFromUriAsync(url).done(
                function completed(result) {
                    onMediaSourceCreated(result);
                });

        }
        catch (e) {
            WinJS.log && WinJS.log("EXCEPTION: " + e.toString(), "sample", "error");
        }
    }

    function setSource() {
        loadMediaSourceFromUri();
    }

    //==============================================================================
    // EME Support
    //==============================================================================

    function PlayReadyManager(vid) {
        this.vid = vid;

        var that = this;
        vid.addEventListener(this.NEEDKEY_EVENT, function (e) {
            that.needPlayReadyKey(e);
        }, false);
    }

    PlayReadyManager.prototype = {
        NEEDKEY_EVENT: "msneedkey",
        KEYMESSAGE_EVENT: "mskeymessage",
        KEYADDED_EVENT: "mskeyadded",
        KEYERROR_EVENT: "mskeyerror",
        KEY_SYSTEM: "com.microsoft.playready",

        needPlayReadyKey: function (e) {
            var that = this;
            var video = document.getElementById("video_player4");
            logEvent("Received needkey message");

            if (!video.msKeys) {
                logEvent("Creating a new MediaKeys(\"" + this.KEY_SYSTEM + "\")");
                try {
                    video.msSetMediaKeys(new MSMediaKeys(this.KEY_SYSTEM));
                } catch (e) {
                    throw "Unable to create MediaKeys(\"" + this.KEY_SYSTEM + "\"). Verify the components are installed and functional. Original error: " + e.message;
                }
            } else {
                return;
            }

            var session = video.msKeys.createSession("video/mp4", e.initData);
            if (!session) {
                throw "Could not create key session";
            }

            session.addEventListener(this.KEYMESSAGE_EVENT, function (e) {
                logEvent("Processing key message");
                that.downloadPlayReadyKey(e.destinationURL, String.fromCharCode.apply(null, new Uint16Array(e.message.buffer)), function (data) {
                    session.update(data);
                });
            });

            session.addEventListener(this.KEYADDED_EVENT, function () {
                logEvent("Key successfully added");
            });

            session.addEventListener(this.KEYERROR_EVENT, function () {
                throw "Unexpected 'keyerror' event from key session. Code: " + session.error.code + ", systemCode: " + session.error.systemCode;
            });
        },

        downloadPlayReadyKey: function (url, keyMessage, callback) {
            logEvent("Parsing key message XML");
            var keyMessageXML = new DOMParser().parseFromString(keyMessage, "application/xml");

            var challenge;
            if (keyMessageXML.getElementsByTagName("Challenge")[0]) {
                challenge = atob(keyMessageXML.getElementsByTagName("Challenge")[0].childNodes[0].nodeValue);
            } else {
                throw "Can not find <Challenge> in key message";
            }

            var headerNames = keyMessageXML.getElementsByTagName("name");
            var headerValues = keyMessageXML.getElementsByTagName("value");
            if (headerNames.length !== headerValues.length) {
                throw "Mismatched header <name>/<value> pair in key message";
            }

            var xhr = new XMLHttpRequest();
            xhr.open("POST", url);
            xhr.responseType = "arraybuffer";
            xhr.onreadystatechange = function () {
                if (xhr.readyState === 4) {
                    if (xhr.status === 200) {
                        callback(new Uint8Array(xhr.response));
                    } else {
                        throw "XHR failed (" + url + "). Status: " + xhr.status + " (" + xhr.statusText + ")";
                    }
                }
            }
            for (var i = 0; i < headerNames.length; i++) {
                xhr.setRequestHeader(headerNames[i].childNodes[0].nodeValue, headerValues[i].childNodes[0].nodeValue);
            }

            logEvent("Loading PlayReady key from: " + url);
            xhr.send(challenge);
        }
    }


})();
