//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var systemNavigationManager = Windows.UI.Core.SystemNavigationManager.getForCurrentView();

    WinJS.UI.Pages.define("/html/samplePage.html", {
        ready: function (element, options) {
            systemNavigationManager.addEventListener("backrequested", backRequested);
            systemNavigationManager.appViewBackButtonVisibility = Windows.UI.Core.AppViewBackButtonVisibility.visible;

            // Run enter page animation to animate in the page that has just been loaded
            WinJS.UI.Animation.enterPage([samplePageHeader, samplePageSection1, samplePageSection2], null);
        },
        unload: function () {
            systemNavigationManager.removeEventListener("backrequested", backRequested);
            systemNavigationManager.appViewBackButtonVisibility = Windows.UI.Core.AppViewBackButtonVisibility.collapsed;
        }
    });

    function backRequested() {
        // When navigating away from this page, run exit page animation on current page, then navigate to new page
        WinJS.UI.Animation.exitPage([samplePageHeader, samplePageSection1, samplePageSection2], null).done(
            function () {
                WinJS.Navigation.navigate("/html/transitionPages.html", "sample page");
            });
    }
})();
