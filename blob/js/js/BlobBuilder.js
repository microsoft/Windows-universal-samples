//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/BlobBuilder.html", {
        ready: function (element, options) {
            document.getElementById("reset").addEventListener("click", /*@static_cast(EventListener)*/reset, false);
            document.getElementById("buildBlob").addEventListener("click", /*@static_cast(EventListener)*/buildBlob, false);
            reset();
        },

        unload: function () {
            reset();
        }
    });

    function reset() {
        document.getElementById("blobBuilderString1").value = "String1";
        document.getElementById("blobBuilderString2").value = "String2";
        WinJS.log && WinJS.log("", "sample", "error");
        actionsChangeVisibility("hidden");
    }

    function buildBlob() {

        // build our Blob with two strings
        var builder = new MSBlobBuilder();
        builder.append(document.getElementById("blobBuilderString1").value);
        builder.append(document.getElementById("blobBuilderString2").value);
        var blob = builder.getBlob();

        // create the FileReader object to read the blob
        var fileReader = new FileReader();

        // setup the handlers to respond to events from the FileReader
        fileReader.onload = fileLoaded;
        fileReader.onerror = fileError;

        fileReader.readAsText(blob);

        actionsChangeVisibility("visible");
    }

    function fileLoaded(evt) {
        WinJS.log && WinJS.log("Read blob: " + evt.target.result, "sample", "status");
    }

    function fileError(evt) {
        WinJS.log && WinJS.log("There was a problem reading the blob", "sample", "status");
    }

    function actionsChangeVisibility(value) {
        document.getElementById("reset").style.visibility = value;
    }
})();
