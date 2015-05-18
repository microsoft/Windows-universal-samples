//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario5_RoundingAndPadding.html", {
        ready: function (element, options) {
            document.getElementById("displayButton").addEventListener("click", doDisplay, false);
        }
    });
    
    // Used to display the rounding algorithm in a string format.  Used by the different scenarios when 
    // displaying results for the scenario
    function displayRoundingAlgorithmAsString(roundingAlgorithm) {
        var rounding = Windows.Globalization.NumberFormatting.RoundingAlgorithm;
        var roundingAsString = "";

        // Convert the enumeration value into a string representation
        switch (roundingAlgorithm) {
            case rounding.none:
                roundingAsString = "no";
                break;
            case rounding.roundAwayFromZero:
                roundingAsString = "away from zero";
                break;
            case rounding.roundDown:
                roundingAsString = "down";
                break;
            case rounding.roundHalfAwayFromZero:
                roundingAsString = "half away from zero";
                break;
            case rounding.roundHalfDown:
                roundingAsString = "half down";
                break;
            case rounding.roundHalfToEven:
                roundingAsString = "half to even";
                break;
            case rounding.roundHalfToOdd:
                roundingAsString = "half to odd";
                break;
            case rounding.roundHalfTowardsZero:
                roundingAsString = "half towards zero";
                break;
            case rounding.roundHalfUp:
                roundingAsString = "half up";
                break;
            case rounding.roundTowardsZero:
                roundingAsString = "toward zero";
                break;
            case rounding.roundUp:
                roundingAsString = "up";
                break;
            default:
                roundingAsString = "unknown";
                break;
        }

        return roundingAsString;
    }

    // Executes the padding scenarios.  The formatter parameter can be either a DecimalFormatter, PercentFormatter
    // PerMilleFormatter or CurrencyFormatter.
    function doPaddingScenarios(formatter) {
        // Display the properties of the scenario
        var results = "Padding with ";
        results = results + formatter.significantDigits + " significant digits, ";
        results = results + formatter.integerDigits + " integer digits, ";
        results = results + formatter.fractionDigits + " fractional digits\n\n";

        // Iterate through the numbers to pad, format them and add them to the results
        var numbersToPad = [0.12, 1.2, 10.2, 102];
        for (var index in numbersToPad) {
            var numberToPad = numbersToPad[index];
            var paddedNumber = formatter.format(numberToPad);
            results = results + "Value: " + numberToPad + " Padded: " + paddedNumber + "\n";
        }

        // Add a carriage return at the end of the scenario for readability
        return results + "\n";
    }

    // Demonstrates how to perform padding and rounding by using the DecimalFormatter class (can be any of the 
    // formatter classes in the Windows.Globalization.Numberformatting namespace) and the IncrementNumberRounder
    // to do so.  The SignificantDigitsNumberRounder can also be used instead of the latter.
    function doPaddingAndRoundingScenarios(roundingAlgorithm, significantDigits, integerDigits, fractionalDigits) {
        // Create the rounder and set the rounding algorithm provided as a parameter
        var rounder = new Windows.Globalization.NumberFormatting.IncrementNumberRounder();
        rounder.roundingAlgorithm = roundingAlgorithm;

        // Create the formatter, set the padding properties provided as parameters and associate the rounder
        // we have just created
        var formatter = new Windows.Globalization.NumberFormatting.DecimalFormatter();
        formatter.significantDigits = significantDigits;
        formatter.integerDigits = integerDigits;
        formatter.fractionDigits = fractionalDigits;
        formatter.numberRounder = rounder;

        // Stores the results
        var results = "";

        // Iterate through the increments we have defined in the scenario
        var incrementsToRound = [0.001, 0.01, 0.1, 1.0];
        for (var incrementIndex in incrementsToRound) {
            // Set the increment to round to
            rounder.increment = incrementsToRound[incrementIndex];

            // Display the properties of the scenario
            results = results + "Padding and rounding with ";
            results = results + formatter.significantDigits + " significant digits, ";
            results = results + formatter.integerDigits + " integer digits, ";
            results = results + formatter.fractionDigits + " fractional digits, ";
            results = results + rounder.increment + " increment and ";
            results = results + displayRoundingAlgorithmAsString(rounder.roundingAlgorithm) + " rounding algorithm\n\n";

            // Iterate through the numbers to round and pad, format them and add them to the results
            var numbersToFormat = [0.1458, 1.458, 14.58, 145.8];
            for (var numberIndex in numbersToFormat) {
                var numberToFormat = numbersToFormat[numberIndex];
                var formattedNumber = formatter.format(numberToFormat);
                results = results + "Value: " + numberToFormat + " Formatted: " + formattedNumber + "\n";
            }

            // Add a carriage return at the end of the scenario for readability
            results = results + "\n";
        }

        return results;
    }

    // This scenario illustrates how to provide a rounding algorithm to a CurrencyFormatter class by means of the 
    // applyRoundingForCurrency method.  This will associate an IncrementNumberRounder using the same increment as the
    // fractionDigits appropriate for the currency and language.  You can always choose to provide a different rounding 
    // algorithm by setting the numberRounder property, as depicted in the doPaddingAndRoundingScenarios function.  
    // The mechanism provided here is better suited for currencies.
    function doCurrencyRoundingScenarios(currencyCode, roundingAlgorithm) {
        // Create the currency formatter and set the rounding algorithm provided as a parameter
        var currencyFormatter = new Windows.Globalization.NumberFormatting.CurrencyFormatter(currencyCode);
        currencyFormatter.applyRoundingForCurrency(roundingAlgorithm);

        // Display the properties of the scenario
        var results = "Currency formatting for ";
        results = results + currencyFormatter.currency + " currency and using the ";
        results = results + displayRoundingAlgorithmAsString(roundingAlgorithm) + " rounding algorithm\n\n";

        // Iterate through the numbers to round and add them to the results
        var numbersToFormat = [14.55, 3.345, 16.2, 175.8];
        for (var numberIndex in numbersToFormat) {
            var numberToFormat = numbersToFormat[numberIndex];
            var formattedNumber = currencyFormatter.format(numberToFormat);
            results = results + "Value: " + numberToFormat + " Formatted: " + formattedNumber + "\n";
        }

        // Add a carriage return at the end of the scenario for readability
        return results + "\n";
    }

    // Shows how to do number rounding using the IncrementNumberRounding class.
    function doIncrementRoundingScenarios(roundingAlgorithm) {
        // Create the rounder and set the rounding algorithm provided as a parameter
        var rounder = new Windows.Globalization.NumberFormatting.IncrementNumberRounder();
        rounder.roundingAlgorithm = roundingAlgorithm;

        // Formatter to display the rounded value.  It is recommended to use the increment number
        // rounder within a formatter object to avoid precision issues when displaying.
        var formatter = new Windows.Globalization.NumberFormatting.DecimalFormatter();
        formatter.numberRounder = rounder;

        // Stores the results
        var results = "";

        // Iterate through the increments we have defined in the scenario
        var incrementsToRound = [0.001, 0.01, 0.1, 1.0];
        for (var incrementIndex in incrementsToRound) {
            // Set the significant digits to round to
            rounder.increment = incrementsToRound[incrementIndex];

            // Display the properties of the scenario
            results = results + "Rounding with ";
            results = results + rounder.increment + " increment and ";
            results = results + displayRoundingAlgorithmAsString(rounder.roundingAlgorithm) + " rounding algorithm\n\n";

            // Iterate through the numbers to round and add them to the results
            var numbersToRound = [0.1458, 1.458, 14.58, 145.8];
            for (var numberIndex in numbersToRound) {
                var numberToRound = numbersToRound[numberIndex];
                results = results + "Value: " + numberToRound + " Rounded: " + formatter.format(numberToRound) + "\n";
            }

            // Add a carriage return at the end of the scenario for readability
            results = results + "\n";
        }

        return results;
    }

    // Shows how to do number rounding using the SignificantDigitsNumberRounder class.
    function doSignificantDigitRoundingScenarios(roundingAlgorithm) {
        // Create the rounder and set the rounding algorithm provided as a parameter.
        var rounder = new Windows.Globalization.NumberFormatting.SignificantDigitsNumberRounder();
        rounder.roundingAlgorithm = roundingAlgorithm;

        // Stores the results
        var results = "";

        // Iterate through the significant digits we have defined in the scenario
        var digitsToRound = [3, 2, 1];
        for (var digitIndex in digitsToRound) {
            // Set the significant digits to round to
            rounder.significantDigits = digitsToRound[digitIndex];

            // Display the properties of the scenario
            results = results + "Rounding with ";
            results = results + rounder.significantDigits + " significant digits and ";
            results = results + displayRoundingAlgorithmAsString(rounder.roundingAlgorithm) + " rounding algorithm\n\n";

            // Iterate through the numbers to round and add them to the results
            var numbersToRound = [0.1458, 1.458, 14.58, 145.8];
            for (var numberIndex in numbersToRound) {
                var numberToRound = numbersToRound[numberIndex];
                var roundedValue = rounder.roundDouble(numberToRound);
                results = results + "Value: " + numberToRound + " Rounded: " + roundedValue + "\n";
            }

            // Add a carriage return at the end of the scenario for readability
            results = results + "\n";
        }

        return results;
    }

    // Executes all of the scenarios for rounding and padding of numbers.
    function doDisplay() {
        // Variable where we keep results to display
        var results = "";

        // Create a decimal formatter used to do padding with only 4 significant digits.  We zero the
        // integerDigits and fractionDigits to only show the behavior of the significantDigits property
        // when formatting.
        var decimalFormatter = new Windows.Globalization.NumberFormatting.DecimalFormatter();
        decimalFormatter.significantDigits = 4;
        decimalFormatter.integerDigits = 0;
        decimalFormatter.fractionDigits = 0;

        // Run through the scenarios with padding
        results = results + doPaddingScenarios(decimalFormatter);

        // Create a percent formatter used to do padding with 4 significant digits, 2 minimum integer 
        // digits, 2 minimum factional digits.  This will show how the behavior of the significantDigits
        // property in conjunction with the integerDigits and fractionDigits properties.
        var percentFormatter = new Windows.Globalization.NumberFormatting.PercentFormatter();
        percentFormatter.significantDigits = 4;
        percentFormatter.integerDigits = 2;
        percentFormatter.fractionDigits = 2;

        // Run through the scenarios
        results = results + doPaddingScenarios(percentFormatter);

        // Do the significant rounding scenarios with round up algorithm 
        results = results + doSignificantDigitRoundingScenarios(Windows.Globalization.NumberFormatting.RoundingAlgorithm.roundUp);

        // Do the significant rounding scenarios with round down algorithm
        results = results + doSignificantDigitRoundingScenarios(Windows.Globalization.NumberFormatting.RoundingAlgorithm.roundDown);

        // Do the increment rounding scenarios with round half up algorithm
        results = results + doIncrementRoundingScenarios(Windows.Globalization.NumberFormatting.RoundingAlgorithm.roundHalfUp);

        // Do the increment rounding scenarios with round half down algorithm
        results = results + doIncrementRoundingScenarios(Windows.Globalization.NumberFormatting.RoundingAlgorithm.roundHalfDown);

        // Do currency formatting with rounding for Japanese Yen with round half to odd algorithm
        results = results + doCurrencyRoundingScenarios("JPY", Windows.Globalization.NumberFormatting.RoundingAlgorithm.roundHalfToOdd);

        // Do currency formatting with rounding for Euro with round half to even algorithm
        results = results + doCurrencyRoundingScenarios("EUR", Windows.Globalization.NumberFormatting.RoundingAlgorithm.roundHalfToEven);

        // Do padding and rounding scenarios using round half up algorithm
        results = results + doPaddingAndRoundingScenarios(Windows.Globalization.NumberFormatting.RoundingAlgorithm.roundHalfUp, 4, 3, 2);

        // Do padding and rounding scenarios using round half down algorithm
        results = results + doPaddingAndRoundingScenarios(Windows.Globalization.NumberFormatting.RoundingAlgorithm.roundHalfDown, 4, 3, 2);

        // Display the results
        document.getElementById("output").innerText = results;
    }
})();
