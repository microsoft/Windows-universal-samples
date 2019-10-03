//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario6.html", {
        ready: function (element, options) {
            document.getElementById("Delete").addEventListener("click", Delete_Click, false);
        }
    });

    function Delete_Click() {

        if (null === SdkSample.reader || undefined === SdkSample.reader) {
            WinJS.log("Use Scenario One to create a TPM virtual smart card.", "sample", "error");
            return;
        }

        var button = document.getElementById("Delete");
        button.disabled = true;

        WinJS.log("Deleting TPM virtual smart card...", "sample", "status");

        SdkSample.getSmartCard().then(
        function (smartCard) {

            // The following two lines are not directly related to TPM virtual
            // smart card deletion, but are used to demonstrate how to handle
            // CardRemoved events by registering an event handler with a
            // SmartCardReader object.  Since we are using a TPM virtual smart
            // card in this case, the card cannot actually be added to or
            // removed from the reader, but a CardRemoved event will fire when
            // the card and reader are deleted.
            SdkSample.reader.oncardremoved = handleCardRemoved;

            return Windows.Devices.SmartCards.SmartCardProvisioning.requestVirtualSmartCardDeletionAsync(smartCard);
        }).done(
        function (result) {
            if (result) {
                WinJS.log("TPM virtual smart card deletion completed.", "sample", "status");
                SdkSample.reader = null;
            }
            else {
                WinJS.log("TPM virtual smart card deletion was canceled by " + "the user.", "sample", "status");
            }
            button.disabled = false;
        },
        function (error) {
            WinJS.log && WinJS.log("TPM virtual smart card deletion failed with exception: " + error.toString(), "sample", "error");
            button.disabled = false;
        });
    }

    function handleCardRemoved(eventArgs) {
        WinJS.log("Card removed from reader " + eventArgs.target.name + ".", "sample", "status");
    }
})();
