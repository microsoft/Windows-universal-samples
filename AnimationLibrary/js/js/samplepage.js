//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    WinJS.UI.Pages.define("/html/samplePage.html", {
        ready: function (element, options) {
            returnButton.addEventListener("click", transitionBetweenPages, false);
//            rootGrid.style.overflow= "auto";
//            content.style.overflow = "visible";

            // Run enter page animation to animate in the page that has just been loaded
            WinJS.UI.Animation.enterPage([samplePageHeader, samplePageSection1, samplePageSection2], null);
        },
        unload: function () {
//            rootGrid.style.overflow = "visible";
//            content.style.overflow = "auto";
        }
    });

    function transitionBetweenPages() {
        // When navigating away from this page, run exit page animation on current page, then navigate to new page
        WinJS.UI.Animation.exitPage([samplePageHeader, samplePageSection1, samplePageSection2], null).done(
            function () {
                WinJS.Navigation.navigate("/html/transitionPages.html", "sample page");
            });
    }
})();
