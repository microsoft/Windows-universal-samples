//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved


(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario1.html", {
        ready: function (element, options) {
            document.getElementById("ResetButton").addEventListener("click", resetScenario1, false);
            document.getElementById("SaveCredButton").addEventListener("click", AddCredential, false);
            document.getElementById("ReadCredButton").addEventListener("click", ReadCredential, false);
            document.getElementById("DeleteCredButton").addEventListener("click", DeleteCredential, false);

            // Initialize the password vault, this may take less than a second
            // An optimistic initialization at this stage improves the UI performance
            // when any other call to the password vault may be made later on
            asyncVaultLoad();
        }
    });

    var authzInProgress = false;

    function asyncVaultLoad() {
        return new WinJS.Promise(function (complete, error, progress) {
            var vault = new Windows.Security.Credentials.PasswordVault();

            // any call to the password vault will load the vault
            var creds = vault.retrieveAll();
            complete();
        });
    }

    function AddCredential() {
        try {
            //get input resource, username, password
            var resource = document.getElementById("InputResourceValue").value;
            var username = document.getElementById("InputUserNameValue").value;
            var password = document.getElementById("InputPasswordValue").value;

            if (resource === "" || username === "" || password === "") {
                WinJS.log("Resouce, Username and Password are required when adding a credential", "Password Vault SDK Sample", "error");
                return;
            }

            //Adding a credenital to PasswordVault with provided Resouce, Username and Password.
            var vault = new Windows.Security.Credentials.PasswordVault();
            var cred = new Windows.Security.Credentials.PasswordCredential(resource, username, password);
            vault.add(cred);
            WinJS.log("Credential saved successfully. Resource: " + cred.resource + " Username: " + cred.userName + " Password: " + cred.password.toString(), "Password Vault SDK Sample", "status");
        }

        catch (e) {
            WinJS.log(e.message, "Password Vault SDK Sample", "error");
            return;
        }
    }

    function ReadCredential() {
        try {
            //get input resource, username, password
            var resource = document.getElementById("InputResourceValue").value;
            var username = document.getElementById("InputUserNameValue").value;
            var vault = new Windows.Security.Credentials.PasswordVault();
            var cred = null;
            var creds = null;

            //If both resource and username are empty, you can use retrieveAll() to enumerate all credentials
            if (resource === "" && username === "") {
                WinJS.log("Retrieving all credentials since resource or username are not specified", "Password Vault SDK Sample", "status");
                creds = vault.retrieveAll();
            }
            //If there is only resouce, you can use findAllByResource() to enumerate by resource.
            else if (username === "") {
                WinJS.log("Retrieving credentials in PasswordVault by resource: " + resource, "Password Vault SDK Sample", "status");
                creds = vault.findAllByResource(resource);
            }
            //If there is only username, you can use findByUserName() to enumerate by resource.
            else if (resource === "") {
                WinJS.log("Retrieving credentials in PasswordVault by username: " + username, "Password Vault SDK Sample", "status");
                creds = vault.findAllByUserName(username);
            }
            //If both resource and username are provided, you can use retrieve() to search the credential
            else {
                WinJS.log("Retrieving in PasswordVault by resoucre and username: " + resource + "/" + username, "Password Vault SDK Sample", "status");
                cred = vault.retrieve(resource, username);
            }
            //printout credenitals
            if (cred !== null) {
                WinJS.log("Read credential (resource: " + cred.resource + ", user: " + cred.userName + " ) succeeds", "Password Vault SDK Sample", "status");
            }
            else if (creds.size > 0) {
                WinJS.log("There are(is) " + creds.size.toString() + " credential(s) found in PasswordVault", "Password Vault SDK Sample", "status");
                for (var i = 0; i < creds.size; i++) {
                    //retrive credenital with resource and username
                    var singleCred = vault.retrieve(creds.getAt(i).resource, creds.getAt(i).userName);
                    WinJS.log("Read credential succeeds. Resource: " + singleCred.resource.toString() + " Username: " + singleCred.userName.toString() + " Password: " + singleCred.password.toString() + ".", "Password Vault SDK Sample", "status");
                }
            }
            else {
                WinJS.log("Credential not found.", "Password Vault SDK Sample", "error");
            }
        }
        catch (e) {
            WinJS.log(e.message, "Password Vault SDK Sample", "error");
            return;
        }
    }

    function DeleteCredential() {
        try {
            //get input resource, username
            var resource = document.getElementById("InputResourceValue").value;
            var username = document.getElementById("InputUserNameValue").value;
            //Delete by resouce name and username
            if (resource === "" || username === "") {
                WinJS.log("To delete a credential, you need to enter both resoucename and username", "Password Vault SDK Sample", "error");
                return;
            }
            else {
                var vault = new Windows.Security.Credentials.PasswordVault();
                var cred = vault.retrieve(resource, username);
                vault.remove(cred);
                WinJS.log("Credential removed successfully. Resource: " + cred.resource + " Username: " + cred.userName, "Password Vault SDK Sample", "status");
            }
        } catch (e) {
            new DebugPrint(e.message);
            return;
        }
    }

    function resetScenario1() {
        try {
            document.getElementById("InputResourceValue").value = "";
            document.getElementById("InputUserNameValue").value = "";
            document.getElementById("InputPasswordValue").value = "";
            var vault = new Windows.Security.Credentials.PasswordVault();
            var creds = vault.retrieveAll();
            for (var i = 0; i < creds.size; i++) {
                //retrive credenital with resource and username
                vault.remove(creds.getAt(i));
            }
            WinJS.log("The scenario has been reset", "Password Vault SDK Sample", "status");
        } catch (err) {
            return;
        }

    }
})();
