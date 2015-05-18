//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var UserProfile = Windows.System.UserProfile;
    var page = WinJS.UI.Pages.define("/html/AddImageFeedScenario.html", {
        ready: function (element, options) {
            document.getElementById("setDefaultButton").addEventListener("click", setDefaultButtonClick, false);
            document.getElementById("removeFeedButton").addEventListener("click", removeFeedButtonClick, false);
        }
    });

    function setDefaultButtonClick() {
        // Pass a URI to your enclosure-based feed of lock screen images.  These must be JPG or PNG files.
        // Note that the sample URL given here is deliberately non-functional.
        UserProfile.LockScreen.requestSetImageFeedAsync(new Windows.Foundation.Uri("http://contoso.com/myimagerss.xml")).then(function (result) {
            if (result === UserProfile.SetImageFeedResult.success) {
                WinJS.log && WinJS.log("Called RequestSetImageFeedAsync: the specified URL was set as the default.", "sample", "status");
            }
            else if (result === UserProfile.SetImageFeedResult.changeDisabled) {
                WinJS.log && WinJS.log("Called RequestSetImageFeedAsync: call succeeded but group policy has the lock screen image slide show turned off.", "sample", "error");
            }
            else
            {
                WinJS.log && WinJS.log("Called RequestSetImageFeedAsync: the specified URL was not set as the default.", "sample", "error");
            }
        });
    }

    function removeFeedButtonClick() {
        var fResult = UserProfile.LockScreen.tryRemoveImageFeed();
        if (fResult)
        {
            WinJS.log && WinJS.log("Called TryRemoveImageFeed: the associated URL registration was removed.", "sample", "status");
        }
        else
        {
            WinJS.log && WinJS.log("Called TryRemoveImageFeed: the associated URL registration removal failed.", "sample", "error");
        }
    }
})();
