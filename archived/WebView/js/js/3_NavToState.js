//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/3_NavToState.html", {
        ready: function (element, options) {
            // Copy the file "html\simple_example.html" from this package's installed location to a new
            // file "NavigateToState\simple_example.html" in the local state folder.  This copy will be
            // loaded into the WebView control.
            Windows.Storage.ApplicationData.current.localFolder.createFolderAsync("NavigateToState", Windows.Storage.CreationCollisionOption.openIfExists).then(function (stateFolder) {
                Windows.ApplicationModel.Package.current.installedLocation.getFileAsync("html\\simple_example.html").then(function (htmlFile) {
                    return htmlFile.copyAsync(stateFolder, "simple_example.html", Windows.Storage.CreationCollisionOption.failIfExists);
                });
            }).done(function () {
                document.getElementById("loadButton").addEventListener("click", navigateToState, false);
            }, function (error) {
                WinJS.log && WinJS.log("Couldn't create HTML file in local app state folder", "sample", "error");
            });
        }
    });

    function navigateToState() {
        document.getElementById("webview").navigate("ms-appdata:///local/NavigateToState/simple_example.html");
    }

})();
