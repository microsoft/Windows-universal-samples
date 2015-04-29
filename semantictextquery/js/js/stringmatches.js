//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/stringMatches.html", {
        ready: function (element, options) {
            document.getElementById("find").addEventListener("click", find, false);
        }
    });

    function find() {  
        var output = document.getElementById("output");
        var queryTextBox = document.getElementById("queryBox");

        // Retrieve the query entered in the textbox
        var searchFilter = queryTextBox.value;
        var content = output.innerText;

        // Look for the matches in the content
        var mySemanticTextQuery = new Windows.Data.Text.SemanticTextQuery(searchFilter, "en-us");
        var ranges = mySemanticTextQuery.find(content);
        
        var newString;

        // Make the matches bold 
        if (ranges.size > 0) {
            newString = SdkSample.highlightString(content, ranges);
        } else {
            newString = content + "<br><br/> No matches were found for your query. Please search again.";
        }
        output.innerHTML = newString;
    }
})();
