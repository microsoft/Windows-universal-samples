//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario2.html", {
        ready: function (element, options) {
            document.getElementById("RequestConsent").addEventListener("click", requestConsent, false);
        }
    });

    // Requests fingerprint consent from the current user.
    function requestConsent() {
        try {
            // Read the message that has to be displayed in the consent request prompt
            var message = document.getElementById("Message").value;

            // Request the currently logged on user's consent via fingerprint swipe
            Windows.Security.Credentials.UI.UserConsentVerifier.requestVerificationAsync(message)
            .then(function (consentResult) {
                switch (consentResult) {
                    case Windows.Security.Credentials.UI.UserConsentVerificationResult.verified:
                        WinJS.log && WinJS.log("User's presence verified.", "sample", "status");
                        break;
                    case Windows.Security.Credentials.UI.UserConsentVerificationResult.deviceNotPresent:
                        WinJS.log && WinJS.log("No biometric device found.", "sample", "error");
                        break;
                    case Windows.Security.Credentials.UI.UserConsentVerificationResult.disabledByPolicy:
                        WinJS.log && WinJS.log("Biometrics is disabled by policy.", "sample", "error");
                        break;
                    case Windows.Security.Credentials.UI.UserConsentVerificationResult.retriesExhausted:
                        WinJS.log && WinJS.log("Too many failed attempts.", "sample", "error");
                        break;
                    case Windows.Security.Credentials.UI.UserConsentVerificationResult.notConfiguredForUser:
                        WinJS.log && WinJS.log("User has no fingeprints registered.", "sample", "error");
                        break;
                    case Windows.Security.Credentials.UI.UserConsentVerificationResult.deviceBusy:
                        WinJS.log && WinJS.log("Biometric device is busy.", "sample", "error");
                        break;
                    case Windows.Security.Credentials.UI.UserConsentVerificationResult.canceled:
                        WinJS.log && WinJS.log("Consent request prompt was canceled.", "sample", "error");
                        break;
                    default:
                        WinJS.log && WinJS.log("Consent verification is currently unavailable.", "sample", "error");
                        break;
                }
            });
        }
        catch (err) {
            WinJS.log && WinJS.log("Error message: " + err.message, "sample", "error");
        }
    }
})();
