//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/showPanel.html", {
        ready: function (element, options) {
            runAnimation.addEventListener("click", togglePanelUI, false);
            myPanel = element.querySelector("#myPanel");
        }
    });

    var animating = WinJS.Promise.wrap();
    var myPanel;

    function togglePanelUI() {
        if (runAnimation.innerHTML === "Show panel") {
            runAnimation.innerHTML = "Hide panel";

            // If element is already animating, wait until current animation is complete before starting the show animation.
            animating = animating
                .then(function () {
                    // Set desired final opacity on the UI element.
                    myPanel.style.opacity = "1";

                    // Run show panel animation.
                    // Element animates from the specified offset to its actual position.
                    // For a panel that is located at the edge of the screen, the offset should be the same size as the panel element.
                    // When possible, use the default offset by leaving the offset argument empty to get the best performance.
                    return WinJS.UI.Animation.showPanel(myPanel);
                });
        } else {
            runAnimation.innerHTML = "Show panel";

            // If element is still animating in, wait until current animation is complete before starting the hide animation.
            animating = animating
                .then(function () { return WinJS.UI.Animation.hidePanel(myPanel); })
                .then(
                    // On animation completion, set final opacity to 0 to hide UI element.
                    function () { myPanel.style.opacity = "0"; });
        }
    }
})();
