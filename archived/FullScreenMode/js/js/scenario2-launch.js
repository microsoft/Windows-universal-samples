//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var ViewManagement = Windows.UI.ViewManagement;
    var ApplicationViewWindowingMode = ViewManagement.ApplicationViewWindowingMode;
    var ApplicationView = ViewManagement.ApplicationView;

    var page = WinJS.UI.Pages.define("/html/scenario2-launch.html", {
        ready: function (element, options) {
            launchInFullScreenMode.addEventListener("click", onLaunchInFullScreenModeChanged);

            launchInFullScreenMode.checked = ApplicationView.preferredLaunchWindowingMode == ApplicationViewWindowingMode.fullScreen;
        }
    });

    function onLaunchInFullScreenModeChanged() {
        ApplicationView.preferredLaunchWindowingMode = launchInFullScreenMode.checked ? ApplicationViewWindowingMode.fullScreen : ApplicationViewWindowingMode.auto;
    }
})();
