//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario2.html", {
        ready: function (element, options) {
            document.getElementById("CheckComplianceLaunch").addEventListener("click", checkCompliance, false);
            document.getElementById("CheckComplianceReset").addEventListener("click", checkComplianceReset, false);
        }
    });



    function checkCompliance() {

        // Get all the parameters from the user
        var requireEncryptionValue = document.getElementById("RequireEncryptionValue").checked;
        if (!requireEncryptionValue) {
            requireEncryptionValue = false;
        }

        var minPasswordLengthValue = document.getElementById("MinPasswordLengthValue").value;
        if (minPasswordLengthValue === null || minPasswordLengthValue === "") {
            minPasswordLengthValue = 0;
        }

        var disallowConvenienceLogonValue = document.getElementById("DisallowConvenienceLogonValue").checked;
        if (!disallowConvenienceLogonValue) {
            disallowConvenienceLogonValue = false;
        }

        var minPasswordComplexCharactersValue = document.getElementById("MinPasswordComplexCharactersValue").value;
        if (minPasswordComplexCharactersValue === null || minPasswordComplexCharactersValue === "") {
            minPasswordComplexCharactersValue = 0;
        }

        var passwordExpirationValue = document.getElementById("PasswordExpirationValue").value;
        if (passwordExpirationValue === null || passwordExpirationValue === "") {
            passwordExpirationValue = 0;
        }

        var passwordHistoryValue = document.getElementById("PasswordHistoryValue").value;
        if (passwordHistoryValue === null || passwordHistoryValue === "") {
            passwordHistoryValue = 0;
        }

        var maxPasswordFailedAttemptsValue = document.getElementById("MaxPasswordFailedAttemptsValue").value;
        if (maxPasswordFailedAttemptsValue === null || maxPasswordFailedAttemptsValue === "") {
            maxPasswordFailedAttemptsValue = 0;
        }


        var maxInactivityTimeLockValue = document.getElementById("MaxInactivityTimeLockValue").value;
        if (maxInactivityTimeLockValue === null || maxInactivityTimeLockValue === "") {
            maxInactivityTimeLockValue = 0;
        }

        try {

            var requestedPolicy = new Windows.Security.ExchangeActiveSyncProvisioning.EasClientSecurityPolicy();
            requestedPolicy.requireEncryption = requireEncryptionValue;
            requestedPolicy.minPasswordLength = parseInt(minPasswordLengthValue);
            requestedPolicy.disallowConvenienceLogon = disallowConvenienceLogonValue;
            requestedPolicy.minPasswordComplexCharacters = parseInt(minPasswordComplexCharactersValue);
            requestedPolicy.passwordExpiration = parseInt(passwordExpirationValue) * 86400000;
            requestedPolicy.maxPasswordFailedAttempts = parseInt(maxPasswordFailedAttemptsValue);
            requestedPolicy.maxInactivityTimeLock = parseInt(maxInactivityTimeLockValue) * 1000;

            var complianceResult = requestedPolicy.checkCompliance();

            document.getElementById("RequireEncryptionComplianceResult").value = complianceResult.requireEncryptionResult;
            document.getElementById("EncryptionProviderTypeComplianceResult").value = complianceResult.encryptionProviderType;
            document.getElementById("MinPasswordLengthComplianceResult").value = complianceResult.minPasswordLengthResult;
            document.getElementById("DisallowConvenienceLogonComplianceResult").value = complianceResult.disallowConvenienceLogonResult;
            document.getElementById("MinPasswordComplexCharactersComplianceResult").value = complianceResult.minPasswordComplexCharactersResult;
            document.getElementById("PasswordExpirationComplianceResult").value = complianceResult.passwordExpirationResult;
            document.getElementById("PasswordHistoryComplianceResult").value = complianceResult.passwordHistoryResult;
            document.getElementById("MaxPasswordFailedAttemptsComplianceResult").value = complianceResult.maxPasswordFailedAttemptsResult;
            document.getElementById("MaxInactivityTimeLockComplianceResult").value = complianceResult.maxInactivityTimeLockResult;

        } catch (err) {
            WinJS.log("Error CheckCompliance: " + err.message, "EAS SDK Sample", "error");
            return;
        }

    }

    function checkComplianceReset() {

        // Get all the result reset

        try {

            document.getElementById("RequireEncryptionValue").checked = false;
            document.getElementById("MinPasswordLengthValue").value = "";
            document.getElementById("DisallowConvenienceLogonValue").checked = false;
            document.getElementById("MinPasswordComplexCharactersValue").value = "";
            document.getElementById("PasswordExpirationValue").value = "";
            document.getElementById("PasswordHistoryValue").value = "";
            document.getElementById("MaxPasswordFailedAttemptsValue").value = "";
            document.getElementById("MaxInactivityTimeLockValue").value = "";

            document.getElementById("RequireEncryptionComplianceResult").value = "";
            document.getElementById("EncryptionProviderTypeComplianceResult").value = "";
            document.getElementById("MinPasswordLengthComplianceResult").value = "";
            document.getElementById("DisallowConvenienceLogonComplianceResult").value = "";
            document.getElementById("MinPasswordComplexCharactersComplianceResult").value = "";
            document.getElementById("PasswordExpirationComplianceResult").value = "";
            document.getElementById("PasswordHistoryComplianceResult").value = "";
            document.getElementById("MaxPasswordFailedAttemptsComplianceResult").value = "";
            document.getElementById("MaxInactivityTimeLockComplianceResult").value = "";

        } catch (err) {
            WinJS.log("Error reset CheckCompliance: " + err.message, "EAS SDK Sample", "error");
            return;
        }

    }

})();