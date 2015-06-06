//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var reverseConversionGenerator = Windows.Data.Text.TextReverseConversionGenerator;
    var notifyText = "";

    var page = WinJS.UI.Pages.define("/html/Scenario3_ReverseConversion.html", {
        ready: function (element, options) {
            document.getElementById("createGeneratorButton").addEventListener("click", createGenerator, false);
            document.getElementById("executeButton").addEventListener("click", executePredicton, false);
        }
    });

    function createGenerator() {

        // Try to parse the language and create reverseConversionGenerator accordingly.
        reverseConversionGenerator = new Windows.Data.Text.TextReverseConversionGenerator(langTagBox.value);

        // Only allow reverseConversionGenerator operation when the language is available and installed.
        if (!reverseConversionGenerator) {
            if (generatorOperationArea != null) {
                generatorOperationArea.style.visibility = "hidden";
            }

            notifyText = "\nFailed to instantiate a Generator.";
            WinJS.log && WinJS.log(notifyText, "sample", "error");
        }
        else if (reverseConversionGenerator.resolvedLanguage === "und") {
            if (generatorOperationArea != null) {
                generatorOperationArea.style.visibility = "hidden";
            }

            notifyText = "\nUnsupported language.";
            WinJS.log && WinJS.log(notifyText, "sample", "error");
        }
        else if (reverseConversionGenerator.languageAvailableButNotInstalled) {
            if (generatorOperationArea != null) {
                generatorOperationArea.style.visibility = "hidden";
            }

            notifyText = "\nLanguage available but not installed.";
            WinJS.log && WinJS.log(notifyText, "sample", "error");
        }
        else {
            if (generatorOperationArea != null) {
                generatorOperationArea.style.visibility = "";
            }

            notifyText = "\nSuccessfully instantiated a Generator.";
            WinJS.log && WinJS.log(notifyText, "sample", "status");
        }
        return;
    }

    function executePredicton() {
        // Clean up result column.
        if (resultList.firstChild) {
            resultList.removeChild(resultList.firstChild);
        }

        // Clean notification area.
        WinJS.log && WinJS.log("", "sample", "status");

        var input = InputStringBox.value;
        var selectedItem = generationMethodSelector.selectedIndex;

        if (reverseConversionGenerator && !isNaN(selectedItem) && input) {
            // Call the API with max candidate number we expect, and list the result when there are any candidates.
            // Suspending the handling until the unsynchronous call finishes.
            reverseConversionGenerator.convertBackAsync(input)
              .done(function (result) {
                  var node = document.createElement("li");
                  var textnode = document.createTextNode(result);
                  node.appendChild(textnode);
                  resultList.appendChild(node);

                  if (result.length === 0) {
                      var notifyText = "\nNo candidates.";
                      WinJS.log && WinJS.log(notifyText, "sample", "status");
                  }
              });
        }
    }
})();
