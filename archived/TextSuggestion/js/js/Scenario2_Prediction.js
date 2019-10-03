//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var predictionGenerator = Windows.Data.Text.TextPredictionGenerator;
    var notifyText = "";

    var GenerationMethod = {
        GetCandidatesAsync: 0,
        GetCandidatesAsyncWithMaxCandidates: 1
    };

    var page = WinJS.UI.Pages.define("/html/Scenario2_Prediction.html", {
        ready: function (element, options) {
            document.getElementById("createGeneratorButton").addEventListener("click", createGenerator, false);
            document.getElementById("generationMethodSelector").addEventListener("change", selectGenerationMethod, false);
            document.getElementById("executeButton").addEventListener("click", executePredicton, false);
        }
    });

    function createGenerator() {

        // Try to parse the language and create predictionGenerator accordingly.
        predictionGenerator = new Windows.Data.Text.TextPredictionGenerator(langTagBox.value);

        // Only allow predictionGenerator operation when the language is available and installed.
        if (!predictionGenerator) {
            if (generatorOperationArea != null) {
                generatorOperationArea.style.visibility = "hidden";
            }

            notifyText = "\nFailed to instantiate a Generator.";
            WinJS.log && WinJS.log(notifyText, "sample", "error");
        } else if (predictionGenerator.resolvedLanguage === "und") {
            if (generatorOperationArea != null) {
                generatorOperationArea.style.visibility = "hidden";
            }

            notifyText = "\nUnsupported language.";
            WinJS.log && WinJS.log(notifyText, "sample", "error");
        } else if (predictionGenerator.languageAvailableButNotInstalled) {
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

    function selectGenerationMethod() {
        if (maxCandidatesArea !== null) {

            // Only show the candidate number input control when user chooses to specify the number.
            if (generationMethodSelector.selectedIndex === GenerationMethod.GetCandidatesAsync) {
                maxCandidatesArea.style.display = "none";
            } else {
                maxCandidatesArea.style.display = "block";
            }
        }
    }

    function executePredicton() {
        // Clean up result column.
        while (resultList.firstChild) {
            resultList.removeChild(resultList.firstChild);
        }

        // Clean notification area.
        WinJS.log && WinJS.log("", "sample", "status");

        var input = InputStringBox.value;
        var selectedItem = generationMethodSelector.selectedIndex;

        if (predictionGenerator && !isNaN(selectedItem) && input) {

            // Specify the candidate number to get.
            if (selectedItem === GenerationMethod.GetCandidatesAsyncWithMaxCandidates) {
                var maxCandidates = parseInt(maxCandidatesBox.value, 0);
                if (!isNaN(maxCandidates) && (maxCandidates > 0)) {

                    // Call the API with max candidate number we expect, and list the results.
                    predictionGenerator.getCandidatesAsync(input, maxCandidates)
                      .done(function (result) {
                          displayResults(result);
                      });
                } else {
                    var notifyText = "\nInvalid Candidate Number.";
                    WinJS.log && WinJS.log(notifyText, "sample", "error");
                }
            } else {

                // Call the API with default candidate number, and list the results.
                predictionGenerator.getCandidatesAsync(input)
                      .done(function (result) {
                          displayResults(result);
                      });
            }
        }
    }

    function displayResults(result) {
        for (var index = 0; index < result.length; index++) {
            var node = document.createElement("li");
            var textnode = document.createTextNode(result[index]);
            node.appendChild(textnode);
            resultList.appendChild(node);
        }

        if (result.length === 0) {
            var notifyText = "\nNo candidates.";
            WinJS.log && WinJS.log(notifyText, "sample", "status");
        }
    }
})();
