//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    var ViewManagement = Windows.UI.ViewManagement;
    // Create a ready handler for stop projection button.
    var stopProjectionButton;
   
    document.addEventListener("DOMContentLoaded", function () {
        document.getElementById("swapDisplayButton").addEventListener("click", swapDisplays, false);
        stopProjectionButton = document.getElementById("stopProjectionButton");
        stopProjectionButton.addEventListener("click", stopProjection, false);
        // Initially Stop Projection button is enabled.
        stopProjectionButton.disabled = false;
    }, false);

    function swapDisplays() {
        if (ViewManagement.ProjectionManager.projectionDisplayAvailable) {
            ViewManagement.ProjectionManager.swapDisplaysForViewsAsync(
                ViewManagement.ApplicationView.getForCurrentView().id,
                MSApp.getViewId(window.opener)
                ).done(null
                , function (e) {
                    WinJS.log && WinJS.log(e + "\n The current app or projection view window must be active for swapDisplaysForViewsAsync() to succeed", "sample", "error");
                });
        }
    }

    function stopProjection() {
        // Disable Stop Projection button to avoid falsely clicking it twice.
        stopProjectionButton.disabled = true;

        ViewManagement.ProjectionManager.stopProjectingAsync(
            ViewManagement.ApplicationView.getForCurrentView().id,
            MSApp.getViewId(window.opener)
        ).done(function () {
            // Re-enable the Stop projection button to allow stop projection when projecting again.
            stopProjectionButton.disabled = false;
        },
            function (e) {
                // Re-enable the Stop projection button if stop projection failed.
                stopProjectionButton.disabled = false;
                WinJS.log && WinJS.log(e + "\n The current app or projection view window must be active for stopProjectingAsync() to succeed", "sample", "error");
            });
    }
})();