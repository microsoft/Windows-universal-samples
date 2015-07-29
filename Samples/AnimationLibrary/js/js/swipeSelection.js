//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/swipeSelection.html", {
        ready: function (element, options) {
            runAnimation.addEventListener("click", toggleSelection, false);
            selectionMark.style.display = "none";
        }
    });

    function toggleSelection() {
        if (selectionMark.style.display === "inherit") {
            swipeDeselect();
            runAnimation.innerText = "Swipe select";
        } else {
            swipeSelect();
            runAnimation.innerText = "Swipe deselect";
        }
    }

    function swipeSelect() {
        // Programmatically move the item down, then run the animation.
        // Normally, it would be the user moving the element down via direct manipulation.
        selectionTarget.style.transform = "translate(0px, 100px)";
        selectionMark.style.transform = "translate(0px, 100px)";
        WinJS.UI.Animation.swipeSelect(selectionTarget, selectionMark);
        selectionMark.style.display = "inherit";
    }

    function swipeDeselect() {
        // Programmatically move the item down, then run the animation.
        // Normally, it would be the user moving the element down via direct manipulation.
        selectionTarget.style.transform = "translate(0px, 100px)";
        selectionMark.style.transform = "translate(0px, 100px)";
        WinJS.UI.Animation.swipeDeselect(selectionTarget, selectionMark);
        selectionMark.style.display = "none";
    }
})();
