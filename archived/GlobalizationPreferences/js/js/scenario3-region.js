//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var outputText;

    var page = WinJS.UI.Pages.define("/html/scenario3-region.html", {
        ready: function (element, options) {
            document.getElementById("showResults").addEventListener("click", showResults);
            outputText = document.getElementById("outputText");
        }
    });

    function reportRegionData(region) {
        return  "Display Name: " + region.displayName + "\n" +
            "Native Name: " + region.nativeName + "\n" +
            "Currencies in use: " + region.currenciesInUse + "\n" +
            "Codes: " + region.codeTwoLetter + ", " + region.codeThreeLetter + ", " + region.codeThreeDigit + "\n\n";
    }

    function showResults() {
        // This scenario uses the Windows.Globalization.GeographicRegion class to
        // obtain the geographic region characteristics.
        var userGeoRegion = new Windows.Globalization.GeographicRegion();

        // This obtains the geographic region characteristics by providing a country or region code.
        var exampleGeoRegion = new Windows.Globalization.GeographicRegion("JP");

        // Display the results
        outputText.innerText = "User's Preferred Geographic Region\n" + reportRegionData(userGeoRegion) +
            "Example Geographic Region by region/country code (JP)\n" + reportRegionData(exampleGeoRegion);
    }
})();
