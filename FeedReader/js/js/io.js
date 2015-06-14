//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    // URI to the feedCollection.json file that contains data of all the feeds.
    var feedCollectionJSON = "ms-appx:///feedCollection.json";

    // The filename of the file containing the selected feed indices.
    var selectionIndicesJSON = "selectionIndices.json";

    // These are the sample's default subscriptions.
    var defaultSubscriptions = ["Engadget", "Windows App Builder Blog"];

    function loadStateAsync() {
        /// <summary>
        /// Sets up the state of the app (loads the feed collection, the selected feed indices,
        /// and sets up the list views and subscribes to the selected feeds).
        /// </summary>
        /// <returns>Promise which loads the state of the app.</returns>
        return new WinJS.Promise(function (complete, error) {
            // Load the feed collection.
            loadFeedCollectionAsync().then(function () {
                // Setup the feed collection for the Subscriptions page.
                Render.setupFeedCollectionListView();

                // Load the selection indices, after the feed collection is loaded.
                return loadSelectionIndicesAsync();
            }).done(function () {
                // Subscribe to the selected feeds.
                Render.selectionIndices.forEach(function (index) {
                    var feed = Render.getFeedObject(index);

                    if (feed) {
                        Data.subscribe(feed);
                    }
                });

                complete();
            }, function () {
                // If there is a problem loading the feed collection, propagate the error.
                error();
            });
        });
    }

    function saveSelectionIndicesAsync() {
        /// <summary>
        /// Saves the current selection of feed indices.
        /// </summary>
        /// <returns>
        /// Returns a promise which saves the current selection of feed indices.
        /// </returns>
        return WinJS.Application.local.writeText(selectionIndicesJSON, JSON.stringify(Render.selectionIndices));
    }
    
    function loadFeedCollectionAsync() {
        /// <summary>
        /// Loads the feed collection.
        /// </summary>
        /// <returns>
        /// Returns a promise which creates a feed for every available feed in feedCollectionJSON.
        /// </returns>
        return Windows.Storage.PathIO.readTextAsync(feedCollectionJSON).then(function (feedCollectionString) {
            // If there is an issue parsing the JSON file, the error will be bubbled up.
            var feedCollectionData = JSON.parse(feedCollectionString);
            feedCollectionData.forEach(function (feedData) {
                Data.allFeeds.push(new Data.Feed(feedData));
            });
        });
    }

    function loadSelectionIndicesAsync() {
        /// <summary>
        /// Loads the selected indices.
        /// </summary>
        /// <returns>Returns a promise which loads the selected or default indices.</returns>
        return new WinJS.Promise(function (complete) {
            WinJS.Application.local.exists(selectionIndicesJSON).then(function (exists) {
                if (exists) {
                    WinJS.Application.local.readText(selectionIndicesJSON).then(function (selectionIndicesString) {
                        Render.selectionIndices = JSON.parse(selectionIndicesString);
                        complete();
                    });
                } else {
                    for (var i = 0, len = defaultSubscriptions.length; i < len; i++) {
                        var index = Render.getFeedIndex(defaultSubscriptions[i]);
                        if (index >= 0) {
                            Render.selectionIndices.push(index);
                        }
                    }

                    complete();
                }
            });
        });
    }

    // Public interface.
    WinJS.Namespace.define("IO", {
        loadStateAsync: loadStateAsync,
        saveStateAsync: saveSelectionIndicesAsync
    });
})();
