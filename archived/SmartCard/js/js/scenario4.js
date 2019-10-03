//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario4.html", {
        ready: function (element, options) {
            document.getElementById("ChangeAdminKey").addEventListener("click", ChangeAdminKey_Click, false);
        }
    });

    function ChangeAdminKey_Click() {
        if (null === SdkSample.reader || undefined === SdkSample.reader) {
            WinJS.log("Use Scenario One to create a TPM virtual smart card.", "sample", "error");
            return;
        }

        var newAdminKey = Windows.Security.Cryptography.CryptographicBuffer.generateRandom(SdkSample.ADMIN_KEY_LENGTH_IN_BYTES);

        var button = document.getElementById("ChangeAdminKey");
        button.disabled = true;

        WinJS.log("Changing smart card admin key...", "sample", "status");

        SdkSample.getSmartCard().then(
        function (smartCard) {
            return Windows.Devices.SmartCards.SmartCardProvisioning.fromSmartCardAsync(smartCard);
        }).then(
        function (smartCardProvisioning) {
            return smartCardProvisioning.getChallengeContextAsync();
        }).then(
        function (challengeContext) {
            var response = SdkSample.calculateChallengeResponse(challengeContext.challenge, SdkSample.adminKey);
            return challengeContext.changeAdministrativeKeyAsync(response, newAdminKey);
        }).done(
        function () {
            SdkSample.adminKey = newAdminKey;
            WinJS.log("Smart card change admin key operation completed.", "sample", "status");
            button.disabled = false;
        },
        function (error) {
            WinJS.log && WinJS.log("Changing smart card admin key failed with exception: " + error.toString(), "sample", "error");
            button.disabled = false;
        });
    }
})();
