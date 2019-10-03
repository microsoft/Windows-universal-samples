//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var ViewManagement = Windows.UI.ViewManagement;
    var ApplicationViewWindowingMode = ViewManagement.ApplicationViewWindowingMode;
    var ApplicationView = ViewManagement.ApplicationView;

    var page = WinJS.UI.Pages.define("/html/scenario2-launch.html", {
        ready: function (element, options) {
            launchAtSize.addEventListener("click", onLaunchAtSizeChanged);

            launchAtSize.checked = ApplicationView.preferredLaunchWindowingMode != ApplicationViewWindowingMode.auto;
        }
    });

    // If you set the preferredLaunchViewSize and preferredLaunchWindowingMode
    // before calling WinJS.UI.processAll, then it will take effect
    // on the current launch. Otherwise, it takes effect at the next launch.
    function onLaunchAtSizeChanged() {
        if (launchAtSize.checked) {
            // For best results, set the preferredLaunchViewSize before setting
            // the preferredLaunchWindowingMode.
            ApplicationView.preferredLaunchViewSize = { width: 600, height: 500 };
            ApplicationView.preferredLaunchWindowingMode = ApplicationViewWindowingMode.preferredLaunchViewSize;
        } else {
            ApplicationView.preferredLaunchWindowingMode = ApplicationViewWindowingMode.auto;
        }
    }
})();
