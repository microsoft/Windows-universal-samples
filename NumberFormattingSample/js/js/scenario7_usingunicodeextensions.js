//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario7_UsingUnicodeExtensions.html", {
        ready: function (element, options) {
            document.getElementById("displayButton").addEventListener("click", doDisplay, false);
       }
    });

    function doDisplay() {
        // This scenario uses language tags with unicode extensions to construct and use the various 
        // formatters and NumeralSystemTranslator in Windows.Globalization.NumberFormatting to format numbers 

        // Make a random number.
        var randomNumber = (Math.random() * 100000);
        // Var to keep results of the scenario
        var results = "Random numbers used by Formatters: " + randomNumber + "\n";
        
        // Create a string to translate
        var stringToTranslate = "These are the 10 digits of a numeral system: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9";
        results += "String used by NumeralSystemTranslater: " + stringToTranslate + "\n";
        
        // Create the rounder and set its increment to 0.001
        var rounder = new Windows.Globalization.NumberFormatting.IncrementNumberRounder();
        rounder.increment = 0.001;
        results += "CurrencyFormatter will be using Euro symbol and all formatters rounding to 0.001 increments" + "\n\n";
        
        // The list of language tags with Unicode extensions we want to test
        var languages = ["de-DE-u-nu-telu-ca-buddist-co-phonebk-cu-usd", "ja-jp-u-nu-arab"];
        
        // Create the various formatters, now using the language list with unicode extensions         
        results += "Creating formatters using following languages in the language list:\n";
        for (var i = 0; i < languages.length; i++) {
            results += "     " + languages[i] + "\n";
        }
        results += "\n";

        // Create the various formatters.
        var decimalFormatter = new Windows.Globalization.NumberFormatting.DecimalFormatter(languages, "ZZ");
        decimalFormatter.numberRounder = rounder; decimalFormatter.fractionDigits = 2;
        var currencyFormatter = new Windows.Globalization.NumberFormatting.CurrencyFormatter(Windows.Globalization.CurrencyIdentifiers.eur, languages, "ZZ");
        currencyFormatter.numberRounder = rounder; currencyFormatter.fractionDigits = 2;
        var percentFormatter = new Windows.Globalization.NumberFormatting.PercentFormatter(languages, "ZZ");
        percentFormatter.numberRounder = rounder; percentFormatter.fractionDigits = 2;
        var permilleFormatter = new Windows.Globalization.NumberFormatting.PermilleFormatter(languages, "ZZ");
        permilleFormatter.numberRounder = rounder; permilleFormatter.fractionDigits = 2;
        var numeralTranslator = new Windows.Globalization.NumberFormatting.NumeralSystemTranslator(languages);

        results += "Using resolved language  " + decimalFormatter.resolvedLanguage + "  : (note that all extension tags other than nu are ignored)" + "\n";
        // Format using formatters and translate using NumeralSystemTranslator.
        results += "Decimal Formatter:   " + decimalFormatter.format(randomNumber) + "\n";
        results += "Currency formatted:   " + currencyFormatter.format(randomNumber) + "\n";
        results += "Percent formatted:   " + percentFormatter.format(randomNumber) + "\n";
        results += "Permille formatted:   " + permilleFormatter.format(randomNumber) + "\n";
        results += "NumeralTranslator translated:   " + numeralTranslator.translateNumerals(stringToTranslate) + "\n\n";

        // Display the results.
        document.getElementById("output").innerText = results;
    }

})();
