//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/customAnimation.html", {
        ready: function (element, options) {
            animation.addEventListener("click", runCustomShowAnimation, false);
            transition.addEventListener("click", runCustomShowTransition, false);
            reset.addEventListener("click", resetView, false);
        }
    });

    function runCustomShowAnimation() {
        var showAnimation = WinJS.UI.executeAnimation(
            target,
            {
                keyframe: "custom-opacity-in",
                property: "opacity",
                delay: 0,
                duration: 500,
                timing: "linear",
                from: 0,
                to: 1
            }
        );
    }

    function runCustomShowTransition() {
        var showTransition = WinJS.UI.executeTransition(
            target,
            {
                property: "opacity",
                delay: 0,
                duration: 500,
                timing: "linear",
                to: 1
            }
        );
    }

    function resetView() {
        target.style.opacity = "0";
    }
})();
