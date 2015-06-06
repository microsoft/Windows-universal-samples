//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/reposition.html", {
        ready: function (element, options) {
            repositionControl.addEventListener("pointerup", runRepositionAnimation, false);
        }
    });

    var distance = 0;

    function runRepositionAnimation() {
        var newDistance = parseFloat(repositionControl.value);
        var elements;

        // If movement is towards the right, animate elements in order from right to left.
        // Otherwise, animate elements in order from left to right.
        if (distance < newDistance) {
            elements = [target3, target2, target1];
        } else {
            elements = [target1, target2, target3];
        }
        distance = newDistance;

        // Create reposition animation
        var reposition = WinJS.UI.Animation.createRepositionAnimation(elements);
        // Change position of elements by resizing another element in the layout flow
        spacingDiv.style.width = parseInt(distance) + "px";
        // Execute reposition animation
        reposition.execute();
    }
})();
