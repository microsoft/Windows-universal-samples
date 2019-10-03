//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario3.html", {
        ready: function (element, options) {
            document.getElementById("ResetPin").addEventListener("click", ResetPin_Click, false);
        }
    });

    function ResetPin_Click() {

        if (null === SdkSample.reader || undefined === SdkSample.reader) {
            WinJS.log("Use Scenario One to create a TPM virtual smart card.", "sample", "error");
            return;
        }

        var button = document.getElementById("ResetPin");
        button.disabled = true;

        WinJS.log("Resetting smart card PIN...", "sample", "status");

        SdkSample.getSmartCard().then(
        function (smartCard) {
            return Windows.Devices.SmartCards.SmartCardProvisioning.fromSmartCardAsync(smartCard);
        }).then(
        function (smartCardProvisioning) {
            // The argument to requestPinResetAsync must be a function which
            // uses the current admin key to calculate and set the response.
            return smartCardProvisioning.requestPinResetAsync(
            function (sender, request) {
                var deferral = request.getDeferral();
                var response = SdkSample.calculateChallengeResponse(request.challenge, SdkSample.adminKey);

                request.setResponse(response);

                deferral.complete();
            });

        }).done(
        function (result) {
            if (result) {
                WinJS.log("Smart card PIN reset operation completed.", "sample", "status");
            }
            else {
                WinJS.log("Smart card PIN reset operation was canceled by " + "the user.", "sample", "status");
            }
            button.disabled = false;
        },
        function (error) {
            WinJS.log && WinJS.log("Resetting smart card PIN failed with exception: " + error.toString(), "sample", "error");
            button.disabled = false;
        });

    }
})();
