//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/showEdgeUI.html", {
        ready: function (element, options) {
            runAnimation.addEventListener("click", toggleEdgeUI, false);
            myEdgeUI = element.querySelector("#myEdgeUI");
        }
    });

    var myEdgeUI;
    var animating = WinJS.Promise.wrap();

    function toggleEdgeUI() {
        if (runAnimation.innerHTML === "Show edge UI") {
            runAnimation.innerHTML = "Hide edge UI";

            // If element is already animating, wait until current animation is complete before starting the show animation.
            animating = animating
                .then(function () {
                    // Make the element visible.
                    myEdgeUI.style.visibility = "visible";

                    // Run show edge UI animation.
                    // Element animates from the specified offset to its actual position.
                    // For UI located at the edge of the screen, the offset should be the same size as the UI element.
                    // When possible, use the default offset by leaving the offset argument empty to get the best performance.
                    return WinJS.UI.Animation.showEdgeUI(myEdgeUI);
                });
        } else {
            runAnimation.innerHTML = "Show edge UI";

            // If element is still animating in, wait until current animation is complete before starting the hide animation.
            animating = animating
                .then(function () { return WinJS.UI.Animation.hideEdgeUI(myEdgeUI); })
                .then(
                    // On animation completion, hide the UI element.
                    function () { myEdgeUI.style.visibility = "hidden"; });
        }
    }
})();
