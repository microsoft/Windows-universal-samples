//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario4_NumberParsing.html", {
        ready: function (element, options) {
            document.getElementById("displayButton").addEventListener("click", doDisplay, false);
        }
    });

    function doDisplay() {
        // This scenario uses the Windows.Globalization.NumberFormatting.DecimalFormatter,
        // Windows.Globalization.NumberFormatting.CurrencyFormatter and
        // Windows.Globalization.NumberFormatting.PercentFormatter classes to format and parse a number
        // percentage or currency.

        // Define percent formatters.
        var percentFormat = new Windows.Globalization.NumberFormatting.PercentFormatter();
        var percentFormatJP = new Windows.Globalization.NumberFormatting.PercentFormatter(["ja-JP"], "ZZ");
        var percentFormatFR = new Windows.Globalization.NumberFormatting.PercentFormatter(["fr-FR"], "ZZ");

        // Define decimal formatters.
        var decimalFormat = new Windows.Globalization.NumberFormatting.DecimalFormatter();
        decimalFormat.isGrouped = true;
        var decimalFormatJP = new Windows.Globalization.NumberFormatting.DecimalFormatter(["ja-JP"], "ZZ");
        decimalFormatJP.isGrouped = true;
        var decimalFormatFR = new Windows.Globalization.NumberFormatting.DecimalFormatter(["fr-FR"], "ZZ");
        decimalFormatFR.isGrouped = true;

        // Define currency formatters
        var userCurrency = Windows.System.UserProfile.GlobalizationPreferences.currencies;
        var currencyFormat = new Windows.Globalization.NumberFormatting.CurrencyFormatter(userCurrency);
        var currencyFormatJP = new Windows.Globalization.NumberFormatting.CurrencyFormatter("JPY", ["ja-JP"], "ZZ");
        var currencyFormatFR = new Windows.Globalization.NumberFormatting.CurrencyFormatter("EUR", ["fr-FR"], "ZZ");

        // Generate numbers for parsing.
        var percentNumber = 0.523;
        var decimalNumber = 12345.67;
        var currencyNumber = 1234.56;

        // Roundtrip the percent numbers by formatting and parsing.
        var percent1 = percentFormat.format(percentNumber);
        var percent1Parsed = percentFormat.parseDouble(percent1);

        var percent1JP = percentFormatJP.format(percentNumber);
        var percent1JPParsed = percentFormatJP.parseDouble(percent1JP);

        var percent1FR = percentFormatFR.format(percentNumber);
        var percent1FRParsed = percentFormatFR.parseDouble(percent1FR);

        // Generate the results for percent parsing.
        var results1 = "Percent parsing of " + percentNumber + "\n" +
                       "Formatted for current user: " + percent1 + " Parsed as current user: " + percent1Parsed + "\n" +
                       "Formatted for ja-JP: " + percent1JP + " Parsed for ja-JP: " + percent1JPParsed + "\n" +
                       "Formatted for fr-FR: " + percent1FR + " Parsed for fr-FR: " + percent1FRParsed;

        // Roundtrip the decimal numbers for formatting and parsing.
        var decimal1 = decimalFormat.format(decimalNumber);
        var decimal1Parsed = decimalFormat.parseDouble(decimal1);

        var decimal1JP = decimalFormatJP.format(decimalNumber);
        var decimal1JPParsed = decimalFormatJP.parseDouble(decimal1JP);

        var decimal1FR = decimalFormatFR.format(decimalNumber);
        var decimal1FRParsed = decimalFormatFR.parseDouble(decimal1FR);

        // Generate the results for decimal parsing.
        var results2 = "Decimal parsing of " + decimalNumber + "\n" +
                       "Formatted for current user: " + decimal1 + " Parsed as current user: " + decimal1Parsed + "\n" +
                       "Formatted for ja-JP: " + decimal1JP + " Parsed for ja-JP: " + decimal1JPParsed + "\n" +
                       "Formatted for fr-FR: " + decimal1FR + " Parsed for fr-FR: " + decimal1FRParsed;

        // Roundtrip the currency numbers for formatting and parsing.
        var currency1 = currencyFormat.format(currencyNumber);
        var currency1Parsed = currencyFormat.parseDouble(currency1);

        var currency1JP = currencyFormatJP.format(currencyNumber);
        var currency1JPParsed = currencyFormatJP.parseDouble(currency1JP);

        var currency1FR = currencyFormatFR.format(currencyNumber);
        var currency1FRParsed = currencyFormatFR.parseDouble(currency1FR);

        // Generate the results for decimal parsing.
        var results3 = "Currency parsing of " + currencyNumber + "\n" +
                       "Formatted for current user: " + currency1 + " Parsed as current user: " + currency1Parsed + "\n" +
                       "Formatted for ja-JP: " + currency1JP + " Parsed for ja-JP: " + currency1JPParsed + "\n" +
                       "Formatted for fr-FR: " + currency1FR + " Parsed for fr-FR: " + currency1FRParsed;

        // Display the results.
        var results = results1 + "\n\n" + results2 + "\n\n" + results3;

        document.getElementById("output").innerText = results;
    }
})();
