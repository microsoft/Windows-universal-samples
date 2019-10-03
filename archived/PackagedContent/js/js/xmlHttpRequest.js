//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/xmlHttpRequest.html", {
        ready: function (element, options) {
            document.getElementById("xhrRemote").addEventListener("click", xhrRemote, false);
            document.getElementById("xhrLocal").addEventListener("click", xhrLocal, false);
        }
    });

    function xhrRemote() {
        makeXhrCall("http://rss.msnbc.msn.com/id/3032127/device/rss/rss.xml", xhrParseXml, true);
    }

    function xhrLocal() {
        makeXhrCall("feed.xml", xhrParseXml, false);
    }

    // This is how to customize caching behavior for XHR.
    // The default caching behavior of XHR is disabled for stream data and enabled for other response types.
    // When caching is disabled, writing of the data to disk is avoided, which can result in better 
    // performance and battery life if the same data is not expected to be downloaded multiple times.
    function disableCacheForXhr(request) {
        request.msCaching = "disabled";
    }

    function makeXhrCall(url, callback, disableCaching) {
        WinJS.log && WinJS.log("", "sample", "status");
        document.getElementById("response").innerHTML = "";

        WinJS.xhr({ url: url, customRequestInitializer: disableCaching ? disableCacheForXhr : null }).done(
            function (result) {
                callback(result.responseXML, result.status);
            },
            function (result) {
                callback(null, result.status);
            });
    }

    function xhrParseXml(xml, statusCode) {
        if (xml) {
            var items = xml.querySelectorAll("rss > channel > item");
            if (items) {
                var /*@override*/ length = Math.min(10, items.length);
                for (var i = 0; i < length; i++) {
                    var link = document.createElement("a");
                    var newLine = document.createElement("br");
                    link.setAttribute("href", items[i].querySelector("link").textContent);
                    link.innerText = (i + 1) + ") " + items[i].querySelector("title").textContent;
                    document.getElementById("response").appendChild(link);
                    document.getElementById("response").appendChild(newLine);
                }
            } else {
                WinJS.log && WinJS.log("There are no items available at this time", "sample", "status");
            }
        } else {
            WinJS.log && WinJS.log("Unable to retrieve data at this time. Status code: " + statusCode, "sample", "error");
        }
    }
})();
