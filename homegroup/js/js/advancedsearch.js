//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/AdvancedSearch.html", {
        ready: function (element, options) {
            document.getElementById("searchFilesUser1").addEventListener("click", AdvancedSearchClicked, false);
            document.getElementById("searchFilesUser2").addEventListener("click", AdvancedSearchClicked, false);
            document.getElementById("searchFilesUser3").addEventListener("click", AdvancedSearchClicked, false);
            document.getElementById("searchFilesUser4").addEventListener("click", AdvancedSearchClicked, false);
            document.getElementById("searchProgress").style.visibility = "hidden";
            initAdvancedSearch();
        }
    });

    function AdvancedSearchClicked(sender) {
        WinJS.log && WinJS.log("", "sample", "status");
        document.getElementById("searchProgress").style.visibility = "visible";
        var userToSearch = sender.srcElement.innerHTML;
        var options = new Windows.Storage.Search.QueryOptions(Windows.Storage.Search.CommonFileQuery.orderBySearchRank, ["*"]);
        var outputString = "";

        // Try to open the homegroup folder ---  wrapped in a try/catch in case something really unexcepted happens, like the user is not joined to a homegroup.
        try {
            var hg = Windows.Storage.KnownFolders.homeGroup;
            hg.getFoldersAsync().done(function (users)  {
                users.forEach(function (user) {
                    if (user.name === userToSearch) {
                        var query = user.createFileQueryWithOptions(options);
                        query.getFilesAsync().done(function (files) {
                            if (files.size === 0) {
                                document.getElementById("searchProgress").style.visibility = "hidden";
                                outputString = "<b>No files shared by " + userToSearch + "</b>";
                            }
                            else {
                                document.getElementById("searchProgress").style.visibility = "hidden";
                                outputString = (files.size === 1) ? (files.size + " file found\n") : (files.size + " files shared by ");
                                outputString = outputString.concat(userToSearch, "\n");
                                files.forEach(function (file) {
                                    outputString = outputString.concat(file.name, "\n");
                                });
                            }
                            WinJS.log && WinJS.log(outputString, "sample", "status");
                        });
                    }
                });
            });
        }
        catch (e) {
            document.getElementById("searchProgress").style.visibility = "hidden";
            WinJS.log && WinJS.log(e.message, "sample", "errror");
        }
    }

    // Scenario 4 initialization.  Find the homegroup users and their thumbnails and draw them as input options.
    function initAdvancedSearch() {
        var idString = "";

        // Hide all the buttons until we determine how many users are currently online
        for (var  buttonIndex = 0; buttonIndex < 4; buttonIndex++) {
            idString = "searchFilesUser" + (buttonIndex + 1);
            document.getElementById(idString).style.visibility = "hidden";
        }

        try {
            var hg = Windows.Storage.KnownFolders.homeGroup;
            // Enumerate the homegroup first-level folders, which represent the homegroup users, and get the thumbnails.
            hg.getFoldersAsync().done(function (folders) {
                if (folders) {
                    // Run through all the users and create the buttons for each user.
                    var userCount = (folders.size < 4) ? folders.size : 4;
                    for (var userIndex = 0; userIndex < userCount; userIndex++) {
                        idString = "searchFilesUser" + (userIndex + 1);
                        document.getElementById(idString).innerHTML = folders.getAt(userIndex).name;
                        document.getElementById(idString).style.visibility = "visible";
                    }
                }
                else {
                    id("scenario4Output").innerHTML = "No homegroup users could be found.  Make sure you are joined to a homegroup and there is someone sharing";
                }
            });
        }
        catch (e) {
            WinJS.log && WinJS.log(e.message, "sample", "error");
        }
    }
})();
