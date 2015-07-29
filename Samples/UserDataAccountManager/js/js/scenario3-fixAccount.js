//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var userDataAccounts = Windows.ApplicationModel.UserDataAccounts;
    var accountSelect;
    var isLaunched = false;

    var page = WinJS.UI.Pages.define("/html/scenario3-fixAccount.html", {
        ready: function (element, options) {
            document.getElementById("fixAccountButton").addEventListener("click", fixSelectedAccount, false);
            accountSelect = document.getElementById("accountSelect");
            return SdkSample.loadDataAccountsAsync(accountSelect);
        }
    });

    function fixSelectedAccount() {
        if (accountSelect.selectedIndex >= 0) {
            var accountId = accountSelect[accountSelect.selectedIndex].value;

            // only one instance of the UserDataAccountsManager pane can be launched at once per thread
            if (isLaunched == false) {
                isLaunched = true;
                WinJS.log && WinJS.log("", "sample", "error");

                isFixNeededAsync(accountId).then(function (result) {
                    if (result) {
                        return userDataAccounts.UserDataAccountManager.showAccountErrorResolverAsync(accountId);
                    } else {
                        WinJS.log && WinJS.log("Account is not in an error state", "sample", "error");
                    }
                }).then(function () {
                    return SdkSample.loadDataAccountsAsync(accountSelect);
                }).done(function () {
                    isLaunched = false;
                });
            }
        }
    }
    
    // Accessing the mailboxes, calendars, and contactLists requires the email, contacts, or appointments in the app's manifest
    function isFixNeededAsync(accountId) {
        return userDataAccounts.UserDataAccountManager.requestStoreAsync(userDataAccounts.UserDataAccountStoreAccessType.AllAccountsReadOnly).then(function (store) {
            if (store == null) {
                WinJS.log && WinJS.log("Access to Contacts, Calendar and Email has been revoked", "sample", "error");
                return;
            }
            return store.getAccountAsync(accountId);
        }).then(function (account) {
            if (account) {
                return account.findEmailMailboxesAsync().then(function (mailboxes) {
                    return hasSyncError(mailboxes, Windows.ApplicationModel.Email.EmailMailboxSyncStatus);
                }).then(function (result) {
                    return result ||
                        account.findAppointmentCalendarsAsync().then(function (calendars) {
                            return hasSyncError(calendars, Windows.ApplicationModel.Appointments.AppointmentCalendarSyncStatus);
                        });
                }).then(function (result) {
                    return result ||
                        account.findContactListsAsync().then(function (contactLists) {
                            return hasSyncError(contactLists, Windows.ApplicationModel.Contacts.ContactListSyncStatus);
                        });
                });
            }
        });
    }

    function hasSyncError(statusList, status) {
        return statusList.some(function (item) {
            if (item.syncManager) {
                switch (item.syncManager.status) {
                case status.authenticationError:
                case status.policyError:
                case status.unknownError:
                    return true;
                }
            }
            return false;
        });
    }
})();
