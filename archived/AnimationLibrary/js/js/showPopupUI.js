//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/showPopupUI.html", {
        ready: function (element, options) {
            runAnimation.addEventListener("click", togglePopupUI, false);
        }
    });

    function togglePopupUI() {
        if (runAnimation.innerHTML === "Show pop-up") {
            // Set desired final opacity on the UI element.
            myPopupUI.style.opacity = "1";

            // Run show popup animation
            WinJS.UI.Animation.showPopup(myPopupUI, null);

            runAnimation.innerHTML = "Hide pop-up";
        } else {
            // Set desired final opacity on the UI element.
            myPopupUI.style.opacity = "0";

            // Run show popup animation
            WinJS.UI.Animation.hidePopup(myPopupUI);

            runAnimation.innerHTML = "Show pop-up";
        }
    }
})();
