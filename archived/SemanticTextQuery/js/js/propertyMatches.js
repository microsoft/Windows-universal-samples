//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/propertyMatches.html", {
        ready: function (element, options) {
            document.getElementById("find").addEventListener("click", find, false);
        }
    });

    function find() {
        var searchFilter = queryBox.value;
        var properties = ["System.Title", "System.Author", "System.Comment"];
        var mySemanticTextQuery = new Windows.Data.Text.SemanticTextQuery(searchFilter);

        // Look for the matches in the properties
        properties.forEach(function (propertyName) {  
            var output = document.getElementById(propertyName);
            var content = output.innerText;
            var ranges = mySemanticTextQuery.findInProperty(content, propertyName);
            var highlightedString = SdkSample.highlightString(content, ranges);
            output.innerHTML = highlightedString;
        });
    }
})();
