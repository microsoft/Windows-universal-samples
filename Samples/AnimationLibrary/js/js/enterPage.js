//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/enterPage.html", {
        ready: function (element, options) {
            runAnimation.addEventListener("click", runEnterPageAnimation, false);
        }
    });

    function runEnterPageAnimation() {
        // Get user selection from control
        var pageSections = pageSectionsControl.value;
        contentHost.style.overflow = "hidden";

        // Animate the application's entrance in the number of stages chosen by the user
        // Use the recommended offset by leaving the offset argument empty to get the best performance
        var enterPage;
        switch (pageSections) {
            case "1":
                // Animate the whole page together
                enterPage = WinJS.UI.Animation.enterPage([[scenarioHeader, input, output]], null);
                break;
            case "2":
                // Stagger the header and body
                enterPage = WinJS.UI.Animation.enterPage([scenarioHeader, [input, output]], null);
                break;
            case "3":
                // Stagger the header, content, and footer areas
                enterPage = WinJS.UI.Animation.enterPage([scenarioHeader, input, output], null);
                break;
        }
        outputText.innerText = "Page appeared in " + pageSections + " section(s).";

        enterPage.done(
            function () {
                contentHost.style.overflow = "auto";
            });
    }
})();
