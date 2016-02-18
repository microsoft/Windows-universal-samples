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
            document.getElementById("getByMonth").addEventListener("click", getByMonth, false);
            document.getElementById("getByRating").addEventListener("click", getByRating, false);
            document.getElementById("getByTag").addEventListener("click", getByTag, false);
        }
    });

    function getByMonth() {
        clearOutput();
        // Get the Pictures library, then rearrange it by month
        Windows.Storage.KnownFolders.getFolderForUserAsync(null /* current user */, Windows.Storage.KnownFolderId.picturesLibrary).then(function (picturesLibrary) {
            var query = picturesLibrary.createFolderQuery(Windows.Storage.Search.CommonFolderQuery.groupByMonth);
            // Retrieve the month groups and display them
            return query.getFoldersAsync();
        }).done(outputFoldersAndFiles);
    }

    function getByRating() {
        clearOutput();
        // Get the Pictures library, then rearrange it by rating
        Windows.Storage.KnownFolders.getFolderForUserAsync(null /* current user */, Windows.Storage.KnownFolderId.picturesLibrary).then(function (picturesLibrary) {
            // Retrieve the rating groups and display them
            var query = picturesLibrary.createFolderQuery(Windows.Storage.Search.CommonFolderQuery.groupByRating);
            return query.getFoldersAsync();
        }).done(outputFoldersAndFiles);
    }

    function getByTag() {
        clearOutput();
        // Get the Pictures library, then rearrange it by tag
        Windows.Storage.KnownFolders.getFolderForUserAsync(null /* current user */, Windows.Storage.KnownFolderId.picturesLibrary).then(function (picturesLibrary) {
            // Retrieve the tag groups and display them
            var query = picturesLibrary.createFolderQuery(Windows.Storage.Search.CommonFolderQuery.groupByTag);
            return query.getFoldersAsync();
        }).done(outputFoldersAndFiles);
    }

    function outputFoldersAndFiles(folders) {
        // Add all file retrieval promises to an array of promises
        var promises = folders.map(function (folder) {
            return folder.getFilesAsync();
        });
        // Aggregate the results of multiple asynchronous operations
        // so that they are returned after all are completed. This
        // ensures that all groups are displayed in order.
        WinJS.Promise.join(promises).done(function (folderContents) {
            for (var i in folderContents) {
                // Create a group for each of the file groups
                var group = outputResultGroup(folders.getAt(i).name);
                // Add the group items in the group
                outputItems(group, folderContents[i]);
            }
        });
    }

    function outputResultGroup(groupname) {
        var outputElement = document.getElementById("itemsList");
        var outputGroup = document.createElement("div");

        // Create the header for the group
        var headerElement = document.createElement("h2");
        headerElement.setAttribute("class", "outputHeader");
        headerElement.textContent = groupname;

        // Setup a list of elements to appear under this group
        var listElement = document.createElement("ul");
        listElement.setAttribute("class", "outputList");

        // Append to the DOM and return group to be populated
        outputGroup.appendChild(headerElement);
        outputGroup.appendChild(listElement);
        outputElement.appendChild(outputGroup);
        return { header: headerElement, list: listElement };
    }

    function outputItems(group, items) {
        // Update the group header with the number of items in the group
        group.header.textContent += " (" + items.size + ")";
        // Add each item as an element in the group's list
        items.forEach(function (item) {
            var listItemElement = document.createElement("li");
            if (item.isOfType(Windows.Storage.StorageItemTypes.folder)) {
                listItemElement.textContent = item.name + "\\";
            } else {
                listItemElement.textContent = item.name;
            }
            group.list.appendChild(listItemElement);
        });
    }

    function clearOutput() {
        document.getElementById("itemsList").innerHTML = "";
        WinJS.log && WinJS.log("", "sample", "status");
    }
})();
