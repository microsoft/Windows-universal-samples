//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/dragAndDrop.html", {
        ready: function (element, options) {
            runAnimation.addEventListener("click", dragSourceToggle, false);
        }
    });

    function dragSourceToggle() {
        if (runAnimation.innerHTML === "Drag Source Start") {
            dragSourceStart();
            runAnimation.innerHTML = "Drag Source End";
        } else {
            dragSourceEnd();
            runAnimation.innerHTML = "Drag Source Start";
        }
    }

    function dragSourceStart() {
        // Play the Drag Source Start animation.
        // Affected objects are objects whose position may be affected by the drag and drop, such as the other items in a re-orderable list.
        WinJS.UI.Animation.dragSourceStart(dragTarget, [affected1, affected2]);
    }

    function dragSourceEnd() {
        // Play the Drag Source End animation to reverse the styles applied by the Drag Source Start animation and move the dropped element to its final position.
        // Typically, when the item is dropped it will not be at its rest location and the developer will need to specify an offset to move it back to its desired location.
        // For this sample, the default offset is used.
        WinJS.UI.Animation.dragSourceEnd(dragTarget, { top: "0px", left: "0px" }, [affected1, affected2]);
    }
})();
