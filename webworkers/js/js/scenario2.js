//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario2.html", {
        ready: function (element, options) {
            document.getElementById("sendXHR").addEventListener("click", sendXMLHttpRequest, false);
            document.getElementById("clearXHRLog").addEventListener("click", function () {
                document.getElementById("xhrLog").innerHTML = "";
            }, false);

        },
        unload: function () {
            stopListenToWorker();
        }
    });

    var xhrWorker;
    
    function stopListenToWorker() {
        if (xhrWorker) {
            xhrWorker.onmessage = null;
        }
    }

    function sendXMLHttpRequest() {
        if (!xhrWorker) {
            xhrWorker = new Worker('js/xhr.js');
        }
        xhrWorker.onmessage = function (e) {
            document.getElementById("xhrLog").innerHTML += e.data + "<br>";
        };
        xhrWorker.postMessage({
            url: "http://www.microsoft.com/",
        });
    }

})();
