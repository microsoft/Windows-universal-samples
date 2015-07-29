//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario1_PercentPermilleFormatting.html", {
        ready: function (element, options) {
            document.getElementById("displayButton").addEventListener("click", doDisplay, false);
       }
    });

    function doDisplay() {
        // This scenario uses the Windows.Globalization.NumberFormatting.PercentFormatter and
        // the Windows.Globalization.NumberFormatting.PermilleFormatter classes to format numbers
        // as a percent or a permille.

        // Create formatters initialized using the current user's preference settings.
        var percentFormat = new Windows.Globalization.NumberFormatting.PercentFormatter();
        var permilleFormat = new Windows.Globalization.NumberFormatting.PermilleFormatter();

        // Make a random number.
        var randomNumber = Math.random();

        // Format with current user preferences.
        var percent = percentFormat.format(randomNumber);
        var permille = permilleFormat.format(randomNumber);

        // Get a fixed number.
        var fixedNumber = 500;

        // Format with grouping using default.
        var percentFormat1 = new Windows.Globalization.NumberFormatting.PercentFormatter();
        percentFormat1.isGrouped = true;
        var percent1 = percentFormat1.format(fixedNumber);

        // Format with grouping using French.
        var percentFormatFR = new Windows.Globalization.NumberFormatting.PercentFormatter(["fr-FR"], "ZZ");
        percentFormatFR.isGrouped = true;
        var percentFR1 = percentFormatFR.format(randomNumber);
        var percentFR2 = percentFormatFR.format(fixedNumber);

        // Format with grouping using Arabic.
        var percentFormatAR = new Windows.Globalization.NumberFormatting.PercentFormatter(["ar"], "ZZ");
        percentFormatAR.isGrouped = true;
        percentFormatAR.fractionDigits = 2;
        var percentAR = percentFormatAR.format(fixedNumber);
        var permilleFormatAR = new Windows.Globalization.NumberFormatting.PermilleFormatter(["ar"], "ZZ");
        var permilleAR = permilleFormatAR.format(randomNumber);

        // Format with no fractional digits using default.
        var percentFormat2 = new Windows.Globalization.NumberFormatting.PercentFormatter();
        percentFormat2.fractionDigits = 0;
        var percent2 = percentFormat2.format(fixedNumber);

        // Format always with a decimal point.
        var percentFormat3 = new Windows.Globalization.NumberFormatting.PercentFormatter();
        percentFormat3.isDecimalPointAlwaysDisplayed = true;
        percentFormat3.fractionDigits = 0;
        var percent3 = percentFormat3.format(fixedNumber);

        // Display the results.
        var results = "Random number (" + randomNumber + ")\n" +
                      "Percent formatted: " + percent + "\n" +
                      "Permille formatted: " + permille + "\n\n" +
                      "Language-specific percent formatted: " + percentFR1 + "\n" +
                      "Language-specific permille formatted: " + permilleAR + "\n\n" +
                      "Fixed number (" + fixedNumber + ")\n" +
                      "Percent formatted (grouped): " + percent1 + "\n" +
                      "Percent formatted (grouped as fr-FR): " + percentFR2 + "\n" +
                      "Percent formatted (grouped w/digits as ar): " + percentAR + "\n" +
                      "Percent formatted (no fractional digits): " + percent2 + "\n" +
                      "Percent formatted (always with a decimal point): " + percent3;

        document.getElementById("output").innerText = results;
    }

})();
