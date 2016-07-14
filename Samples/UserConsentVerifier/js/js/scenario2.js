//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario2.html", {
        ready: function (element, options) {
            document.getElementById("RequestConsent").addEventListener("click", requestConsent, false);
        }
    });

    // Request the logged on user's consent using Windows Hello via biometric verification or a PIN.
    function requestConsent() {
        try {
            // Read the message that has to be displayed in the consent request prompt
            var message = document.getElementById("Message").value;

            Windows.Security.Credentials.UI.UserConsentVerifier.requestVerificationAsync(message)
            .then(function (consentResult) {
                switch (consentResult) {
                    case Windows.Security.Credentials.UI.UserConsentVerificationResult.verified:
                        WinJS.log && WinJS.log("User consent verified!", "sample", "status");
                        break;
                    case Windows.Security.Credentials.UI.UserConsentVerificationResult.deviceNotPresent:
                        WinJS.log && WinJS.log("No PIN or biometric found, please set one up.", "sample", "error");
                        break;
                    case Windows.Security.Credentials.UI.UserConsentVerificationResult.canceled:
                        WinJS.log && WinJS.log("User consent verification canceled.", "sample", "error");
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
