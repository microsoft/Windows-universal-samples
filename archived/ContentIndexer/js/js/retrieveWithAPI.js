//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/retrieveWithAPI.html", {
        ready: function (element, options) {
            document.getElementById("addToIndex").addEventListener("click", IndexerHelpers.addItemsToIndex, false);
            document.getElementById("retrieveAllItems").addEventListener("click", retrieveAllItems, false);
            document.getElementById("retrieveMatchingItems").addEventListener("click", retrieveMatchingItems, false);
        }
    });

    // This function retrieves all items added to the index by this app via the ContentIndexer
    function retrieveAllItems() {
        var indexer = Windows.Storage.Search.ContentIndexer.getIndexer(); // Retrieve an instance of the ContentIndexer
        var query = indexer.createQuery("*", [Windows.Storage.SystemProperties.itemNameDisplay, Windows.Storage.SystemProperties.keywords, Windows.Storage.SystemProperties.comment]); // Queries the index for all items by doing a * search and retrieve the ItemNameDisplay, Keywords, and Comment properties
        var output;
        query.getAsync().done(function (indexedItems) { // Retrieve an array of IIndexableContent-based runtimeclasses based on the query
            output = createItemString(indexedItems);
            WinJS.log && WinJS.log(output, "sample", "status");
        });
    }

    // This function retrieves all items added to the index by this app via the ContentIndexer which
    // contain the word "0" in either the ItemNameDisplay, Keywords, or Comment properties.
    function retrieveMatchingItems() {
        var indexer = Windows.Storage.Search.ContentIndexer.getIndexer(); // Retrieve an instance of the ContentIndexer
        var query = indexer.createQuery("0", [Windows.Storage.SystemProperties.itemNameDisplay, Windows.Storage.SystemProperties.keywords, Windows.Storage.SystemProperties.comment]); // Queries the index for all items containing a "0" and retrieve the ItemNameDisplay, Keywords, or Comment properties
        var output;
        query.getAsync().done(function (indexedItems) { // Retrieve an array of StorageItems based on the query
            output = createItemString(indexedItems);
            WinJS.log && WinJS.log(output, "sample", "status");
        });
    }

    function createItemString(indexedItemArray) {
        var output;
        if (indexedItemArray) {
            output = "";
            for (var i = 0, len = indexedItemArray.length; i < len; i++) {
                var retrievedItemName = indexedItemArray[i].properties[Windows.Storage.SystemProperties.itemNameDisplay],
                    retrievedItemComment = indexedItemArray[i].properties[Windows.Storage.SystemProperties.comment],
                    retrievedItemKey = indexedItemArray[i].id,
                    retrievedItemKeywords = indexedItemArray[i].properties[Windows.Storage.SystemProperties.keywords];
                output += "Key: " + retrievedItemKey;
                output += "\nName: " + retrievedItemName;
                output += "\nKeywords: " + IndexerHelpers.createKeywordString(retrievedItemKeywords);
                output += "\nComment: " + retrievedItemComment;
                if (i < len - 1) {
                    output += "\n\n";
                }
            }
        }
        return output;
    }
})();