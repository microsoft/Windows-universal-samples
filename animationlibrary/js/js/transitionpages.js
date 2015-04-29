//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/transitionPages.html", {
        ready: function (element, options) {
            runAnimation.addEventListener("click", transitionBetweenPages, false);

            // If navigating from sample page, run the enter page animation on the incoming page
            if (WinJS.Navigation.state === "sample page") {
                outputText.innerText = "Transitioned back from sample page.";

                // Use the recommended offset by leaving the offset argument empty to get the best performance
                WinJS.UI.Animation.enterPage([scenarioHeader, [input, output]], null);
            }
        }
    });

    function transitionBetweenPages() {
        // Run the exit page animation on the current page, and then navigate to the new page
        // Use the recommended offset by leaving the offset argument empty to get the best performance
        WinJS.UI.Animation.exitPage([scenarioHeader, [input, output]], null).done(
            function () {
                WinJS.Navigation.navigate("/html/samplePage.html");
            });
}
})();
