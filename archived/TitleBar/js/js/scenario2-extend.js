//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var ViewManagement = Windows.UI.ViewManagement;
    var ApplicationView = ViewManagement.ApplicationView;

    // CoreTitleBarHelper is a helper class provided in the CoreViewHelpers project.
    // It allows JavaScript to access properties and events of the
    // CoreApplicationViewTitleBar object.
    var titleBarHelper = CoreViewHelpers.CoreTitleBarHelper.getForCurrentView();

    var page = WinJS.UI.Pages.define("/html/scenario2-extend.html", {
        ready: function (element, options) {
            toggleFullScreenMode.addEventListener("click", onToggleFullScreenMode);
            extendView.addEventListener("click", onExtendView);

            // The resize event is raised when the view enters or exits full screen mode.
            // This scenario subscribes to the event just so it can update the
            // "full screen" button.
            window.addEventListener("resize", onresize);
            onresize();
        },

        unload: function () {
            window.removeEventListener("resize", onresize);
        }
    });

    function onToggleFullScreenMode() {
        var view = ApplicationView.getForCurrentView();
        if (view.isFullScreenMode) {
            view.exitFullScreenMode();
            WinJS.log && WinJS.log("Exited full screen mode", "samples", "status");
        } else {
            if (view.tryEnterFullScreenMode()) {
                WinJS.log && WinJS.log("Entered full screen mode", "samples", "status");
            } else {
                WinJS.log && WinJS.log("Failed to enter full screen mode", "samples", "error");
            }
        }
    }

    function onExtendView() {
        var extend = extendView.checked;
        titleBarHelper.extendViewIntoTitleBar = extend;
        if (extend) {
            SdkSample.addCustomTitleBar().done();
        } else {
            SdkSample.removeCustomTitleBar().done();
        }
    }

    function onresize() {
        var view = ApplicationView.getForCurrentView();
        var isFullScreenMode = view.isFullScreenMode;
        toggleFullScreenMode.innerText = WinJS.UI.AppBarIcon[isFullScreenMode ? "backtowindow" : "fullscreen"];
        reportFullScreenMode.innerText = isFullScreenMode ? "is in" : "is not in";
    }

})();
