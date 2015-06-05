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

    // For a panel that is located at the edge of the screen, the offset should be the same size as the panel element.
    // When possible, use the default offset by leaving the offset argument empty to get the best performance.
    function getPanelOffset() {
        return myPanel.clientWidth == 364 ? null : { top: "0px", left: myPanel.clientWidth + "px", rtlflip: true };
    }

    function togglePanelUI() {
        if (runAnimation.innerHTML === "Show panel") {
            runAnimation.innerHTML = "Hide panel";

            // If element is already animating, wait until current animation is complete before starting the show animation.
            animating = animating
                .then(function () {
                    // Make the element visible.
                    myPanel.style.visibility = "visible";

                    // Run show panel animation.
                    // Element animates from the specified offset to its actual position.
                    return WinJS.UI.Animation.showPanel(myPanel, getPanelOffset());
                });
        } else {
            runAnimation.innerHTML = "Show panel";

            // If element is still animating in, wait until current animation is complete before starting the hide animation.
            animating = animating
                .then(function () { return WinJS.UI.Animation.hidePanel(myPanel, getPanelOffset()); })
                .then(
                    // On animation completion, hide the UI element.
                    function () { myPanel.style.visibility = "hidden"; });
        }
    }
})();
