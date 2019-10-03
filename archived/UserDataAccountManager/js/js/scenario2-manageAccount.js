//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var userDataAccounts = Windows.ApplicationModel.UserDataAccounts;
    var accountSelect;
    var isLaunched = false;

    var page = WinJS.UI.Pages.define("/html/scenario2-manageAccount.html", {
        ready: function (element, options) {
            document.getElementById("manageAccountButton").addEventListener("click", manageAccount, false);
            accountSelect = document.getElementById("accountSelect");
            return SdkSample.loadDataAccountsAsync(accountSelect);
        }
    });

    function manageAccount() {
        if (accountSelect.selectedIndex >= 0)
        {
            var accountId = accountSelect[accountSelect.selectedIndex].value;

            // only one instance of the UserDataAccountsManager pane can be launched at once per thread
            if (isLaunched == false)
            {
                isLaunched = true;

                return userDataAccounts.UserDataAccountManager.showAccountSettingsAsync(accountId).then(function () {
                    return SdkSample.loadDataAccountsAsync(accountSelect);
                }).done(function () {
                    isLaunched = false;
                });
            }
        }
    }
})();
