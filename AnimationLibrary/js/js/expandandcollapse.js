//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/expandAndCollapse.html", {
        ready: function (element, options) {
            runAnimation.addEventListener("click", toggleExpandOrCollapse, false);
            expandedItem.style.display = "none";
        }
    });

    function toggleExpandOrCollapse() {
        if (expandedItem.style.display === "none") {
            expand(expandedItem, document.querySelectorAll(".affectedItem"));
            runAnimation.innerText = "Collapse";
        } else {
            collapse(expandedItem, document.querySelectorAll(".affectedItem"));
            runAnimation.innerText = "Expand";
        }
    }

    function expand(element, affected) {
        // Create expand animation.
        var expandAnimation = WinJS.UI.Animation.createExpandAnimation(element, affected);

        // Insert expanding item into document flow. This will change the position of the affected items.
        element.style.display = "block";
        element.style.position = "inherit";
        element.style.opacity = "1";

        // Execute expand animation.
        expandAnimation.execute();
    }

    function collapse(element, affected) {
        // Create collapse animation.
        var collapseAnimation = WinJS.UI.Animation.createCollapseAnimation(element, affected);

        // Remove collapsing item from document flow so that affected items reflow to their new position.
        // Do not remove collapsing item from DOM or display at this point, otherwise the animation on the collapsing item will not display
        element.style.position = "absolute";
        element.style.opacity = "0";

        // Execute collapse animation.
        collapseAnimation.execute().done(
            // After animation is complete, remove from display.
            function () { element.style.display = "none"; }
        );
    }
})();
