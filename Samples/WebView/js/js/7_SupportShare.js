//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/7_SupportShare.html", {
        ready: function (element, options) {
            // Register handler for the "datarequested" event.  This event is fired when the Share charm is used,
            // and the handler will be responsible for creating the data package to be shared.
            Windows.ApplicationModel.DataTransfer.DataTransferManager.getForCurrentView().addEventListener("datarequested", dataRequested);

            document.getElementById("shareButton").disabled = true;
            document.getElementById("launchInBrowserButton").disabled = true;
            document.getElementById("copyToClipboardButton").disabled = true;

            document.getElementById("webview").addEventListener("MSWebViewNavigationCompleted", navigationCompleted);
            document.getElementById("webview").navigate("http://go.microsoft.com/fwlink/?LinkId=294155");

            document.getElementById("shareButton").addEventListener("click", showShareUI, false);
            document.getElementById("launchInBrowserButton").addEventListener("click", launchInBrowser, false);
            document.getElementById("copyToClipboardButton").addEventListener("click", copyToClipboard, false);
        },
        unload: function () {
            // When this scenario is unloaded, remove the handler for the "datarequested" event since
            // sharing is not implemented in the other scenarios.
            var dataTransferManager = Windows.ApplicationModel.DataTransfer.DataTransferManager.getForCurrentView();
            dataTransferManager.removeEventListener("datarequested", dataRequested);
        }
    });

    // NavigationCompleted event is triggered either after all the DOM content has been loaded
    // successfully, or when loading failed.  The event arg for this is different from the other
    // navigation events, and includes a isSuccess field to indicate the status.
    function navigationCompleted(e) {
        if (e.isSuccess) {
            document.getElementById("shareButton").disabled = false;
            document.getElementById("launchInBrowserButton").disabled = false;
            document.getElementById("copyToClipboardButton").disabled = false;
        } else {
            WinJS.log && WinJS.log("Webview navigation failed with error code " + e.webErrorStatus, "sdksample", "error");
        }
    }

    function showShareUI() {
        Windows.ApplicationModel.DataTransfer.DataTransferManager.showShareUI();
    }

    function dataRequested(e) {
        var dataPackage = e.request.data;
        var webviewControl = document.getElementById("webview");

        // Set the data package's properties.  These are displayed within the Share UI
        // to give the user an indication of what is being shared.  They can also be
        // used by target apps to determine the source of the data.
        dataPackage.properties.title = webviewControl.documentTitle;
        dataPackage.properties.description = "Web page shared from WebView";
        dataPackage.properties.applicationName = "WebView JS Sample";

        // Set the data being shared.  Here we share the WebView's current URL and an
        // image of the WebView's current rendered page.
        dataPackage.setText(webviewControl.src);
        dataPackage.setUri(new Windows.Foundation.Uri(webviewControl.src));

        var captureOperation = webviewControl.capturePreviewToBlobAsync();
        captureOperation.oncomplete = function (completeEvent) {
            var bitmapStream = Windows.Storage.Streams.RandomAccessStreamReference.createFromStream(completeEvent.target.result.msDetachStream());
            dataPackage.setBitmap(bitmapStream);
        };
        captureOperation.start();
    }

    function launchInBrowser() {
        var destinationUrl = document.getElementById("webview").src;
        var launchOptions = new Windows.System.LauncherOptions;
        launchOptions.desiredRemainingView = Windows.UI.ViewManagement.ViewSizePreference.useHalf;
        Windows.System.Launcher.launchUriAsync(new Windows.Foundation.Uri(destinationUrl), launchOptions);
    }

    function copyToClipboard() {
        var webviewControl = document.getElementById("webview");

        // Create a data package containing the WebView's current URL to be placed on the clipboard.
        var dataPackage = new Windows.ApplicationModel.DataTransfer.DataPackage();
        dataPackage.setText(webviewControl.src);
        dataPackage.setUri(new Windows.Foundation.Uri(webviewControl.src));

        Windows.ApplicationModel.DataTransfer.Clipboard.setContent(dataPackage);
    }

})();
