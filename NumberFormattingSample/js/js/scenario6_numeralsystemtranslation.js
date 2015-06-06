//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario6_NumeralSystemTranslation.html", {
        ready: function (element, options) {
            document.getElementById("displayButton").addEventListener("click", doDisplay, false);
        }
    });

    function doDisplay() {
        // Text for which translation of the numeral system will be performed.  Please note that translation only happens between
        // Latin and any other supported numeral system.  Translation between numeral systems is not a supported scenario.
        var stringToTranslate = "These are the 10 digits of a numeral system: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9";

        // Variable where we keep the results of the scenario
        var results = "Original string: " + stringToTranslate + "\n\n";

        // The numeral system translator is initialized based on the current application language.
        var numeralTranslator = new Windows.Globalization.NumberFormatting.NumeralSystemTranslator();

        // Do translation
        results = results + "Using application settings (" + numeralTranslator.numeralSystem + "): ";
        results = results + numeralTranslator.translateNumerals(stringToTranslate) + "\n";

        // Switch to a different numeral system
        numeralTranslator.numeralSystem = "hanidec";

        // Do translation
        results = results + "Using numeral system via property (" + numeralTranslator.numeralSystem + " ): ";
        results = results + numeralTranslator.translateNumerals(stringToTranslate) + "\n";

        // Create a converter using a language list to initialize the numeral system to an appropriate default
        numeralTranslator = new Windows.Globalization.NumberFormatting.NumeralSystemTranslator(["ar-SA", "en-US"]); 

        // Do translation
        results = results + "Using numeral system via language list (" + numeralTranslator.numeralSystem + "): ";
        results = results + numeralTranslator.translateNumerals(stringToTranslate) + "\n";

        // Display the results.
        document.getElementById("output").innerText = results;
    }
})();
