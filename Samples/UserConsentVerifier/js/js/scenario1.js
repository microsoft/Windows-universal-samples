//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario1.html", {
        ready: function (element, options) {
            document.getElementById("CheckAvailability").addEventListener("click", checkConsentAvailability, false);
        }
    });

    // Check the availability of Windows Hello authentication through User Consent Verifier.
    function checkConsentAvailability() {
        try {

            Windows.Security.Credentials.UI.UserConsentVerifier.checkAvailabilityAsync()
            .then(function (consentAvailability) {
                switch (consentAvailability) {
                    case Windows.Security.Credentials.UI.UserConsentVerifierAvailability.available:
                        WinJS.log && WinJS.log("User consent verification available!", "sample", "status");
                        break;
                    case Windows.Security.Credentials.UI.UserConsentVerifierAvailability.deviceNotPresent:
                        WinJS.log && WinJS.log("No PIN or biometric found, please set one up.", "sample", "error");
                        break;
                    default:
                        WinJS.log && WinJS.log("User consent verification is currently unavailable.", "sample", "error");
                        break;
                }
            });
        }
        catch (err) {
            WinJS.log && WinJS.log("Error message: " + err.message, "sample", "error");
        }
    }
})();
