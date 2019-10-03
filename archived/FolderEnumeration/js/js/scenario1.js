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
            document.getElementById("getFilesAndFolders").addEventListener("click", getFilesAndFolders, false);
        }
    });

    // Get the Pictures library and enumerate all its files and folders
    function getFilesAndFolders() {
        clearOutput();
        var group;
        Windows.Storage.KnownFolders.getFolderForUserAsync(null /* current user */, Windows.Storage.KnownFolderId.picturesLibrary).then(function (picturesLibrary) {
            // Create a group for the Pictures library
            group = outputResultGroup(picturesLibrary.name);
            return picturesLibrary.getItemsAsync();
        }).done(function (items) {
            // Output all contents under the library group
            outputItems(group, items);
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
    }
})();
