//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var ViewManagement = Windows.UI.ViewManagement;
    var ApplicationView = ViewManagement.ApplicationView;

    var page = WinJS.UI.Pages.define("/html/scenario1-basic.html", {
        ready: function (element, options) {
            resizeView.addEventListener("click", onResizeView);
            setMinimumSize.addEventListener("click", onSetMinimumSize);
            window.addEventListener("resize", onresize);
            onresize();
        },

        unload: function () {
            window.removeEventListener("resize", onresize);
        }
    });

    function onResizeView() {
        var view = ApplicationView.getForCurrentView();
        if (view.tryResizeView({ width: 600, height: 500 })) {
            WinJS.log && WinJS.log("Resizing to 600 \u2715 500", "samples", "status");
            // The resize event will be raised when the resize is complete.
        } else {
            // The system does not support resizing, or the provided size is out of range.
            WinJS.log && WinJS.log("Failed to resize view", "samples", "error");
        }
    }
    
    function onSetMinimumSize()
    {
        var view = ApplicationView.getForCurrentView();
        if (setMinimumSize.checked) {
            // If this size is not permitted by the system, the nearest permitted value is used.
            view.setPreferredMinSize({ width: 300, height: 200 });
        } else {
            // Passing width = height = 0 returns to the default system behavior.
            view.setPreferredMinSize({ width: 0, height: 0 });
        }
    }

    function onresize() {
        reportViewWidth.innerText = window.innerWidth;
        reportViewHeight.innerText = window.innerHeight;
    }
})();
