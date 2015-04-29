//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario2_DecimalFormatting.html", {
        ready: function (element, options) {
            document.getElementById("displayButton").addEventListener("click", doDisplay, false);
        }
    });

    function doDisplay() {
        // This scenario uses the Windows.Globalization.NumberFormatting.DecimalFormatter class
        // to format a number.

        // Create formatter initialized using the current user's preference settings.
        var decimalFormat = new Windows.Globalization.NumberFormatting.DecimalFormatter();

        // Make a random number.
        var randomNumber = (Math.random() * 100000);

        // Format with the user's default preferences.
        var decimal = decimalFormat.format(randomNumber);

        // Format with grouping.
        var decimalFormat1 = new Windows.Globalization.NumberFormatting.DecimalFormatter();
        decimalFormat1.isGrouped = true;
        var decimal1 = decimalFormat1.format(randomNumber);

        // Format with grouping using French.
        var decimalFormatFR = new Windows.Globalization.NumberFormatting.DecimalFormatter(["fr-FR"], "ZZ");
        decimalFormatFR.isGrouped = true;
        var decimalFR = decimalFormatFR.format(randomNumber);

        // Illustrate how automatic digit substitution works
        var decimalFormatAR = new Windows.Globalization.NumberFormatting.DecimalFormatter(["ar"], "ZZ");
        decimalFormatAR.isGrouped = true;
        var decimalAR = decimalFormatAR.format(randomNumber);

        // Get a fixed number.
        var fixedNumber = 500;

        // Format with the user's default preferences.
        var decimal2 = decimalFormat.format(fixedNumber);

        // Format always with a decimal point.
        var decimalFormat3 = new Windows.Globalization.NumberFormatting.DecimalFormatter();
        decimalFormat3.isDecimalPointAlwaysDisplayed = true;
        decimalFormat3.fractionDigits = 0;
        var decimal3 = decimalFormat3.format(fixedNumber);

        // Format with no fractional digits or decimal point.
        var decimalFormat4 = new Windows.Globalization.NumberFormatting.DecimalFormatter();
        decimalFormat4.fractionDigits = 0;
        var decimal4 = decimalFormat4.format(fixedNumber);

        // Display the results.
        var results = "Random number (" + randomNumber + ")\n" +
                      "With current user preferences: " + decimal + "\n" +
                      "With grouping separators: " + decimal1 + "\n" +
                      "With grouping separators (fr-FR): " + decimalFR + "\n" +
                      "With digit substitution (ar): " + decimalAR + "\n\n" +
                      "Fixed number (" + fixedNumber + ")\n" +
                      "With current user preferences: " + decimal2 + "\n" +
                      "Always with a decimal point: " + decimal3 + "\n" +
                      "With no fraction digits or decimal points: " + decimal4;

        document.getElementById("output").innerText = results;
    }
})();
