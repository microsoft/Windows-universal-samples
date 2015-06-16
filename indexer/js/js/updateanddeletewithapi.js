//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/updateAndDeleteWithAPI.html", {
        ready: function (element, options) {
            document.getElementById("addToIndex").addEventListener("click", IndexerHelpers.addItemsToIndex, false);
            document.getElementById("updateName").addEventListener("click", updateItemName, false);
            document.getElementById("deleteKeywords").addEventListener("click", deleteKeywords, false);
            document.getElementById("deleteSingleItem").addEventListener("click", deleteSingleItem, false);
            document.getElementById("deleteMultipleItems").addEventListener("click", deleteMultipleItems, false);
            document.getElementById("deleteAllItems").addEventListener("click", deleteAllItems, false);
        }
    });

    // Initializes the value used throughout the sample which tracks the expected index revision number.
    // This value is used to check if the app's expected index revision number matches the actual index revision number and
    // is stored in the app's localSettings to allow it to persist across termination.
    var appData = Windows.Storage.ApplicationData.current,
        localSettings = appData.localSettings;
    if (!localSettings.values["expectedIndexRevision"]) {
        localSettings.values["expectedIndexRevision"] = 0;
    }
    
    // This function updates the ItemNameDisplay property for the indexed item with the key "SampleKey0".  The
    // updated ItemNameDisplay property comes from the itemNameInput text box.
    function updateItemName() {
        var indexer = Windows.Storage.Search.ContentIndexer.getIndexer(), // Retrieves an instance of the ContentIndexer
            itemKeyValue = "SampleKey0",
            itemNameValue = document.getElementById("itemNameInput").value,
            content = new Windows.Storage.Search.IndexableContent();
        content.id = itemKeyValue;
        content.properties.insert(Windows.Storage.SystemProperties.itemNameDisplay, itemNameValue);
        var output;
        indexer.retrievePropertiesAsync(itemKeyValue, [Windows.Storage.SystemProperties.itemNameDisplay]).then(function (map) { // Retrieves the ItemNameDisplay property for the indexed item based on the itemKey
            var originalRetrievedItemName = map[Windows.Storage.SystemProperties.itemNameDisplay];
            output = "Original item name: " + originalRetrievedItemName;
            localSettings.values["expectedIndexRevision"]++; // Update the expected index revision number in expectation of the updateAsync operation succeeding 
            return indexer.updateAsync(content); // Updates the indexed item whose key is itemKeyValue with the defined properties (in this case, setting ItemNameDisplay to the entered value).  Any property which exists will be replaced if it is included in the properties variable.  Any property not specified will be left untouched.
        }).then(function () {
            localSettings.values["expectedIndexRevision"] = indexer.revision; // Set the expectedIndexRevision to be the reported index revision number because the updateAsync operations succeeded
            return indexer.retrievePropertiesAsync(itemKeyValue, [Windows.Storage.SystemProperties.itemNameDisplay]);
        }).done(function (map) {
            var retrievedItemName = map[Windows.Storage.SystemProperties.itemNameDisplay];
            output += "\nNew item name: " + retrievedItemName;
            WinJS.log && WinJS.log(output, "sample", "status");
        }, function (err) {
            WinJS.log && WinJS.log("Exception thrown! Did you try to retrieve or update a property on a non-existent item?\n" + err.toString(), "sample", "error");
        });
    }

    // This function deletes the Keywords property for the indexed item with the key "SampleKey0"
    function deleteKeywords() {
        var indexer = Windows.Storage.Search.ContentIndexer.getIndexer(), // Retrieves an instance of the ContentIndexer
            itemKeyValue = "SampleKey0",
            content = new Windows.Storage.Search.IndexableContent();
        content.id = itemKeyValue;
        content.properties.insert(Windows.Storage.SystemProperties.keywords, null); // To delete an indexed property, update it to be null
        var output;
        indexer.retrievePropertiesAsync(itemKeyValue, [Windows.Storage.SystemProperties.keywords]).then(function (map) {
            var originalRetrievedKeywords = map[Windows.Storage.SystemProperties.keywords];
            output = "Original keywords: " + IndexerHelpers.createKeywordString(originalRetrievedKeywords);
            localSettings.values["expectedIndexRevision"]++; // Update the expected index revision number in expectation of the updateAsync operation succeeding
            return indexer.updateAsync(content); // Updates the indexed item whose key is itemKeyValue with the defined properties (in this case, setting Keywords to null).  Any property which exists will be replaced if it is included in the properties variable.  Any property not specified will be left untouched.
        }).then(function () {
            localSettings.values["expectedIndexRevision"] = indexer.revision; // Set the expectedIndexRevision to be the reported index revision number because the updateAsync operations succeeded
            return indexer.retrievePropertiesAsync(itemKeyValue, [Windows.Storage.SystemProperties.keywords]);
        }).done(function (map) {
            var retrievedKeywords = map[Windows.Storage.SystemProperties.keywords];
            output += "\nNew keywords: " + IndexerHelpers.createKeywordString(retrievedKeywords);
            WinJS.log && WinJS.log(output, "sample", "status");
        }, function (err) {
            WinJS.log && WinJS.log("Exception thrown! Did you try to retrieve or update a property on a non-existent item?\n" + err.toString(), "sample", "error");
        });
    }

    // This function deletes the indexed item with the key "SampleKey0" added to the index by this app via the ContentIndexer
    function deleteSingleItem() {
        var indexer = Windows.Storage.Search.ContentIndexer.getIndexer(), // Retrieves an instance of the ContentIndexer
            itemKeyValue = "SampleKey0",
            query = indexer.createQuery("*", [Windows.Storage.SystemProperties.itemNameDisplay]);
        var output;
        query.getCountAsync().then(function (count) {
            output = "Original number of items: " + count;
            localSettings.values["expectedIndexRevision"]++; // Update the expected index revision number in expectation of the deleteAsync operation succeeding
            return indexer.deleteAsync(itemKeyValue); // Deletes the item from the index whose key is itemKeyValue
        }).then(function () {
            localSettings.values["expectedIndexRevision"] = indexer.revision; // Set the expectedIndexRevision to be the reported index revision number because the delete operation succeeded
            query = indexer.createQuery("*", [Windows.Storage.SystemProperties.itemNameDisplay]);
            return query.getCountAsync();
        }).done(function (count) {
            output += "\nNew number of items: " + count;
            WinJS.log && WinJS.log(output, "sample", "status");
        });
    }

    // This function deletes the indexed items with the keys "SampleKey1" and "SampleKey2" added to the index by this app via the ContentIndexer
    function deleteMultipleItems() {
        var indexer = Windows.Storage.Search.ContentIndexer.getIndexer(), // Retrieves an instance of the ContentIndexer
            query = indexer.createQuery("*", [Windows.Storage.SystemProperties.itemNameDisplay]);
        var output;
        query.getCountAsync().then(function (count) {
            output = "Original number of items: " + count;
            localSettings.values["expectedIndexRevision"]++; // Update the expected index revision number in expectation of the deleteMultipleAsync operation succeeding
            return indexer.deleteMultipleAsync(["SampleKey1", "SampleKey2"]); // Deletes the item from the index whose key is "SampleKey1" or "SampleKey2"
        }).then(function () {
            localSettings.values["expectedIndexRevision"] = indexer.revision; // Set the expectedIndexRevision to be the reported index revision number because the deleteMultipleAsync operation succeeded
            query = indexer.createQuery("*", [Windows.Storage.SystemProperties.itemNameDisplay]);
            return query.getCountAsync();
        }).done(function (count) {
            output += "\nNew number of items: " + count;
            WinJS.log && WinJS.log(output, "sample", "status");
        });
    }

    // This function deletes all items added to the index by this app via the ContentIndexer
    function deleteAllItems() {
        var indexer = Windows.Storage.Search.ContentIndexer.getIndexer(), // Retrieves an instance of the ContentIndexer
            query = indexer.createQuery("*", [Windows.Storage.SystemProperties.itemNameDisplay]);
        var output;
        query.getCountAsync().then(function (count) {
            output = "Original number of items: " + count;
            localSettings.values["expectedIndexRevision"]++; // Update the expected index revision number in expectation of the deleteAllAsync operation succeeding
            return indexer.deleteAllAsync(); // Deletes all items added to the index by this app via the ContentIndexer
        }).then(function () {
            localSettings.values["expectedIndexRevision"] = indexer.revision; // Set the expectedIndexRevision to be the reported index revision number because the deleteAllAsync operation succeeded
            query = indexer.createQuery("*", [Windows.Storage.SystemProperties.itemNameDisplay]);
            return query.getCountAsync();
        }).done(function (count) {
            output += "\nNew number of items: " + count;
            WinJS.log && WinJS.log(output, "sample", "status");
        });
    }
})();