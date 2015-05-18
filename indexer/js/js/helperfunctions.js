//// Copyright (c) Microsoft Corporation. All rights reserved


(function () {
    "use strict";
    
    // Initializes the value used throughout the sample which tracks the expected index revision number.
    // This value is used to check if the app's expected index revision number matches the actual index revision number and
    // is stored in the app's localSettings to allow it to persist across termination.
    var appData = Windows.Storage.ApplicationData.current,
        localSettings = appData.localSettings;
    if (!localSettings.values["expectedIndexRevision"]) {
        localSettings.values["expectedIndexRevision"] = 0;
    }

    // This function creates an instance of the ContentIndexer and inserts three items into the index,
    // with generic Key, Name, Keywords, and Comment properties.
    function _addItemsToIndex() {
        var indexer = Windows.Storage.Search.ContentIndexer.getIndexer(), // Retrieve an instance of the ContentIndexer
            numberOfItemsToIndex = 3;
        var promiseArray = [];
        for (var i = 0; i < numberOfItemsToIndex; i++) {
            var itemKeyValue = "SampleKey" + i.toString(),
                itemNameValue = "Sample Item Name " + i.toString(),
                itemKeywordsValue = "Sample keyword " + i.toString(),
                itemCommentValue = "Sample comment " + i.toString(),
                content = new Windows.Storage.Search.IndexableContent();
            content.properties.insert(Windows.Storage.SystemProperties.itemNameDisplay, itemNameValue);
            content.properties.insert(Windows.Storage.SystemProperties.keywords, itemKeywordsValue);
            content.properties.insert(Windows.Storage.SystemProperties.comment, itemCommentValue);
            content.id = itemKeyValue;
            localSettings.values["expectedIndexRevision"]++; // Update the expected index revision number in expectation of the addAsync operation succeeding
            promiseArray[i] = indexer.addAsync(content);
        }
        WinJS.Promise.join(promiseArray).then(function () {
            localSettings.values["expectedIndexRevision"] = indexer.revision; // Set the expectedIndexRevision to be the reported index reivision number because the addAsync operations succeeded
            var query = indexer.createQuery("*", [Windows.Storage.SystemProperties.itemNameDisplay]); // Query the index for all items by doing a * search and retrieve the ItemNameDisplay property
            return query.getCountAsync(); // Retrieve the number of items that the query contains, in this case this represents all items added to the index
        }).done(function (count) {
            var output = "Number of items currently in the index: " + count;
            WinJS.log && WinJS.log(output, "sample", "status");
        });
    }

    // For the purposes of this sample, the appcontent-ms files are stored in an "appcontent-ms" folder
    // in the install directory.  These are then copied into the app's "LocalState\Indexed" folder,
    // which exposes them to the indexer.
    function _addAppContentFilesToIndexedFolder() {
        var localFolder = appData.localFolder,
            appcontentFolder,
            indexedFolder,
            installDirectory = Windows.ApplicationModel.Package.current.installedLocation;
        var output = "Items added to the \"Indexed\" folder:\n";
        installDirectory.getFolderAsync("appcontent-ms").then(function (retrievedAppcontentFolder) {
            appcontentFolder = retrievedAppcontentFolder;
            return localFolder.createFolderAsync("Indexed", Windows.Storage.CreationCollisionOption.openIfExists);
        }).then(function (retrievedIndexedFolder) {
            indexedFolder = retrievedIndexedFolder;
            return appcontentFolder.getFilesAsync(appcontentFolder);
        }).then(function (files) {
            var promiseArray = [];
            for (var i = 0, len = files.length; i < len; i++) {
                promiseArray[i] = files[i].copyAsync(indexedFolder, files[i].name, Windows.Storage.NameCollisionOption.replaceExisting);
                output += files[i].displayName + files[i].fileType;
                if (i < len - 1) {
                    output += "\n";
                }
            }
            return WinJS.Promise.join(promiseArray);
        }).done(function () {
            WinJS.log && WinJS.log(output, "sample", "status");
        });
    }

    function _createKeywordString(keywordArray) {
        var keywordString = "";
        if (keywordArray) {
            for (var i = 0, len = keywordArray.length; i < len; i++) {
                keywordString += keywordArray[i];
                if (i < len - 1) {
                    keywordString += "; ";
                }
            }
        }
        return keywordString;
    }

    WinJS.Namespace.define("IndexerHelpers", {
        addItemsToIndex: _addItemsToIndex,
        addAppContentFilesToIndexedFolder: _addAppContentFilesToIndexedFolder,
        createKeywordString: _createKeywordString
    });
})();