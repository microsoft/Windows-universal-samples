//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/userFirstAndLastName.html", {
        ready: function (element, options) {
            document.getElementById("getFirstName").addEventListener("click", getFirstName, false);
            document.getElementById("getLastName").addEventListener("click", getLastName, false);
        }
    });

    function getFirstName() {
        if (Windows.System.UserProfile.UserInformation.nameAccessAllowed) {
            Windows.System.UserProfile.UserInformation.getFirstNameAsync().done(function (result) {
                if (result) {
                    WinJS.log && WinJS.log("First name = " + result, "sample", "status");
                } else {
                    WinJS.log && WinJS.log("No first name was returned.", "sample", "status");
                }
            });
        } else {
            WinJS.log && WinJS.log("Access to name disabled by Privacy Setting or Group Policy.", "sample", "error");
        }
    }

    function getLastName() {
        if (Windows.System.UserProfile.UserInformation.nameAccessAllowed) {
            Windows.System.UserProfile.UserInformation.getLastNameAsync().done(function (result) {
                if (result) {
                    WinJS.log && WinJS.log("Last name = " + result, "sample", "status");
                } else {
                    WinJS.log && WinJS.log("No last name was returned.", "sample", "status");
                }
            });
        } else {
            WinJS.log && WinJS.log("Access to name disabled by Privacy Setting or Group Policy.", "sample", "error");
        }
    }
})();
