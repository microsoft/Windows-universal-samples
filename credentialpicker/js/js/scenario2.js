//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario2.html", {
        ready: function (element, options) {
            document.getElementById("LaunchButton").addEventListener("click", launchCredPicker, false);
        }
    });

    function launchCredPicker() {
        try {
            var message = document.getElementById("InputMessage").value;
            var caption = document.getElementById("InputCaption").value;
            var targetName = document.getElementById("InputTarget").value;
            Windows.Security.Credentials.UI.CredentialPicker.pickAsync(targetName, message, caption).then(function (results) {
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
