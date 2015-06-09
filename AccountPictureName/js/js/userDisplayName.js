//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/userDisplayName.html", {
        ready: function (element, options) {
            document.getElementById("getDisplayName").addEventListener("click", getDisplayName, false);
        }
    });

    function getDisplayName() {
        if (Windows.System.UserProfile.UserInformation.nameAccessAllowed) {
            Windows.System.UserProfile.UserInformation.getDisplayNameAsync().done(function (result) {
                if (result) {
                    WinJS.log && WinJS.log("Display name = " + result, "sample", "status");
                } else {
                    WinJS.log && WinJS.log("No display name was returned.", "sample", "status");
                }
            });
        } else {
            WinJS.log && WinJS.log("Access to name disabled by Privacy Setting or Group Policy.", "sample", "error");
        }
    }
})();
