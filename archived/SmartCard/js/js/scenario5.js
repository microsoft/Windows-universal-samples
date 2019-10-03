//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario5.html", {
        ready: function (element, options) {
            document.getElementById("VerifyResponse").addEventListener("click", VerifyResponse_Click, false);
        }
    });

    function VerifyResponse_Click() {

        if (null === SdkSample.reader || undefined === SdkSample.reader) {
            WinJS.log("Use Scenario One to create a TPM virtual smart card.", "sample", "error");
            return;
        }

        var button = document.getElementById("VerifyResponse");
        button.disabled = true;

        WinJS.log("Verifying smart card response...", "sample", "status");

        SdkSample.getSmartCard().then(
        function (smartCard) {
            return Windows.Devices.SmartCards.SmartCardProvisioning.fromSmartCardAsync(smartCard);
        }).then(
        function (smartCardProvisioning) {
            return smartCardProvisioning.getChallengeContextAsync();
        }).then(
        function (challengeContext) {
            var response = SdkSample.calculateChallengeResponse(challengeContext.challenge, SdkSample.adminKey);
            return challengeContext.verifyResponseAsync(response);
        }).done(
        function (result) {
            WinJS.log("Smart card response verification completed. Result: " + result, "sample", "status");
            button.disabled = false;
        },
        function (error) {
            WinJS.log && WinJS.log("Smart card response verification failed with exception: " + error.toString(), "sample", "error");
            button.disabled = false;
        });
    }
})();
