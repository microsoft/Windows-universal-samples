//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var DataTransferManager = Windows.ApplicationModel.DataTransfer.DataTransferManager;
    var RandomAccessStreamReference = Windows.Storage.Streams.RandomAccessStreamReference;
    var ShareProvider = Windows.ApplicationModel.DataTransfer.ShareProvider;
    var Uri = Windows.Foundation.Uri;

    var dataTransferManager = DataTransferManager.getForCurrentView();
    var progressRing;
    var shareCompletedText;

    var page = WinJS.UI.Pages.define("/html/provider.html", {
        ready: function (element, options) {
            progressRing = document.getElementById("progressRing");
            shareCompletedText = document.getElementById("shareCompletedText");

            dataTransferManager.addEventListener("datarequested", dataRequested);

            // Register the event to add a Share Provider
            dataTransferManager.addEventListener("shareprovidersrequested", shareProvidersRequested);

            document.getElementById("shareButton").addEventListener("click", showShareUI, false);
        },
        unload: function () {
            dataTransferManager.removeEventListener("datarequested", dataRequested);
            dataTransferManager.removeEventListener("shareprovidersrequested", shareProvidersRequested);
        }
    });

    function dataRequested(e) {
        SdkSample.registerForShareCompletion(e);

        var data = e.request.data;
        data.properties.title = "I got a new high score!";
        data.setText("I got a high score of 15063!");
    }

    function shareProvidersRequested(e) {
        // Take a deferral so that we can perform async operations. (This sample
        // doesn't perform any async operations in the ShareProvidersRequested
        // event handler, but we take the deferral anyway to demonstrate the pattern.)
        var deferral = e.getDeferral();

        // Create the custom share provider and add it.
        var icon = RandomAccessStreamReference.createFromUri(new Uri("ms-appx:///images/windows-sdk.png"));
        var provider = new ShareProvider("Contoso Chat", icon, Windows.UI.Colors.lightGray, shareToContosoChat);
        e.providers.push(provider);

        deferral.complete();
    }

    function shareToContosoChat(operation) {
        shareCompletedText.innerText = "";

        // Obtain the text from the data package. This should match
        // the information placed in it by the GetShareContent method.
        var text;

        operation.data.getTextAsync().then(function (dataPackageText) {
            text = dataPackageText;

            // The app can display custom UI to complete the Share operation.
            // Here, we simply display a progress control and delay for a while,
            // to simulate posting to the Contoso Chat service.
            progressRing.style.visibility = "visible";
            return WinJS.Promise.timeout(2000);
        }).done(function () {
            // All done. Show the success message.
            progressRing.style.visibility = "hidden";
            shareCompletedText.innerText = `Your message '${text}' has been shared with Contoso Chat.`;

            // Completing the operation causes ShareCompleted to be raised.
            operation.reportCompleted();
        });
    }

    function showShareUI() {
        DataTransferManager.showShareUI();
    }
})();
