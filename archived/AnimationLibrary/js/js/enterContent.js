//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/enterContent.html", {
        ready: function (element, options) {
            runAnimation.addEventListener("click", runEnterContentAnimation, false);
            contentArea.style.opacity = "0";
        }
    });

    function runEnterContentAnimation() {
        if (runAnimation.innerHTML === "Reset") {
            contentArea.style.opacity = "0";
            runAnimation.innerHTML = "Run animation";
        } else {
            contentHost.style.overflow = "hidden";

            // Run the enterContent animation
            // The animation will cause opacity to transition to 1
            // Use the recommended offset by leaving the offset argument empty to get the best performance
            WinJS.UI.Animation.enterContent(contentArea, null).done(
                function () {
                    contentHost.style.overflow = "auto";
                });

            runAnimation.innerHTML = "Reset";
        }
    }
})();
