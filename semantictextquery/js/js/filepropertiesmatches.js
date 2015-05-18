//// Copyright (c) Microsoft Corporation. All rights reserved
(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/filePropertiesMatches.html", {
        ready: function (element, options) {
            document.getElementById("find").addEventListener("click", searchOnPicturesLibraryAndDisplayResults, false);
        }
    });

    function searchOnPicturesLibraryAndDisplayResults() {
        var outputDiv = document.getElementById("output");
        var searchFilter = queryBox.value;
        if (searchFilter === "") {
            return;
        }

        // Create a new file query from the pictures library and apply the AQS filter
        var picturesLibrary = Windows.Storage.KnownFolders.picturesLibrary;
        var options = new Windows.Storage.Search.QueryOptions(Windows.Storage.Search.CommonFileQuery.orderBySearchRank, ["*"]);
        options.userSearchFilter = searchFilter;
        options.setPropertyPrefetch(Windows.Storage.FileProperties.PropertyPrefetchOptions.documentProperties, []);

        var fileQuery = picturesLibrary.createFileQueryWithOptions(options);

        fileQuery.getFilesAsync().done(function (files) {
            if (files.size > 0) {

                // Create an output string to hold results count and filenames
                var filesLabel = (files.size === 1) ? "file" : "files";
                var output = "<b>" + SdkSample.highlightString(files.size + " " + filesLabel + " found") + "</b><br><br>";
                
                // Print all the file names for the results and highlight any matches on the filename property
                files.forEach(function (file) {
                    var searchHits = fileQuery.getMatchingPropertiesWithRanges(file);
                    var newString = "";

                    // If one of the hits we found in on the filename we'll highlight the file name
                    if (searchHits.hasKey("System.FileName")) {
                        newString += SdkSample.highlightString(file.name, searchHits.lookup("System.FileName"));
                    } else {
                        newString += SdkSample.highlightString(file.name);
                    }

                    output += newString + "<br/><br/>";
                });
                outputDiv.innerHTML = output;
            } else {
            outputDiv.innerText = "There were no matching files in your Pictures library";
            }
        });
    }
})();

