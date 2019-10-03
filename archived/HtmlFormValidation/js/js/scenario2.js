//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario2.html", {
        ready: function (element, options) {
            // Listeners used in all scenarios
            document.querySelector('form').addEventListener("submit", formNoSubmit, false);

            var inputs = document.querySelectorAll("form input");
            for (var i = 0; i < inputs.length; i++) {
                inputs[i].addEventListener("focus", formReset, false);
            }
        }
    });

    // Functions used in all scenarios
    function formNoSubmit(e) {
        this.nextElementSibling.className = "success";
        e.preventDefault();
    }

    function formReset(e) {
        this.form.nextElementSibling.className = "hide";
    }

})();
