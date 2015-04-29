//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/updateTile.html", {
        ready: function (element, options) {
            runAnimation.addEventListener("click", peekToggle, false);
        }
    });

    function peekToggle() {
        if (runAnimation.innerHTML === "Peek") {
            peekTile();
            runAnimation.innerHTML = "Unpeek";
        } else {
            unpeekTile();
            runAnimation.innerHTML = "Peek";
        }
    }

    // Play the Peek animation
    function peekTile() {
        // Create peek animation
        var peekAnimation = WinJS.UI.Animation.createPeekAnimation([tile1, tile2]);

        // Reposition tiles to their desired post-animation position
        tile1.style.top = "-150px";
        tile2.style.top = "-150px";

        // Execute animation
        peekAnimation.execute();
    }

    // Unpeek back to the original state
    function unpeekTile() {
        // Create peek animation
        var peekAnimation = WinJS.UI.Animation.createPeekAnimation([tile1, tile2]);

        // Reposition tiles to their desired post-animation position
        tile1.style.top = "0px";
        tile2.style.top = "0px";

        // Execute animation
        peekAnimation.execute();
    }
})();
