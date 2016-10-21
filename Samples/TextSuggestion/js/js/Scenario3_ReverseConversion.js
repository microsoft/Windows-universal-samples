//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var reverseConversionGenerator = Windows.Data.Text.TextReverseConversionGenerator;
    var notifyText = "";

    var GenerationMethod = {
        ConvertBackAsync: 0,
        GetPhonemesAsync: 1
    };

    var page = WinJS.UI.Pages.define("/html/Scenario3_ReverseConversion.html", {
        ready: function (element, options) {
            document.getElementById("createGeneratorButton").addEventListener("click", createGenerator, false);
            document.getElementById("executeButton").addEventListener("click", executeReverseConversion, false);
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
        } else if (reverseConversionGenerator.resolvedLanguage === "und") {
            if (generatorOperationArea != null) {
                generatorOperationArea.style.visibility = "hidden";
            }

            notifyText = "\nUnsupported language.";
            WinJS.log && WinJS.log(notifyText, "sample", "error");
        } else if (reverseConversionGenerator.languageAvailableButNotInstalled) {
            if (generatorOperationArea != null) {
                generatorOperationArea.style.visibility = "hidden";
            }

            notifyText = "\nLanguage available but not installed.";
            WinJS.log && WinJS.log(notifyText, "sample", "error");
        } else {
            if (generatorOperationArea != null) {
                generatorOperationArea.style.visibility = "";
            }

            notifyText = "\nSuccessfully instantiated a Generator.";
            WinJS.log && WinJS.log(notifyText, "sample", "status");
        }
    }

    function executeReverseConversion() {
        // Clean up result column.
        while (resultList.firstChild) {
            resultList.removeChild(resultList.firstChild);
        }

        // Clean notification area.
        WinJS.log && WinJS.log("", "sample", "status");

        var input = InputStringBox.value,
            selectedItem = generationMethodSelector.selectedIndex;

        if (reverseConversionGenerator && !isNaN(selectedItem) && input) {

            // Specify the candidate number to get.
            if (selectedItem === GenerationMethod.ConvertBackAsync) {
                // Call the reverse conversion API, and list the results.
                reverseConversionGenerator.convertBackAsync(input)
                  .done(function (result) {
                      var node = document.createElement("li"),
                          textnode = document.createTextNode(result);
                      node.appendChild(textnode);
                      resultList.appendChild(node);

                      if (result.length === 0) {
                          var notifyText = "\nNo results.";
                          WinJS.log && WinJS.log(notifyText, "sample", "status");
                      }
                  });
            }
            else {
                // Call the reverse conversion per phoneme API, and list the results.
                reverseConversionGenerator.getPhonemesAsync(input)
                .done(function (phonemes) {
                    for (var index = 0; index < phonemes.length; index++) {
                        var node = document.createElement("li"),
                            textnode = document.createTextNode(phonemes[index].displayText + " -> " + phonemes[index].readingText);
                        node.appendChild(textnode);
                        resultList.appendChild(node);
                    }

                    if (phonemes.length === 0) {
                        var notifyText = "\nNo results.";
                        WinJS.log && WinJS.log(notifyText, "sample", "status");
                    }
                });
            }
        }
    }
})();
