//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Number formatting JS sample";

    var scenarios = [{
    url: "/html/scenario1_PercentPermilleFormatting.html",
    title: "Percent and Permille Formatting"
}, {
    url: "/html/scenario2_DecimalFormatting.html",
    title: "Decimal Formatting"
}, {
    url: "/html/scenario3_CurrencyFormatting.html",
    title: "Currency Formatting"
}, {
    url: "/html/scenario4_NumberParsing.html",
    title: "Number Parsing"
}, {
    url: "/html/scenario5_RoundingAndPadding.html",
    title: "Rounding and Padding"
}, {
    url: "/html/scenario6_NumeralSystemTranslation.html",
    title: "Numeral System Translation"
}, {
    url: "/html/scenario7_UsingUnicodeExtensions.html",
    title: "Formatting/Translation using Unicode Extensions"
}];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();
