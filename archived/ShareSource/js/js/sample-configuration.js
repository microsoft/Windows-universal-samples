//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Share source JS sample";

    var scenarios = [
        { url: "/html/text.html", title: "Share text", applink: "ms-sdk-sharesourcejs:navigate?page=ShareText" },
        { url: "/html/weblink.html", title: "Share web link", applink: "ms-sdk-sharesourcejs:navigate?page=ShareWebLink" },
        { url: "/html/applicationlink.html", title: "Share application link", applink: "ms-sdk-sharesourcejs:navigate?page=ShareApplicationLink" },
        { url: "/html/image.html", title: "Share an image", applink: "ms-sdk-sharesourcejs:navigate?page=ShareImage" },
        { url: "/html/files.html", title: "Share files", applink: "ms-sdk-sharesourcejs:navigate?page=ShareFiles" },
        { url: "/html/delay-render.html", title: "Share delay rendered files", applink: "ms-sdk-sharesourcejs:navigate?page=ShareDeplayRenderedFiles" },
        { url: "/html/html.html", title: "Share HTML content", applink: "ms-sdk-sharesourcejs:navigate?page=ShareHTML" },
        { url: "/html/custom.html", title: "Share custom data", applink: "ms-sdk-sharesourcejs:navigate?page=ShareCustomData" },
        { url: "/html/provider.html", title: "Share with custom provider", applink: "ms-sdk-sharesourcejs:navigate?page=ShareCustomProvider" },
        { url: "/html/fail.html", title: "Fail with display text", applink: "ms-sdk-sharesourcejs:navigate?page=ShareErrorMessage" }
    ];

    // Install some handlers for all share operations, for demonstration purposes.
    var dataTransferManager = Windows.ApplicationModel.DataTransfer.DataTransferManager.getForCurrentView();

    // Request to be notified when the user chooses a share target app.
    dataTransferManager.addEventListener("targetapplicationchosen", function (e) {
        WinJS.log && WinJS.log(`User chose ${e.applicationName}`, "samples", "status");
    });

    // Common handler which listens for share completion.
    function registerForShareCompletion(e) {
        // Request to be notified when the share operation completes.
        e.request.data.addEventListener("sharecompleted", function (e) {
            var shareCompletedStatus = "Shared successfully. ";

            // Typically, this information is not displayed to the user because the
            // user already knows which share target was selected.
            if (e.shareTarget.appUserModelId) {
                // The user picked an app.
                shareCompletedStatus += `Target: App \"${e.shareTarget.appUserModelId}\"`;
            } else if (e.shareTarget.shareProvider) {
                // The user picked a ShareProvider.
                shareCompletedStatus += `Target: Share Provider \"${e.shareTarget.shareProvider.title}\"`;
            }
            WinJS.log && WinJS.log(shareCompletedStatus, "samples", "status");
        });
    }

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        registerForShareCompletion: registerForShareCompletion,
    });
})();
