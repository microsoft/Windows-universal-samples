//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var searchProgress;
    var page = WinJS.UI.Pages.define("/html/scenario2-search.html", {
        ready: function (element, options) {
            document.getElementById("searchHomeGroup").addEventListener("click", searchHomeGroup, false);
            searchProgress = document.getElementById("searchProgress");
            searchProgress.style.visibility = "hidden";
        }
    });

    function searchHomeGroup() {
        // Clear the status of any previous output
        WinJS.log && WinJS.log("", "sample", "status");

        // Make our progress ring visible, since our search over the network could take a while.
        searchProgress.style.visibility = "visible";

        // Set our query options: We want the results sorted by relevance and then date modified.
        // orderBySearchRank gives us this view.
        var options = new Windows.Storage.Search.QueryOptions(Windows.Storage.Search.CommonFileQuery.orderBySearchRank, ["*"]);

        // Set the query filter provided by the user.
        options.userSearchFilter = document.getElementById("queryString").value;

        // Wrap our search query over the network in a try/catch in case something really unexcepted happens,
        // such as the user attempting to access the HomeGroup node while not joined to a HomeGroup.
        try {
            // Everything we need to query has been supplied.
            // Execute it and collect the results.
            var fileQuery = Windows.Storage.KnownFolders.homeGroup.createFileQueryWithOptions(options);
            
            fileQuery.getFilesAsync().done(function (files) {
                // Remove the progress ring
                searchProgress.style.visibility = "hidden";

                // Report results
                var outputString = "Files found: " + files.size + "\n";
                files.forEach(function (file) {
                    outputString = outputString.concat(file.name, "\n");
                });
                WinJS.log && WinJS.log(outputString, "sample", "status");
            }, function(e) {
                // Something bad happened, so let's show and log an error
                searchProgress.style.visibility = "hidden";
                WinJS.log && WinJS.log(e, "sample", "error");
            });
        } catch (e) {
            // Something bad happened, so let's show and log an error
            searchProgress.style.visibility = "hidden";
            WinJS.log && WinJS.log(e.message, "sample", "error");
        }
    }
})();
