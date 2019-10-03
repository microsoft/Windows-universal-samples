//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Semantic text query JS sample";

    var scenarios = [
        { url: "/html/stringMatches.html", title: "Find Scenario" },
        { url: "/html/propertyMatches.html", title: "FindInProperty Scenario" },
        { url: "/html/filePropertiesMatches.html", title: "GetMatchingPropertiesWithRanges Scenario" }
    ];

    // Helper element to sanitize strings.
    var spanElement = document.createElement("span");

    function sanitizeString(originalString)
    {
        spanElement.innerText = originalString;
        return spanElement.innerHTML;
    }

    // Method used to add bold tags to matches on a string.
    function highlightString(originalString, rangesToHighlight) {
        var pointerPosition = 0;
        var newString = "";
        if (rangesToHighlight && rangesToHighlight.size > 0) {
            var currentAfterPosition = 0;
            rangesToHighlight.forEach(function (textRange) {
                newString += sanitizeString(originalString.slice(pointerPosition, textRange.startPosition));
                currentAfterPosition = textRange.startPosition + textRange.length;
                var temp = "<b>" + sanitizeString(originalString.slice(textRange.startPosition, currentAfterPosition)) + "</b>";
                newString += temp;
                pointerPosition = currentAfterPosition;
            });
            if (pointerPosition !== originalString.length) {
                newString += sanitizeString(originalString.slice(pointerPosition, originalString.length));
            }
        }
        else {
            newString = sanitizeString(originalString);
        }

        return newString;
    }

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        highlightString: highlightString
    });
})();
