//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var searchProgress;
    var page = WinJS.UI.Pages.define("/html/scenario4-searchUser.html", {
        ready: function (element, options) {
            searchProgress = document.getElementById("searchProgress");
            searchProgress.style.visibility = "hidden";
            initAdvancedSearch();
        }
    });

    function searchFolder(folder) {
        WinJS.log && WinJS.log("", "sample", "status");
        searchProgress.style.visibility = "visible";
        var options = new Windows.Storage.Search.QueryOptions(Windows.Storage.Search.CommonFileQuery.orderBySearchRank, ["*"]);

        // Try to open the HomeGroup folder.
        // Wrap in a try/catch in case something really unexcepted happens, like the user leave the HomeGroup
        // after we created the button.
        try {
            var query = folder.createFileQueryWithOptions(options);
            query.getFilesAsync().done(function (files) {
                searchProgress.style.visibility = "hidden";
                var outputString = "Files shared by " + folder.name + ": " + files.size + "\n";
                files.forEach(function (file) {
                    outputString = outputString.concat(file.name, "\n");
                });
                WinJS.log && WinJS.log(outputString, "sample", "status");
            });
        } catch (e) {
            searchProgress.style.visibility = "hidden";
            WinJS.log && WinJS.log(e.message, "sample", "errror");
        }
    }

    // Find the HomeGroup users and create a button for each one.
    function initAdvancedSearch() {
        try {
            var homeGroup = Windows.Storage.KnownFolders.homeGroup;
            // Enumerate the HomeGroup first-level folders, which represent the HomeGroup users.
            homeGroup.getFoldersAsync().done(function (folders) {
                if (folders && folders.size) {
                    var buttonContainer = document.getElementById("buttonContainer");
                    folders.forEach(function (folder) {
                        var button = document.createElement("button");
                        WinJS.Utilities.addClass(button, "win-button");
                        button.innerText = folder.name;
                        button.addEventListener("click", function() { searchFolder(folder); }, false);
                        buttonContainer.appendChild(button);
                        buttonContainer.appendChild(document.createTextNode(" "));
                    });
                } else {
                    WinJS.log && WinJS.log("No HomeGroup users could be found.  Make sure you are joined to a HomeGroup and there is someone sharing", "sample", "error");
                }
            });
        } catch (e) {
            WinJS.log && WinJS.log(e.message, "sample", "error");
        }
    }
})();
