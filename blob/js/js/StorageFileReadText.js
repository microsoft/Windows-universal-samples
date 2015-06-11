//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/StorageFileReadText.html", {
        ready: function (element, options) {
            document.getElementById("reset").addEventListener("click", /*@static_cast(EventListener)*/reset, false);
            document.getElementById("readText").addEventListener("click", /*@static_cast(EventListener)*/readText, false);
            reset();
        },

        unload: function () {
            reset();
        }
    });

    function reset() {
        WinJS.log && WinJS.log("", "sample", "error");
        actionsChangeVisibility("hidden");
    }

    function readText() {
        var picker = new Windows.Storage.Pickers.FileOpenPicker();
        picker.fileTypeFilter.replaceAll([".txt"]);
        picker.pickSingleFileAsync().done(function (file) {
            // Check that the picker returned a file. The picker will return null if the user clicked cancel.
            if (file) {
                // create the FileReader object to read the file
                var fileReader = new FileReader();

                // setup the handlers to respond to events from the FileReader
                fileReader.onload = fileLoaded;
                fileReader.onerror = fileError;

                fileReader.readAsText(file);

                actionsChangeVisibility("visible");
            }
            else {
                WinJS.log && WinJS.log("No file was selected", "sample", "status");
            }
        }, BlobSample.asyncError);
    }

    function fileLoaded(evt) {
        WinJS.log && WinJS.log("Read file: " + evt.target.result, "sample", "status");
    }

    function fileError(evt) {
        WinJS.log && WinJS.log("There was a problem reading the file", "sample", "status");
    }

    function actionsChangeVisibility(value) {
        document.getElementById("reset").style.visibility = value;
    }
})();
