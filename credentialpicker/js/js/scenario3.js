//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario3.html", {
        ready: function (element, options) {
            document.getElementById("LaunchButton").addEventListener("click", launchCredPicker, false);
            document.getElementById("InputProtocol").addEventListener("change", hideShowCustomProtocol, false);
        }
    });

    function hideShowCustomProtocol() {
        // ignore if fired when elements are missing or uninitialized
        if (document.getElementById("InputProtocol") === null || document.getElementById("CustomProtocolContainer") === null) {
            return;
        }
        // toggle visibility
        if (document.getElementById("InputProtocol").value === "Custom") {
            document.getElementById("CustomProtocolContainer").style.display = "table-row";
        } else {
            document.getElementById("CustomProtocolContainer").style.display = "none";
        }
    }

    function launchCredPicker() {
        try {
            var options = new Windows.Security.Credentials.UI.CredentialPickerOptions();
            options.message = document.getElementById("InputMessage").value;
            options.caption = document.getElementById("InputCaption").value;
            options.targetName = document.getElementById("InputTarget").value;
            options.alwaysDisplayDialog = document.getElementById("InputAlwaysShowUI").checked;
            options.callerSavesCredential = document.getElementById("InputCallerSaves").checked;
            switch (document.getElementById("InputProtocol").value) {
                case "Negotiate":
                    options.authenticationProtocol = Windows.Security.Credentials.UI.AuthenticationProtocol.negotiate;
                    break;
                case "Kerberos":
                    options.authenticationProtocol = Windows.Security.Credentials.UI.AuthenticationProtocol.kerberos;
                    break;
                case "CredSsp":
                    options.authenticationProtocol = Windows.Security.Credentials.UI.AuthenticationProtocol.credSsp;
                    break;
                case "Basic":
                    options.authenticationProtocol = Windows.Security.Credentials.UI.AuthenticationProtocol.basic;
                    break;
                case "Digest":
                    options.authenticationProtocol = Windows.Security.Credentials.UI.AuthenticationProtocol.digest;
                    break;
                case "NTLM":
                    options.authenticationProtocol = Windows.Security.Credentials.UI.AuthenticationProtocol.ntlm;
                    break;
                case "Custom":
                    options.authenticationProtocol = Windows.Security.Credentials.UI.AuthenticationProtocol.custom;
                    options.customAuthenticationProtocol = document.getElementById("InputCustomProtocol").value;
                    break;
                default:
                    WinJS.log && WinJS.log("Bad auth protocol specified: " + document.getElementById("InputProtocol").value, "sample", "error");
                    break;
            }
            switch (document.getElementById("InputCheckboxState").value) {
                case "Unselected":
                    options.credentialSaveOption = Windows.Security.Credentials.UI.CredentialSaveOption.unselected;
                    break;
                case "Selected":
                    options.credentialSaveOption = Windows.Security.Credentials.UI.CredentialSaveOption.selected;
                    break;
                case "Hidden":
                    options.credentialSaveOption = Windows.Security.Credentials.UI.CredentialSaveOption.hidden;
                    break;
                default:
                    WinJS.log && WinJS.log("Bad save option specified: " + document.getElementById("InputCheckboxState").value, "sample", "error");
                    break;
            }
            Windows.Security.Credentials.UI.CredentialPicker.pickAsync(options).then(function (results) {
                document.getElementById("OutputDomainName").value = results.credentialDomainName;
                document.getElementById("OutputUserName").value = results.credentialUserName;
                document.getElementById("OutputPassword").value = results.credentialPassword;
                document.getElementById("OutputCredentialSaved").value = results.credentialSaved ? "Yes" : "No";
                document.getElementById("OutputCredentialSaveState").value = (results.credentialSaveOption === Windows.Security.Credentials.UI.CredentialSaveOption.hidden) ? "Hidden" :
                                                                             ((results.credentialSaveOption === Windows.Security.Credentials.UI.CredentialSaveOption.selected) ? "Selected" : "Unselected");
                WinJS.log && WinJS.log("pickAsync status: " + results.errorCode, "sample", "status");
            });
        } catch (err) {
            WinJS.log && WinJS.log("Error message: " + err.message, "sample", "error");
        }
    }
})();
