//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/retrieveWithAppContent.html", {
        ready: function (element, options) {
            document.getElementById("addToIndex").addEventListener("click", IndexerHelpers.addAppContentFilesToIndexedFolder, false);
            document.getElementById("retrieveAllItems").addEventListener("click", retrieveAllItems, false);
            document.getElementById("retrieveMatchedItems").addEventListener("click", retrieveMatchedItems, false);
        }
    });
    
    // This function retrieves all items added to the index through the app's appcontent-ms files
    function retrieveAllItems() {
        var applicationData = Windows.Storage.ApplicationData.current,
            localFolder = applicationData.localFolder;
        var output;
        localFolder.createFolderAsync("Indexed", Windows.Storage.CreationCollisionOption.openIfExists).then(function (indexedFolder) {
            var queryAll = indexedFolder.createFileQuery(Windows.Storage.Search.CommonFileQuery.orderByName); // Queries for all files in the "LocalState\Indexed" folder and sorts the results by name
            return queryAll.getFilesAsync();
        }).then(function (indexedItems) {
            var promiseArray = [];
            output = "";
            for (var i = 0, len = indexedItems.length; i < len; i++) {
                promiseArray[i] = indexedItems[i].properties.retrievePropertiesAsync([Windows.Storage.SystemProperties.itemNameDisplay, Windows.Storage.SystemProperties.comment, Windows.Storage.SystemProperties.keywords, Windows.Storage.SystemProperties.title]).then(function (map) { // Retrieves the ItemNameDisplay, Comment, Keywords, and Title properties for the item
                    output += "Name: " + map[Windows.Storage.SystemProperties.itemNameDisplay];
                    output += "\nKeywords: " + IndexerHelpers.createKeywordString(map[Windows.Storage.SystemProperties.keywords]);
                    output += "\nComment: " + map[Windows.Storage.SystemProperties.comment];
                    output += "\nTitle: " + map[Windows.Storage.SystemProperties.title] + "\n\n";
                });
            }
            return WinJS.Promise.join(promiseArray);
        }).done(function () {
            WinJS.log && WinJS.log(output, "sample", "status");
        });
    }

    // This function retrieves all items added to the index through the app's appcontent-ms files which
    // have an ItemNameDisplay property containing "Sample 1".
    function retrieveMatchedItems() {
        var applicationData = Windows.Storage.ApplicationData.current,
            localFolder = applicationData.localFolder,
            queryOptions = new Windows.Storage.Search.QueryOptions();
        queryOptions.indexerOption = Windows.Storage.Search.IndexerOption.onlyUseIndexer; // Include only items returned via the index in the search results
        queryOptions.applicationSearchFilter = Windows.Storage.SystemProperties.itemNameDisplay + ":\"Sample 1\""; // Create an AQS (Advanced Query Syntax) query which will look for ItemNameDisplay properties which contain "Sample 1"
        var output;
        localFolder.createFolderAsync("Indexed", Windows.Storage.CreationCollisionOption.openIfExists).then(function (indexedFolder) {
            var query = indexedFolder.createFileQueryWithOptions(queryOptions); // Query for files using the options specified above
            return query.getFilesAsync();
        }).then(function (indexedItems) {
            var promiseArray = [];
            output = "";
            for (var i = 0, len = indexedItems.length; i < len; i++) {
                promiseArray[i] = indexedItems[i].properties.retrievePropertiesAsync([Windows.Storage.SystemProperties.itemNameDisplay, Windows.Storage.SystemProperties.comment, Windows.Storage.SystemProperties.keywords, Windows.Storage.SystemProperties.title]).then(function (map) {
                    output += "Name: " + map[Windows.Storage.SystemProperties.itemNameDisplay];
                    output += "\nKeywords: " + IndexerHelpers.createKeywordString(map[Windows.Storage.SystemProperties.keywords]);
                    output += "\nComment: " + map[Windows.Storage.SystemProperties.comment];
                    output += "\nTitle: " + map[Windows.Storage.SystemProperties.title] + "\n\n";
                });
            }
            return WinJS.Promise.join(promiseArray);
        }).done(function () {
            WinJS.log && WinJS.log(output, "sample", "status");
        });
    }
})();