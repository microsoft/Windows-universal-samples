//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/fadeInAndOut.html", {
        ready: function (element, options) {
            fadeIn.addEventListener("click", runFadeIn, false);
            fadeOut.addEventListener("click", runFadeOut, false);
            fadeIn.style.display = "none";
            fadeOut.style.display = "inline";
            resetOpacity.addEventListener("click", runResetOpacity, false);
        }
    });

    function runFadeIn() {
        // Run fade in animation on all targets
        WinJS.UI.Animation.fadeIn(document.querySelectorAll(".fadeTarget"));

        fadeIn.style.display = "none";
        fadeOut.style.display = "inline";
    }

    function runFadeOut() {
        // Run fade out animation on all targets
        WinJS.UI.Animation.fadeOut(document.querySelectorAll(".fadeTarget"));

        fadeIn.style.display = "inline";
        fadeOut.style.display = "none";
    }

    function runResetOpacity() {
        fadeIn.style.display = "none";
        fadeOut.style.display = "inline";

        targetParent.style.opacity = "1";

        target1.style.opacity = "1";
        target2.style.opacity = ".75";
        target3.style.opacity = ".5";
        target4.style.opacity = ".25";
    }
})();
