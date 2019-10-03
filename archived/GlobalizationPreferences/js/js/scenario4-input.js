//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var outputText;

    var page = WinJS.UI.Pages.define("/html/scenario4-input.html", {
        ready: function (element, options) {
            document.getElementById("showResults").addEventListener("click", showResults);
            outputText = document.getElementById("outputText");
        }
    });

    function showResults() {
        // This scenario uses the Windows.Globalization.Language class to obtain the user's current 
        // input language.  The language tag returned reflects the current input language specified 
        // by the user.
        var userInputLanguage = Windows.Globalization.Language.currentInputMethodLanguageTag;

        // Display the results
        outputText.innerText = "User's current input language: " + userInputLanguage;
    }
})();
