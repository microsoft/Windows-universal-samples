//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario3_CurrencyFormatting.html", {
        ready: function (element, options) {
            document.getElementById("displayButton").addEventListener("click", doDisplay, false);
        }
    });

    function doDisplay() {
        // This scenario uses the Windows.Globalization.NumberFormatting.CurrencyFormatter class
        // to format a number as a currency.

        // Determine the current user's default currency.
        var userCurrency = Windows.System.UserProfile.GlobalizationPreferences.currencies;

        // Generate numbers used for formatting.
        var wholeNumber = 12345;
        var fractionalNumber = 12345.67;

        // Create formatter initialized using the current user's preference settings for number formatting.
        var userCurrencyFormat = new Windows.Globalization.NumberFormatting.CurrencyFormatter(userCurrency);
        var currencyDefault = userCurrencyFormat.format(fractionalNumber);

        // Create a formatter initialized to a specific currency, in this case it's the US Dollar, but with the default number formatting for the current user.
        var currencyFormatUSD = new Windows.Globalization.NumberFormatting.CurrencyFormatter(Windows.Globalization.CurrencyIdentifiers.usd); 
        var currencyUSD = currencyFormatUSD.format(fractionalNumber);

        // Create a formatter initialized to a specific currency, in this case it's the Euro with the default number formatting for France.
        var currencyFormatEuroFR = new Windows.Globalization.NumberFormatting.CurrencyFormatter(Windows.Globalization.CurrencyIdentifiers.eur, ["fr-FR"], "ZZ");
        var currencyEuroFR = currencyFormatEuroFR.format(fractionalNumber);

        // Create a formatter initialized to a specific currency, in this case it's the Euro with the default number formatting for Ireland.
        var currencyFormatEuroIE = new Windows.Globalization.NumberFormatting.CurrencyFormatter(Windows.Globalization.CurrencyIdentifiers.eur, ["gd-IE"], "IE");
        var currencyEuroIE = currencyFormatEuroIE.format(fractionalNumber);

        // Formatted so that fraction digits are always included.
        var currencyFormatUSD1 = new Windows.Globalization.NumberFormatting.CurrencyFormatter(Windows.Globalization.CurrencyIdentifiers.usd);
        currencyFormatUSD1.fractionDigits = 2;
        var currencyUSD1 = currencyFormatUSD1.format(wholeNumber);

        // Formatted so that integer grouping separators are included.
        var currencyFormatUSD2 = new Windows.Globalization.NumberFormatting.CurrencyFormatter(Windows.Globalization.CurrencyIdentifiers.usd);
        currencyFormatUSD2.isGrouped = 1;
        var currencyUSD2 = currencyFormatUSD2.format(fractionalNumber);

        // Formatted using currency code instead of currency symbol
        var currencyFormatEuroModeSwitch = new Windows.Globalization.NumberFormatting.CurrencyFormatter(Windows.Globalization.CurrencyIdentifiers.eur);
        currencyFormatEuroModeSwitch.mode = Windows.Globalization.NumberFormatting.CurrencyFormatterMode.useCurrencyCode;
        var currencyEuroCode = currencyFormatEuroModeSwitch.format(fractionalNumber);

        // Return back to currency symbol
        currencyFormatEuroModeSwitch.mode = Windows.Globalization.NumberFormatting.CurrencyFormatterMode.useSymbol;
        var currencyEuroSymbol = currencyFormatEuroModeSwitch.format(fractionalNumber);

        // Display the results.
        var results = "Fixed number (" + fractionalNumber + ")\n" +
                      "With user's default currency: " + currencyDefault + "\n" +
                      "Formatted US Dollar: " + currencyUSD + "\n" +
                      "Formatted Euro (fr-FR defaults): " + currencyEuroFR + "\n" +
                      "Formatted Euro (gd-IE defaults): " + currencyEuroIE + "\n" +
                      "Formatted US Dollar (with fractional digits): " + currencyUSD1 + "\n" +
                      "Formatted US Dollar (with grouping separators): " + currencyUSD2 + "\n" +
                      "Formatted Euro (as currency code): " + currencyEuroCode + "\n" +
                      "Formatted Euro (as symbol): " + currencyEuroSymbol;

        document.getElementById("output").innerText = results;
    }
})();
