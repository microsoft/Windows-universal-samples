//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var ViewManagement = Windows.UI.ViewManagement;

    // Create ready handlers for start/stop projection button.
    var startProjectionButton;
    var requestAndStartProjectionButton;

    // Create a new view for projecting.
    var view = window.open("ms-appx:///html/secondaryView.html", null, "msHideView=yes");
    var page = WinJS.UI.Pages.define("/html/scenario2.html", {
        ready: function (element, options) {
            startProjectionButton = document.getElementById("startProjectionButton");
            requestAndStartProjectionButton = document.getElementById("requestAndStartProjectionButton");
            startProjectionButton.addEventListener("click", startProjection, false);
            requestAndStartProjectionButton.addEventListener("click", requestAndStartProjection, false);
            startProjectionButton.hidden = !ViewManagement.ProjectionManager.projectionDisplayAvailable;
            requestAndStartProjectionButton.hidden = ViewManagement.ProjectionManager.projectionDisplayAvailable;

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

    function requestAndStartProjection() {
        // Disable Start projection button to avoid clicking it twice.
        requestAndStartProjectionButton.disabled = true;

        // Start projection using the previously created secondary view.
        var pickerLocation = {
            height: 300.0,
            width: 200.0,
            x: 470.0,
            y: 0.0
        };
        ViewManagement.ProjectionManager.requestStartProjectingAsync(
            MSApp.getViewId(view),
            ViewManagement.ApplicationView.getForCurrentView().id,
            pickerLocation,
            Windows.UI.Popups.Placement.above
        ).done(function () {
            // Re-enable the Start projection button to allow starting again when stop projection from projection window.
            requestAndStartProjectionButton.disabled = false;

            // Clear out the previous error message if there is any.
            WinJS.log("", "sample", "status");
        }, function (e) {
            // Re-enable the Start projection button if start projection failed.
            requestAndStartProjectionButton.disabled = false;
            WinJS.log && WinJS.log(e + "\n The current app or projection view window must be active for requestAndStartProjectingAsync() to succeed", "sample", "error");
        });
    }
})();

