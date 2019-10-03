//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/addWithAPI.html", {
        ready: function (element, options) {
            document.getElementById("addToIndex").addEventListener("click", addItemToIndex, false);
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

    // This function creates an instance of the ContentIndexer and inserts a single item using the
    // Key, Name, Keywords, and Comment specified in the text boxes.
    function addItemToIndex() {
        var itemKeyValue = document.getElementById("itemKeyInput").value;
        if (itemKeyValue === "") {
            WinJS.log && WinJS.log("You must add an item key to insert an item into the index.", "sample", "error");
        }
        else {
            var indexer = Windows.Storage.Search.ContentIndexer.getIndexer(), // Retrieve an instance of the ContentIndexer
                itemNameValue = document.getElementById("nameInput").value,
                itemKeywordsValue = document.getElementById("keywordsInput").value,
                itemCommentValue = document.getElementById("commentInput").value,
                itemContentValue = document.getElementById("contentInput").value,
                itemLanguageValue = document.getElementById("languageInput").value,
                content = new Windows.Storage.Search.IndexableContent();
            var output;
            var contentStream = new Windows.Storage.Streams.InMemoryRandomAccessStream();
            var contentWriter = new Windows.Storage.Streams.DataWriter(contentStream);
            contentWriter.writeString(itemContentValue);
            contentWriter.storeAsync().then(function () {
                content.id = itemKeyValue;
                if (itemLanguageValue) { // A language has been specified, so add the properties to the index along with the specified BCP-47 language code
                    var itemNameValueAndLanguage = new Windows.Storage.Search.ValueAndLanguage(),
                        itemKeywordsValueAndLanguage = new Windows.Storage.Search.ValueAndLanguage(),
                        itemCommentValueAndLanguage = new Windows.Storage.Search.ValueAndLanguage();
                    itemNameValueAndLanguage.language = itemLanguageValue;
                    itemNameValueAndLanguage.value = itemNameValue;
                    itemKeywordsValueAndLanguage.language = itemLanguageValue;
                    itemKeywordsValueAndLanguage.value = itemKeywordsValue;
                    itemCommentValueAndLanguage.language = itemLanguageValue;
                    itemCommentValueAndLanguage.value = itemCommentValue;
                    content.properties.insert(Windows.Storage.SystemProperties.itemNameDisplay, itemNameValueAndLanguage);
                    content.properties.insert(Windows.Storage.SystemProperties.keywords, itemKeywordsValueAndLanguage);
                    content.properties.insert(Windows.Storage.SystemProperties.comment, itemCommentValueAndLanguage);
                } else { // No language has been specified, so add the properties to the index without any language information
                    content.properties.insert(Windows.Storage.SystemProperties.itemNameDisplay, itemNameValue);
                    content.properties.insert(Windows.Storage.SystemProperties.keywords, itemKeywordsValue);
                    content.properties.insert(Windows.Storage.SystemProperties.comment, itemCommentValue);
                }
                contentStream.seek(0);
                content.stream = contentStream;
                content.streamContentType = "text/plain";
                localSettings.values["expectedIndexRevision"]++; // Update the expected index revision number in expectation of the addAsync operation succeeding
                return indexer.addAsync(content);
            }).then(function () {
                localSettings.values["expectedIndexRevision"] = indexer.revision; // Set the expectedIndexRevision to be the reported index revision number because the addAsync operation succeeded
                contentStream.close();
                contentWriter.close();
                return indexer.retrievePropertiesAsync(itemKeyValue, [Windows.Storage.SystemProperties.itemNameDisplay, Windows.Storage.SystemProperties.keywords, Windows.Storage.SystemProperties.comment]); // Items are retrieved from the index by passing in the key for the item and the properties you'd like to retrieve
            }).done(function (map) {
                output = createItemString(map, itemKeyValue);
                WinJS.log && WinJS.log(output, "sample", "status");
            });
        }
    }

    function createItemString(propertyArray, itemKey) {
        var retrievedItemName = propertyArray[Windows.Storage.SystemProperties.itemNameDisplay];
        var retrievedItemComment = propertyArray[Windows.Storage.SystemProperties.comment];
        var retrievedItemKeywords = propertyArray[Windows.Storage.SystemProperties.keywords];
        var output = "Key: " + itemKey;
        output += "\nName: " + retrievedItemName;
        output += "\nKeywords: " + IndexerHelpers.createKeywordString(retrievedItemKeywords);
        output += "\nComment: " + retrievedItemComment;
        return output;
    }
})();