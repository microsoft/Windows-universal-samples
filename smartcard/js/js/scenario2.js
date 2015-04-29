//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario2.html", {
        ready: function (element, options) {
            document.getElementById("ChangePin")
                        .addEventListener("click", ChangePin_Click, false);
        }
    });

    function ChangePin_Click() {

        if (null === SdkSample.reader || undefined === SdkSample.reader) {
            WinJS.log("Use Scenario One to create a TPM virtual smart card.", "sample", "error");
            return;
        }

        var button = document.getElementById("ChangePin");
        button.disabled = true;

        WinJS.log("Changing smart card PIN...", "sample", "status");

        SdkSample.getSmartCard().then(
        function (smartCard) {
            return Windows.Devices.SmartCards.SmartCardProvisioning.fromSmartCardAsync(smartCard);
        }).then(
        function (smartCardProvisioning) {
            return smartCardProvisioning.requestPinChangeAsync();
        }).done(
        function (result) {
            if (result) {
                WinJS.log("Smart card change PIN operation completed.", "sample", "status");
            }
            else {
                WinJS.log("Smart card change PIN operation was canceled by " + "the user.", "sample", "status");
            }
            button.disabled = false;
        },
        function (error) {
            WinJS.log && WinJS.log("Changing smart card PIN failed with exception: " + error.toString(), "sample", "error");
            button.disabled = false;
        });
    }
})();
