//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/dragBetween.html", {
        ready: function (element, options) {
            runAnimation.addEventListener("click", dragBetweenToggle, false);
        }
    });

    function dragBetweenToggle() {
        if (runAnimation.innerHTML === "Drag Between Enter") {
            dragBetweenEnter();
            runAnimation.innerHTML = "Drag Between Leave";
        } else {
            dragBetweenLeave();
            runAnimation.innerHTML = "Drag Between Enter";
        }
    }

    function dragBetweenEnter() {
        // Play the Drag Between Enter animation.
        // Offsets should be chosen so the elements move in the direction they would if the dragged item were dropped. For a vertical list, this is up and down.
        WinJS.UI.Animation.dragBetweenEnter([box1, box2], [{ top: "-40px", left: "0px" }, { top: "40px", left: "0px" }]);
    }

    function dragBetweenLeave() {
        // Play the Drag Between Leave animation.
        // Transforms applied by the Drag Between Enter animation are removed.
        WinJS.UI.Animation.dragBetweenLeave([box1, box2]);
    }
})();
