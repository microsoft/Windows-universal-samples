//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/scenario1.html", {
        ready: function (element, options) {
            document.getElementById("search").addEventListener("click", search, false);
        }
    });

    function search() {
        // Initiate the div elements
        var outputDiv = document.getElementById("output");
        var queryBox = document.getElementById("queryBox");

        // Retrieve the query entered in the textbox
        var searchFilter = queryBox.value;

        // Create a new file query from the music library and apply the AQS filter
        Windows.Storage.KnownFolders.getFolderForUserAsync(null /* current user */, Windows.Storage.KnownFolderId.musicLibrary).then(function (musicLibrary) {
            var options = new Windows.Storage.Search.QueryOptions(Windows.Storage.Search.CommonFileQuery.orderBySearchRank, ["*"]);
            options.userSearchFilter = searchFilter;
            var fileQuery = musicLibrary.createFileQueryWithOptions(options);
            return fileQuery.getFilesAsync();
        })
        .done(function (files) {
            if (files.size === 0) {
                outputDiv.innerText = "No files found for '" + searchFilter + "'";
            } else {
                // Create an output string to hold results count and filenames
                var filesLabel = (files.size === 1) ? "file" : "files";
                var output = files.size + " " + filesLabel + " found\n\n";

                // Iterate through the results and print each filename to the output field
                files.forEach(function (file) {
                    output += file.name + "\n";
                });
                outputDiv.innerText = output;
            }
        });
    }
})();
