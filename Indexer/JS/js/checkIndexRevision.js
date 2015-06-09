//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/checkIndexRevision.html", {
        ready: function (element, options) {
            document.getElementById("checkIndexRevision").addEventListener("click", checkIndexRevision, false);
        }
    });

    // Initializes the value used throughout the sample which tracks the expected index revision number.
    // This value is used to check if the app's expected index revision number matches the actual index revision number and
    // is stored in the app's localSettings to allow it to persist across termination.
    var appData = Windows.Storage.ApplicationData.current;
    var localSettings = appData.localSettings;
    if (!localSettings.values["expectedIndexRevision"]) {
        localSettings.values["expectedIndexRevision"] = 0;
    }

    // This function displays the app's expected index revision number and the actual index revision number reported by the index.
    // For purposes of this sample, this is done in reaction to a button being pressed.  In practice, this
    // should be done when an app is activated.
    // If the reported index revision number is 0, it means the index has been reset and an app should re-push all of its data.
    // If the reported index revision number is not 0, but doesn't match the app's expected index revision number, it indicates that not all
    // of the app's adds/deletes/updates were successfully processed.  This should be an edge case, but when hit,
    // the app should re-do the missed operations.  Tracking the expected index revision number can be used to create
    // checkpoints in the app so it can only re-do the operations required to match the expected index revision number.
    function checkIndexRevision() {
        var indexer = Windows.Storage.Search.ContentIndexer.getIndexer(); // Retrieve an instance of the ContentIndexer
        var output = "Expected Index Revision Number: " + localSettings.values["expectedIndexRevision"] + "\n";
        output += "Reported Index Revision Number: " + indexer.revision;
        if (localSettings.values["expectedIndexRevision"] !== indexer.revision) {
            // There is a mismatch between the expected and reported index revision numbers
            if (indexer.revision === 0) {
                // The index has been reset, so code would be added here to re-push all data
            } else {
                // The index hasn't been reset, but it doesn't contain all expected updates, so
                // add code to get the index back into the expected state.
            }
            localSettings.values["expectedIndexRevision"] = indexer.revision; // After doing the necessary work to get back to a synchronized state, set the expected index revision number to match the reported revision number
        }
        WinJS.log && WinJS.log(output, "sample", "status");
    }
})();