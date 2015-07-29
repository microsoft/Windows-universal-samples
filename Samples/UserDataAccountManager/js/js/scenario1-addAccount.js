//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/scenario1-addAccount.html", {
        ready: function (element, options) {
            document.getElementById("addAccountButton").addEventListener("click", addAccountClick, false);
        }
    });
    
    var addAccountLaunched = false;

    function addAccountClick() {
        var userDataAccounts = Windows.ApplicationModel.UserDataAccounts;

        WinJS.log && WinJS.log("Launching add account", "sample", "status");

        // only one instance of the Accounts pane can be launched at once per thread
        if (addAccountLaunched == false)
        {
            addAccountLaunched = true;

            // Allow user to add email, and appointment providing accounts. For contacts, include that UserDataAccountContentKind as well.
            userDataAccounts.UserDataAccountManager.showAddAccountAsync(userDataAccounts.UserDataAccountContentKinds.email | userDataAccounts.UserDataAccountContentKinds.appointment).then(function (accountString) {
                addAccountLaunched = false;

                if (accountString == "") {
                    WinJS.log && WinJS.log("User cancelled or add account failed", "sample", "status");
                    return;
                }

                userDataAccounts.UserDataAccountManager.requestStoreAsync(userDataAccounts.UserDataAccountStoreAccessType.AllAccountsReadOnly).then(function (store) {
                    if (store == null) {
                        WinJS.log && WinJS.log("Access to Contacts, Calendar and Email has been revoked", "sample", "error");
                        return;
                    }

                    store.getAccountAsync(accountString).then(function (userAccount) {
                        WinJS.log && WinJS.log("Added account: " + userAccount.userDisplayName, "sample", "status");
                    });

                });
            });
        }
    }
})();
