//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var ViewManagement = Windows.UI.ViewManagement;
    var UIViewSettings = ViewManagement.UIViewSettings;
    var UserInteractionMode = ViewManagement.UserInteractionMode;

    var page = WinJS.UI.Pages.define("/html/scenario1-basic.html", {
        ready: function (element, options) {
            // The resize event is raised when the user interaction mode changes.
            window.addEventListener("resize", onresize);
            onresize();
        },

        unload: function () {
            window.removeEventListener("resize", onresize);
        }
    });

    function onresize() {
        var settings = UIViewSettings.getForCurrentView();
        if (settings.userInteractionMode == UserInteractionMode.mouse) {
            currentInteractionMode.innerText = "mouse";
            WinJS.Utilities.removeClass(document.body, "touch");
        } else {
            currentInteractionMode.innerText = "touch";
            WinJS.Utilities.addClass(document.body, "touch");
        }
    }

})();
