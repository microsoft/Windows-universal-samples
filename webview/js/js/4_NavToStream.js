//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/4_NavToStream.html", {
        ready: function (element, options) {
            // Copy the file "html\simple_example.html" from this package's installed location to a new
            // file "NavigateToStream\simple_example.html" in the local state folder.  This copy will be
            // loaded into the WebView control.
            Windows.Storage.ApplicationData.current.localFolder.createFolderAsync("NavigateToStream", Windows.Storage.CreationCollisionOption.openIfExists).then(function (stateFolder) {
                Windows.ApplicationModel.Package.current.installedLocation.getFileAsync("html\\simple_example.html").then(function (htmlFile) {
                    return htmlFile.copyAsync(stateFolder, "simple_example.html", Windows.Storage.CreationCollisionOption.failIfExists);
                });
            }).done(function () {
                document.getElementById("loadButtonCS").addEventListener("click", navigateToStreamWithCSResolver, false);
                document.getElementById("loadButtonCPP").addEventListener("click", navigateToStreamWithCppResolver, false);
            }, function (error) {
                WinJS.log && WinJS.log("Couldn't create HTML file in local app state folder", "sample", "error");
            });
        }
    });

    function navigateToStreamWithCSResolver() {
        var contentUri = document.getElementById("webview").buildLocalStreamUri("NavigateToStream", "simple_example.html");
        var uriResolver = new SDK.WebViewSampleCS.StreamUriResolver();
        document.getElementById("webview").navigateToLocalStreamUri(contentUri, uriResolver);
    }

    function navigateToStreamWithCppResolver() {
        var contentUri = document.getElementById("webview").buildLocalStreamUri("NavigateToStream", "simple_example.html");
        var uriResolver = new SDK.WebViewSampleCpp.StreamUriResolver();
        document.getElementById("webview").navigateToLocalStreamUri(contentUri, uriResolver);
    }

})();
