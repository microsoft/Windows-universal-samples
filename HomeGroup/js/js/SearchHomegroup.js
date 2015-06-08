//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/SearchHomegroup.html", {
        ready: function (element, options) {
            document.getElementById("searchHomegroup").addEventListener("click", searchHomegroup, false);
            document.getElementById("searchProgress").style.visibility = "hidden";
        }
    });

    function searchHomegroup() {
        // Clear the status of any previous output
        WinJS.log && WinJS.log("", "sample", "status");

        // Make our progress ring visible, since our search over the network could take a while and its good to provide the user feedback
        document.getElementById("searchProgress").style.visibility = "visible";

        // Obtain the query term(s) from the user's input that we will use from our input text field
        var query = document.getElementById("queryString").value;

        // Set our query options --- in this case we want the results sorted by relevance and then date modified - orderBySearchRank gives us this view
        var options = new Windows.Storage.Search.QueryOptions(Windows.Storage.Search.CommonFileQuery.orderBySearchRank, ["*"]);

        // Set the query filter, which in this case is the string we're searching on
        options.userSearchFilter = query;

        // We'll build our results up as a block of text to show to the user when we're complete --- so make sure it's empty before we start
        var outputString = "";

        // Wrap our search query over the network in a try/catch in case something really unexcepted happens, such as the user attempting to access the homegroup node while not joined to a homegroup.
        try {
            // Everything we need to query has been supplied --- now execute it and place the query results in a place where it is easy to iterate over them as we build up our output
            var queryResult = Windows.Storage.KnownFolders.homeGroup.createFileQueryWithOptions(options);
            // Check if we received 0 results and show appropriate output and turn off the progress ring
            queryResult.getFilesAsync().done(function (files) {
                if (files.size === 0) {
                    WinJS.log && WinJS.log("No files found for \"" + query + "\"", "sample", "status");
                    document.getElementById("searchProgress").style.visibility = "hidden";
                }
                // Otherwise, we got results sor let's show them --- making sure to turn off the progress ring.
                else {
                    outputString = (files.size === 1) ? (files.size + " file found\n") : (files.size + " files found\n");
                    files.forEach(function (file) {
                        outputString = outputString.concat(file.name, "\n");
                    });
                    WinJS.log && WinJS.log(outputString, "sample", "status");
                    document.getElementById("searchProgress").style.visibility = "hidden";
                }
            });
        }
        catch (e) {
            // Something bad happened, so let's show and log an error
            document.getElementById("searchProgress").style.visibility = "hidden";
            WinJS.log && WinJS.log(e.message, "sample", "error");
        }
    }
})();
