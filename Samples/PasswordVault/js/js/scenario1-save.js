//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var PasswordVault = Windows.Security.Credentials.PasswordVault;
    var PasswordCredential = Windows.Security.Credentials.PasswordCredential;

    var inputResource;
    var inputUserName;
    var inputPassword;

    var page = WinJS.UI.Pages.define("/html/scenario1-save.html", {
        ready: function (element, options) {
            document.getElementById("SaveCredButton").addEventListener("click", saveCredential, false);
            inputResource = document.getElementById("InputResourceValue");
            inputUserName = document.getElementById("InputUserNameValue");
            inputPassword = document.getElementById("InputPasswordValue");
        }
    });

    function saveCredential() {
        //get input resource, username, password
        var resource = inputResource.value;
        var username = inputUserName.value;
        var password = inputPassword.value;

        if (resource === "" || username === "" || password === "") {
            WinJS.log && WinJS.log("All fields are required when saving a credential.", "sample", "error");
            return;
        }

        // Add a credential to PasswordVault with provided resource, user name, and password.
        // Replaces any existing credential with the same resource and user name.
        var vault = new PasswordVault();
        var cred = new PasswordCredential(resource, username, password);
        vault.add(cred);
        WinJS.log && WinJS.log("Credential saved successfully. Resource: " + cred.resource + ", UserName: " + cred.userName + ", Password: " + cred.password, "sample", "status");
    }
})();
