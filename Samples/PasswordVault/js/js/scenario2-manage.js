//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved


(function () {
    "use strict";

    var PasswordVault = Windows.Security.Credentials.PasswordVault;

    var inputResource;
    var inputUserName;
    var retrievedCredentials;
    var creds;

    var ElementNotFound = 0x80070490 - (0xFFFFFFFF + 1);

    var page = WinJS.UI.Pages.define("/html/scenario2-manage.html", {
        ready: function (element, options) {
            document.getElementById("RetrieveCredButton").addEventListener("click", retrieveCredentials, false);
            document.getElementById("RevealPasswordsButton").addEventListener("click", revealPasswords, false);
            document.getElementById("RemoveCredButton").addEventListener("click", removeCredentials, false);

            inputResource = document.getElementById("InputResourceValue");
            inputUserName = document.getElementById("InputUserNameValue");
            retrievedCredentials = document.getElementById("retrievedCredentials");
        }
    });

    function retrieveCredentials() {
        // Clear previous output.
        retrievedCredentials.innerHTML = "";
        WinJS.log && WinJS.log("", "sample", "status");
        creds = [];

        // Get input resource and user name.
        var resource = inputResource.value;
        var userName = inputUserName.value;

        var vault = new PasswordVault();
        
        // The credential retrieval functions raise an "Element not found"
        // exception if there were no matches.
        try {
            // Perform the desired query.
            if (resource === "" && userName === "") {
                // Both resource and user name are empty: Use retrieveAll().
                creds = vault.retrieveAll();
            } else if (userName === "") {
                // Resource is provided but no user name: Use findAllByResource().
                creds = vault.findAllByResource(resource);
            } else if (resource === "") {
                // User name is provided but no resource: Use findByUserName().
                creds = vault.findAllByUserName(userName);
            } else {
                // Both resource and user name are provided: Use retrieve().
                var cred = vault.retrieve(resource, userName);
                // Add it to our results if the retrieval was successful.
                if (cred)
                {
                    creds.push(cred);
                }
            }
        } catch (e) {
            if (e.number != ElementNotFound) {
                throw e;
            }
        }

        // Display the results. Note that the password field is initially blank.
        // You must call retrievePassword() to get the password.

        if (creds.length == 0) {
            WinJS.log && WinJS.log("No matching credentials", "sample", "error");
        } else {
            creds.forEach(function (cred, i) {
                var option = document.createElement("option");
                option.text = cred.resource + ": " + cred.userName;
                option.value = i;
                retrievedCredentials.add(option);
            });
            retrievedCredentials.size = retrievedCredentials.options.length;
            WinJS.log && WinJS.log("Credentials found: " + creds.length, "sample", "status");
        }
    }

    function revealPasswords() {
        var selectedOptions = retrievedCredentials.selectedOptions;
        if (selectedOptions.length) {
        var retrievedCount = 0;
            Array.prototype.forEach.call(selectedOptions, function (option) {
                var cred = creds[option.value];
                try {
                    cred.retrievePassword();
                    option.text = cred.resource + ": " + cred.userName + " (" + cred.password + ")";
                    retrievedCount++;
                } catch (e) {
                    if (e.number != ElementNotFound) {
                        throw e;
                    }
                    option.text = cred.resource + ": " + cred.userName + " (password unavailable)";
                }
            });
            WinJS.log && WinJS.log("Passwords retrieved: " + retrievedCount, "sample", "status");
        } else {
            WinJS.log && WinJS.log("No credentials selected", "sample", "error");
        }
    }

    function removeCredentials() {
        var selectedOptions = retrievedCredentials.selectedOptions;
        if (selectedOptions.length) {
            var vault = new PasswordVault();
            var removedCount = 0;

            // Take a snapshot of the selectedOptions collection because we will be
            // changing the collection during the loop.
            var snapshot = Array.prototype.slice.call(selectedOptions, 0);
            snapshot.forEach(function (option) {
                var cred = creds[option.value];
                retrievedCredentials.removeChild(option);

                try {
                    vault.remove(cred);
                    removedCount++;
                } catch (e) {
                    // Ignore the exception if the credential was already removed.
                    if (e.number != ElementNotFound) {
                        throw e;
                    }
                }
            });
            retrievedCredentials.size = retrievedCredentials.options.length;
            WinJS.log && WinJS.log("Credentials removed: " + removedCount, "sample", "status");
        } else {
            WinJS.log && WinJS.log("No credentials selected", "sample", "error");
        }
    }
})();
