//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario1.html", {
        ready: function (element, options) {
            document.getElementById("CheckAvailability").addEventListener("click", checkConsentAvailability, false);
        }
    });

    // Checks the availability of User consent requisition via registered fingerprints.
    function checkConsentAvailability() {
        try {

            // Check the availability of User Consent with fingerprints facility
            Windows.Security.Credentials.UI.UserConsentVerifier.checkAvailabilityAsync()
            .then(function (consentAvailability) {
                switch (consentAvailability) {
                    case Windows.Security.Credentials.UI.UserConsentVerifierAvailability.available:
                        WinJS.log && WinJS.log("User consent requisition facility is available.", "sample", "status");
                        break;
                    case Windows.Security.Credentials.UI.UserConsentVerifierAvailability.deviceNotPresent:
                        WinJS.log && WinJS.log("No biometric device found.", "sample", "error");
                        break;
                    case Windows.Security.Credentials.UI.UserConsentVerifierAvailability.disabledByPolicy:
                        WinJS.log && WinJS.log("Biometrics is disabled by policy.", "sample", "error");
                        break;
                    case Windows.Security.Credentials.UI.UserConsentVerifierAvailability.notConfiguredForUser:
                        WinJS.log && WinJS.log("User has no fingeprints registered.", "sample", "error");
                        break;
                    case Windows.Security.Credentials.UI.UserConsentVerifierAvailability.deviceBusy:
                        WinJS.log && WinJS.log("Biometric device is busy.", "sample", "error");
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
