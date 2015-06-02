//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario3.html", {
        ready: function (element, options) {
            // Listeners used in all scenarios
            document.querySelector('form').addEventListener("submit", formNoSubmit, false);

            var inputs = document.querySelectorAll("form input");
            for (var i = 0; i < inputs.length; i++) {
                inputs[i].addEventListener("focus", formReset, false);
            }

            // Listeners used in Scenario 3
            id("bYear3").addEventListener("change", checkBirthYear, false);
            id("email3").addEventListener("invalid", checkForError, false);
            id("password3").addEventListener("invalid", checkForError, false);
            id("tel3").addEventListener("invalid", checkForError, false);
            id("username3").addEventListener("invalid", checkForError, false);
            id("email3").addEventListener("change", reCheckForError, false);
            id("password3").addEventListener("change", reCheckForError, false);
            id("tel3").addEventListener("change", reCheckForError, false);
            id("username3").addEventListener("change", reCheckForError, false);

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

    function id(elementId) {
        return document.getElementById(elementId);
    }

    // Functions used in scneario 3
    function checkBirthYear() {
        if (this.validity.rangeOverflow) {
            this.setCustomValidity("You must be at least 14 years old to use this site.");
        } else {
            this.setCustomValidity("");
        }
    }

    function checkForError(e) {

        if (!this.validity.valid) {
            this.previousElementSibling.style.display = "block";
        } else {
            this.previousElementSibling.style.display = "none";
        }
        e.preventDefault();
    }

    function reCheckForError(e) {
        if (this.validity.valid) {
            this.previousElementSibling.style.display = "none";
        }
    }

})();
