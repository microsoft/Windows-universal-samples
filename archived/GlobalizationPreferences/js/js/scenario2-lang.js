//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var outputText;

    var page = WinJS.UI.Pages.define("/html/scenario2-lang.html", {
        ready: function (element, options) {
            document.getElementById("showResults").addEventListener("click", showResults);
            outputText = document.getElementById("outputText");
        }
    });

    function reportLanguageData(lang) {
        return "Display Name: " + lang.displayName + "\n" +
            "Language Tag: " + lang.languageTag + "\n" +
            "Native Name: " + lang.nativeName + "\n" +
            "Script Code: " + lang.script + "\n\n";
    }

    function showResults() {
        // This scenario uses the Windows.System.UserProfile.GlobalizationPreferences class to
        // obtain the user's preferred language characteristics.
        var topUserLanguage = Windows.System.UserProfile.GlobalizationPreferences.languages[0];
        var userLanguage = new Windows.Globalization.Language(topUserLanguage);

        // This obtains the language characteristics by providing a BCP47 tag.
        var exampleLanguage = new Windows.Globalization.Language("ja");

        // Display the results
        outputText.innerText = "User's Preferred Language\n" + reportLanguageData(userLanguage) +
            "Example Language by BCP47 tag (ja)\n" + reportLanguageData(exampleLanguage);
    }
})();
