//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

/// <reference path="Windows.Storage.js"/>
/// <reference path="Blob.js"/>
/// <reference path="XMLhttpRequest.js"/>


(function () {
    "use strict";
    function receiveMessage(messageEvent) {
        reset();
        
        if (messageEvent.origin !== "ms-appx://" + document.location.hostname) {
            return;
        }

        var url = "../images/placeholder-sdk.png";
        if (typeof (messageEvent.data) === "object") {

            document.getElementById("statusMessage").innerText = "Successfully sent a blob via postMessage to the web context.";
            
            // Retrieve blob from the event object
            var blob = messageEvent.data;
            url = URL.createObjectURL(blob, {oneTimeOnly: true});
        }

        document.getElementById("imageHolder").src = url;
       
    }
    
    function reset() {
        document.getElementById("statusMessage").innerHTML = "";
    }
    function initialize() {
        window.addEventListener("message", /*@static_cast(EventListener)*/receiveMessage, false);
    }

    document.addEventListener("DOMContentLoaded", initialize, false);

})();
