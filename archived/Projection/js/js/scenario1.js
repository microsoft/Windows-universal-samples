//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var ViewManagement = Windows.UI.ViewManagement;

    // Create ready handlers for start/stop projection button.
    var startProjectionButton;
    var stopProjectionButton;

    // Create a new view for projecting.
    var view = window.open("ms-appx:///html/secondaryView.html", null, "msHideView=yes");
    var page = WinJS.UI.Pages.define("/html/scenario1.html", {
        ready: function (element, options) {
            startProjectionButton = document.getElementById("startProjectionButton");
            startProjectionButton.addEventListener("click", startProjection, false);
        }
    });

    function startProjection() {
        // Disable Start projection button to avoid clicking it twice.
        startProjectionButton.disabled = true;

        // Start projection using the previously created secondary view.
        ViewManagement.ProjectionManager.startProjectingAsync(
            MSApp.getViewId(view),
            ViewManagement.ApplicationView.getForCurrentView().id
        ).done(function () {
            // Re-enable the Start projection button to allow starting again when stop projection from projection window.
            startProjectionButton.disabled = false;

            // Clear out the previous error message if there is any.
            WinJS.log("", "sample", "status");
        }, function (e) {
            // Re-enable the Start projection button if start projection failed.
            startProjectionButton.disabled = false;
            WinJS.log && WinJS.log(e + "\n The current app or projection view window must be active for startProjectingAsync() to succeed", "sample", "error");
        });
    }
})();

