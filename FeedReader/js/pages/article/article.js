//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    // Helper variables.
    var ui = WinJS.UI;

    ui.Pages.define("/pages/article/article.html", {
        ready: function (element, options) {
            /// <summary>
            /// This function is called whenever a user navigates to this page. It populates the
            /// page elements with the app's data.
            /// </summary>
            /// <param name="element">
            /// The DOM element that contains all the content for the page.
            /// </param>
            /// <param name="options">
            /// An object that contains one or more property/value pairs to apply to the PageControl.
            /// </param>
            var appbar = document.getElementById("appbar").winControl;
            appbar.disabled = true;

            var article = options.article;
            element.querySelector(".titlearea .pagetitle").innerText = article.title;
            element.querySelector("#article #content").innerHTML = article.body;
            element.querySelector("#articleArea").focus();
        },

        unload: function () {
            /// <summary>This function is called when the user navigates away from the page.</summary>
            var appbar = document.getElementById("appbar").winControl;
            appbar.disabled = false;
        }

    });
})();