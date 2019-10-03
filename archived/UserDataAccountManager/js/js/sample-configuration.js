//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Sample Title here";

    var scenarios = [
        { url: "/html/scenario1-addAccount.html", title: "Add an account" },
        { url: "/html/scenario2-manageAccount.html", title: "Manage an account" },
        { url: "/html/scenario3-fixAccount.html", title: "Fix an account" }
    ];

    var userDataAccounts = Windows.ApplicationModel.UserDataAccounts;

    function loadDataAccountsAsync(accountSelect)
    {
        var length = accountSelect.length;
        for (var i = 0; i < length; i++) {
            accountSelect.removeChild(accountSelect[0]);
        }

        return userDataAccounts.UserDataAccountManager.requestStoreAsync(userDataAccounts.UserDataAccountStoreAccessType.AllAccountsReadOnly).then(function (store) {
            if (store == null) {
                WinJS.log && WinJS.log("Access to Contacts, Calendar and Email has been revoked", "sample", "error");
                return;
            }

            return store.findAccountsAsync().then(function (accounts) {
                accounts.forEach(function (account) {
                    addOption(accountSelect, account);
                });
            });
        });
    }

    function addOption(accountSelect, dataAccount)
    {
        var newOption = document.createElement("option");
        newOption.text = dataAccount.userDisplayName;
        newOption.value = dataAccount.id;
        newOption.selected = false;
        accountSelect.add(newOption);
    }

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        loadDataAccountsAsync: loadDataAccountsAsync
    });
})();